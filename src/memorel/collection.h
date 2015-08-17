#ifndef MEMOREL_COLLECTION_H_
#define MEMOREL_COLLECTION_H_

#include <vector>

#include "hugopeixoto/enumerable.h"

template <typename T>
class Collection : public std::vector<T>, public Enumerable<T, Collection> {
public:
  virtual void each(std::function<void(const T &)> pred) const override {
    for (const T &element : *this) {
      pred(element);
    }
  }

  Collection<T> operator+(const Collection<T> &o) const {
    Collection<T> sum;

    for (const T &element : *this) {
      sum.push_back(element);
    }

    for (const T &element : o) {
      sum.push_back(element);
    }

    return sum;
  }
};

#endif
