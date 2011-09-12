#include "errors.hpp"
#include <boost/make_shared.hpp>

/* `metadata_member_read_view_t` and `metadata_member_readwrite_view_t`
correspond to some values of a `std::map` contained in another metadata view. */

template<class key_t, class value_t>
class metadata_member_read_view_t : public metadata_read_view_t<value_t> {

public:
    metadata_member_read_view_t(key_t k, boost::shared_ptr<metadata_read_view_t<std::map<key_t, value_t> > > sv) :
        key(k), superview(sv)
    {
        rassert(superview->get().count(key) != 0);
    }

    value_t get() {
        return superview->get()[key];
    }

    publisher_t<boost::function<void()> > *get_publisher() {
        return superview->get_publisher();
    }

private:
    key_t key;
    boost::shared_ptr<metadata_read_view_t<std::map<key_t, value_t> > > superview;
    DISABLE_COPYING(metadata_member_read_view_t);
};

template<class key_t, class value_t>
class metadata_member_readwrite_view_t : public metadata_readwrite_view_t<value_t> {

public:
    metadata_member_readwrite_view_t(key_t k, boost::shared_ptr<metadata_readwrite_view_t<std::map<key_t, value_t> > > sv) :
        key(k), superview(sv)
    {
        rassert(superview->get().count(key) != 0);
    }

    value_t get() {
        return superview->get()[key];
    }

    void join(const value_t &v) {
        std::map<key_t, value_t> map = superview->get();
        semilattice_join(&map[key], v);
        superview->join(map);
    }

    publisher_t<boost::function<void()> > *get_publisher() {
        return superview->get_publisher();
    }

private:
    key_t key;
    boost::shared_ptr<metadata_readwrite_view_t<std::map<key_t, value_t> > > superview;
    DISABLE_COPYING(metadata_member_readwrite_view_t);
};

template<class key_t, class value_t>
boost::shared_ptr<metadata_read_view_t<value_t> > metadata_member(
    key_t key,
    boost::shared_ptr<metadata_read_view_t<std::map<key_t, value_t> > > outer)
{
    return boost::make_shared<metadata_member_read_view_t<key_t, value_t> >(
        key, outer);
}

template<class key_t, class value_t>
boost::shared_ptr<metadata_readwrite_view_t<value_t> > metadata_member(
    key_t key,
    boost::shared_ptr<metadata_readwrite_view_t<std::map<key_t, value_t> > > outer)
{
    return boost::make_shared<metadata_member_readwrite_view_t<key_t, value_t> >(
        key, outer);
}

template<class key_t, class value_t>
boost::shared_ptr<metadata_readwrite_view_t<value_t> > metadata_new_member(
    key_t key,
    boost::shared_ptr<metadata_readwrite_view_t<std::map<key_t, value_t> > > outer)
{
    rassert(outer->get().count(key) == 0);
    std::map<key_t, value_t> new_value;
    new_value.insert(std::pair<key_t, value_t>(key, value_t()));
    outer->join(new_value);
    return metadata_member(key, outer);
}