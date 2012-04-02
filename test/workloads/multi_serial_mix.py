#!/usr/bin/python
import multiprocessing, sys, workload_common, os, serial_mix, time
from vcoptparse import *

def child(opts, log_path):
    # This is run in a separate process
    import sys
    sys.stdout = sys.stderr = file(log_path, "w")
    print "Starting test against server at %s:%d..." % (opts["host"], opts["port"])
    with workload_common.make_memcache_connection(opts) as mc:
        serial_mix.test(opts, mc)
    print "Done with test."

op = serial_mix.option_parser_for_serial_mix()
op["num_testers"] = IntFlag("--num-testers", 16)
opts = op.parse(sys.argv)

shutdown_grace_period = 15

tester_log_dir = "multi_serial_mix_out"
if not os.path.isdir(tester_log_dir): os.mkdir(tester_log_dir)

processes = []
try:
    print "Starting %d child processes..." % opts["num_testers"]
    print "Writing output from child processes to %r" % tester_log_dir

    for id in xrange(opts["num_testers"]):

        log_path = os.path.join(tester_log_dir, "%d.txt" % id)

        opts2 = dict(opts)
        opts2["keysuffix"] = "_%d" % id   # Prevent collisions between tests

        process = multiprocessing.Process(target = child, args = (opts2, log_path))
        process.start()

        processes.append((process, id))

    print "Waiting for child processes..."

    start_time = time.time()
    def time_remaining():
        time_elapsed = time.time() - start_time
        # Give subprocesses lots of extra time
        return opts["duration"] * 2 - time_elapsed + 1

    for (process, id) in processes:
        tr = time_remaining()
        if tr <= 0: tr = shutdown_grace_period
        process.join(tr)

    stuck = sorted(id for (process, id) in processes
        if process.is_alive())
    failed = sorted(id for (process, id) in processes
        if not process.is_alive() and process.exitcode != 0)

    if stuck or failed:
        if len(stuck) == opts["num_testers"]:
            raise ValueError("All %d processes did not finish in time." % opts["num_testers"])
        elif len(failed) == opts["num_testers"]:
            raise ValueError("All %d processes failed." % opts["num_testers"])
        else:
            raise ValueError("Of processes [1 ... %d], the following did not finish in time: " \
                "%s and the following failed: %s" % (opts["num_testers"], stuck, failed))

finally:
    for (process, id) in processes:
        if process.is_alive():
            process.terminate()

print "Done."