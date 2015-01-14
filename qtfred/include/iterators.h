#pragma once
/*! \file Iterators on FS2 classes. */
#include <iterator>
#include <type_traits>
#include "globalincs/linklist.h"

namespace fso {
namespace fred {

#ifdef _MSC_VER
    // MSVC apparently doesn't like declval (this will cause issues with derived classes)
template<typename T, typename element_type = T*>
#else
template<typename T, typename element_type = decltype(std::declval<T>().prev)>
#endif
struct iterator : public std::iterator<std::bidirectional_iterator_tag, T>
{
    iterator(const T &t): current{GET_FIRST(&t)} {}
    iterator(const iterator &rhs): current{rhs.current} {}
    iterator &operator++() { current = GET_NEXT(current); return *this; }
    iterator &operator--() { current = GET_PREV(current); return *this; }
    iterator &operator++(int) { iterator lhs(*this); operator++(); return lhs; }
    iterator &operator--(int) { iterator lhs(*this); operator--(); return lhs; }

    bool operator ==(const iterator &rhs) { return current == rhs.current; }
    bool operator !=(const iterator &rhs) { return !operator==(rhs); }
    element_type &operator*() { return current; }

private:
    element_type current;
};

template<
    typename T,
    typename Tptr = typename std::add_const<typename std::add_pointer<T>::type>::type,
    typename prev_type = decltype(std::declval<T>().prev),
    typename next_type = decltype(std::declval<T>().next)>
typename std::enable_if<
    std::is_convertible<Tptr, prev_type>::value
    && std::is_convertible<Tptr, next_type>::value
    && std::is_same<prev_type, next_type>::value, iterator<T>>::type
begin(const T&t) {
    return iterator<T>(t);
}

template<
    typename T,
    typename Tptr = typename std::add_const<typename std::add_pointer<T>::type>::type,
    typename prev_type = decltype(std::declval<T>().prev),
    typename next_type = decltype(std::declval<T>().next)>
typename std::enable_if<
    std::is_convertible<Tptr, prev_type>::value
    && std::is_convertible<Tptr, next_type>::value
    && std::is_same<prev_type, next_type>::value, iterator<T>>::type
end(const T&t) {
    iterator<T> last(t);
    --last; // make the iterator points to the head, which is considered out of list.

    return last;
}

} // namespace fred
} // namespace fso

