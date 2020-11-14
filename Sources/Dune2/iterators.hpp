#pragma once

#include <functional>
#include <iterator>

namespace nr::dune2 {

template<typename T>
struct Iterator {
public:
    using difference_type = size_t;
    using value_type = T;
    using reference_type = value_type;
    using iterator_category = std::input_iterator_tag;

public:
    using ItemGetter = std::function<T(size_t)>;

public:
    Iterator(ItemGetter &&item_getter, size_t index)
        : getItem_{item_getter}
        , index_{index} {
    }

public:
    bool operator==(const Iterator &rhs) {
        return index_ == rhs.index_;
    }

    bool operator!=(const Iterator &rhs)
    { return index_ != rhs.index_; }

    const T operator*() const
    { return getItem_(index_); }

    Iterator &operator++() {
        index_ += 1;
        return *this;
    }

private:
    ItemGetter getItem_;
    size_t index_{0};
};

} // namespace nr::dune2
