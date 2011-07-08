#ifndef __RIAK_RIAK_HPP__
#define __RIAK_RIAK_HPP__

#include "http/http.hpp"
#include <boost/tokenizer.hpp>

namespace riak {

// Data defintions:
// these definition include everything that will actually need to be serialized for the object
// bucket data definition

struct hook_t {
    enum lang_t {
        JAVASCRIPT,
        ERLANG
    } lang;
    std::string code;
};

struct bucket_t {
    std::string name;
    int n_val;
    bool allow_mult;
    bool last_write_wins;
    std::vector<hook_t> precommit;
    std::vector<hook_t> postcommit;
    int r, w, dw, rw; //Quorum values
    std::string backend;
};

class bucket_iterator_t {
    //TODO actually implement this instead of just duping the type checker
private:
    bucket_t bucket;
public:
    bool operator!=(bucket_iterator_t const &) {not_implemented(); return true;}
    bool operator==(bucket_iterator_t const &) {not_implemented(); return true;}
    bucket_iterator_t operator++() {not_implemented(); return *this;}
    bucket_iterator_t operator++(int) {not_implemented(); return *this;}
    bucket_t operator*() {not_implemented(); return bucket_t(); }
    bucket_t *operator->() {not_implemented(); return &bucket;};
};

struct link_t {
    std::string bucket;
    std::string key;
    std::string tag;

    std::string as_string();
};

struct object_t {
    std::string key;
    std::string content;
    std::string content_type;
    std::string meta_data;
    std::string ETag;
    int last_written;
    std::vector<link_t> links;
};

class object_iterator_t {
    //TODO actually implement this instead of just duping the type checker
private:
    object_t object;
public:
    bool operator!=(object_iterator_t const &) {not_implemented(); return true;}
    bool operator==(object_iterator_t const &) {not_implemented(); return true;}
    object_iterator_t operator++() {not_implemented(); return *this;}
    object_iterator_t operator++(int) {not_implemented(); return *this;}
    object_t operator*() {not_implemented(); return object_t(); }
    object_t *operator->() {not_implemented(); return &object;};
};

class riak_interface_t {
public:
    // Get a bucket by name
    bucket_t const &bucket_read(std::string) {
        crash("Not implementated");
    }

    bucket_t &bucket_write(std::string) {
        crash("Not implementated");
    }

    //Get all the buckets
    std::pair<bucket_iterator_t, bucket_iterator_t> buckets() { 
        crash("Not implementated");
    };

    //Get all the keys in a bucket
    std::pair<object_iterator_t, object_iterator_t> objects(std::string) { 
        crash("Not implementated");
    };

    const object_t &get_object(std::string, std::string) {
        crash("Not implementated");
    }

    const object_t &store_object(std::string, object_t&) {
        crash("Not implementated");
    }

    const object_t &delete_object(std::string, std::string) {
        crash("Not implementated");
    }
};

class riak_server_t : public http_server_t {
private:
    http_res_t handle(const http_req_t &);

private:
    riak_interface_t riak;

private:
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    typedef tokenizer::iterator tok_iterator;

//handlers for specific commands, really just to break up the code
private:
    http_res_t list_buckets(const http_req_t &);

    //http_res_t list_keys(const http_req_t &);
    http_res_t get_bucket(const http_req_t &);
    http_res_t set_bucket(const http_req_t &);
    http_res_t fetch_object(const http_req_t &);
    http_res_t store_object(const http_req_t &);
    http_res_t delete_object(const http_req_t &);
    http_res_t link_walk(const http_req_t &);
    http_res_t mapreduce(const http_req_t &);
    http_res_t luwak_props(const http_req_t &);
    http_res_t luwak_keys(const http_req_t &);
    http_res_t luwak_fetch(const http_req_t &);
    http_res_t luwak_store(const http_req_t &);
    http_res_t luwak_delete(const http_req_t &);
    http_res_t ping(const http_req_t &);
    http_res_t status(const http_req_t &);
    http_res_t list_resources(const http_req_t &);
};

}; //namespace riak

#endif
