#ifndef MEMOREL_PARTIALLY_ORDERED_TUPLE_H_
#define MEMOREL_PARTIALLY_ORDERED_TUPLE_H_

namespace memorel {
template <size_t N, typename... Args>
struct partially_ordered_tuple : public std::tuple<Args...> {
  bool operator<(const partially_ordered_tuple &o) const {
    return less_than(o);
  }

  bool operator==(const partially_ordered_tuple &o) const { return equal(o); }

  template <size_t I = 0>
  bool less_than(const partially_ordered_tuple &o) const {
    if (I < N) {
      if (std::get < I < N ? I : N > (*this) < std::get < I < N ? I : N > (o)) {
        return true;
      } else if (std::get < I < N ? I : N > (o) < std::get < I < N
                                            ? I
                                            : N > (*this)) {
        return false;
      } else {
        return less_than < I < N ? I + 1 : N > (o);
      }
    } else {
      return false;
    }
  }

  template <size_t I = 0> bool equal(const partially_ordered_tuple &o) const {
    if (I < N) {
      if (std::get < I < N ? I : N > (*this) != std::get < I < N ? I
                                                                 : N > (o)) {
        return false;
      } else {
        return equal < I < N ? I + 1 : N > (o);
      }
    } else {
      return true;
    }
  }
};
}

#endif
