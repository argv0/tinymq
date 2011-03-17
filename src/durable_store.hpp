#ifndef STORE_HPP_
#define STORE_HPP_

#include <string>
#include <boost/noncopyable.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/managed_mapped_file.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/mem_algo/rbtree_best_fit.hpp>
#include <boost/interprocess/offset_ptr.hpp>
#include <boost/interprocess/indexes/flat_map_index.hpp>
#include "tinymq_log.hpp"


namespace tinymq { 

namespace bip = boost::interprocess;

template <template <class> class index_type=bip::flat_map_index>
class store : public bip::basic_managed_mapped_file<
    char, 
    bip::rbtree_best_fit<bip::mutex_family, bip::offset_ptr<void> >,
    index_type > {
public:
    typedef typename bip::basic_managed_mapped_file<
        char,
        bip::rbtree_best_fit<bip::mutex_family, bip::offset_ptr<void> >,
        index_type>                          base_t;
    typedef typename base_t::segment_manager segment_manager_type;
    typedef bip::allocator<void, segment_manager_type> void_allocator_type;
private:
    std::string filename_;
    std::size_t filesize_;
public:
    store(const std::string& filename, std::size_t init_size) :
        base_t(bip::open_or_create, filename.c_str(), init_size),
        filename_(filename),
        filesize_(init_size) {
        if (!base_t::check_sanity()) {
            throw std::runtime_error("corrupt data file");
        }
    }
    virtual ~store() { 
        sync();
        bip::shared_memory_object::remove(filename_.c_str());

    }
public:
    const std::string& filename() const { return filename_; }
    std::size_t  filesize() { return filesize_; }

    virtual std::size_t free_memory() const { 
        return base_t::get_free_memory(); 
    }

    virtual void grow(std::size_t by) {
        //L_(info) << "growing store by " << by << " bytes";
        sync();
        filesize_ += by;
        bip::shared_memory_object::remove(filename_.c_str());
        base_t::grow(filename_.c_str(), filesize_);
        base_t newstore(bip::open_only, filename_.c_str());
        base_t::swap(newstore);
    }

    virtual void sync() { 
        base_t::flush();
    }
};

} // namespace tinymq

#endif  // include guard
