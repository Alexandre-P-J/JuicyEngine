#pragma once
#include <limits>
#include <type_traits>
#include <vector>


template <typename unsigned_type>
class SparseSet {
    std::vector<unsigned_type> dense;
    std::vector<std::size_t> sparse;

public:
    static const constexpr std::size_t invalid_index =
        std::numeric_limits<std::size_t>::max();
    // default constructible
    SparseSet() { static_assert(std::is_unsigned<unsigned_type>::value); };
    // construct putting elements
    template <class... natural>
    SparseSet(natural const... numbers) {
        static_assert(std::is_unsigned<unsigned_type>::value);
        static_assert(std::is_same<natural..., unsigned_type>::value);
        dense.reserve(sizeof...(numbers));
        put(numbers...);
    }
    // get size of internal array of unique naturals
    inline std::size_t size() const noexcept { return dense.size(); }
    // get the internal array of unique naturals
    inline std::vector<unsigned_type> const &get_internal() const noexcept {
        return dense;
    };
    // get the index of the internal array where the given natural is stored or
    // ECS_id_null if its not in the set
    inline std::size_t get_internal_index(
        unsigned_type const natural) const noexcept {
        return has(natural) ? sparse[natural] : invalid_index;
    }

    std::size_t put(unsigned_type const natural) noexcept {
        if (!has(natural)) {
            auto index = dense.size();
            dense.push_back(natural);
            sparse.resize(std::max(sparse.size(), natural + 1), invalid_index);
            sparse[natural] = index;
            return index;
        }
        return sparse[natural];
    }

    // erases an element if its in the set. It does so swapping
    // the element to erase and the last element in the internal array,
    // then deletes the last array element.
    // returns the erased element old index and the old index of the element
    // that now occupies the erased element index. If the element its not in the
    // set returns a pair of invalid_index
    std::pair<std::size_t, std::size_t> erase(
        unsigned_type const natural) noexcept {
        if (has(natural)) {
            auto last_element_value = dense.back();
            auto former_element_dense_pos = sparse[natural];
            dense[former_element_dense_pos] = last_element_value;
            sparse[natural] = invalid_index;
            sparse[last_element_value] = former_element_dense_pos;
            dense.pop_back();
            return std::make_pair(former_element_dense_pos, dense.size());
        }
        return std::make_pair(invalid_index, invalid_index);
    }

    inline bool has(unsigned_type const natural) const noexcept {
        return (sparse.size() > natural) && (dense.size() > sparse[natural]) &&
               (dense[sparse[natural]] == natural);
    }
};

