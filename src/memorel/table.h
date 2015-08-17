#ifndef MEMOREL_TABLE_H_
#define MEMOREL_TABLE_H_

#include <vector>
#include <limits>
#include <algorithm>

#include "memorel/id.h"
#include "memorel/collection.h"

namespace memorel {
template <typename Resource, typename PrimaryKey = KeyAssociation<
                                 Resource, uint64_t, &Resource::id>>
class Table : public Enumerable<const Resource *, Collection> {
public:
  template <typename A, typename R> struct It {
    It(A it) : it_(it) {}
    It operator++() { return It(++it_); }
    R operator*() { return *it_; }
    bool operator==(const It &o) { return it_ == o.it_; }
    bool operator!=(const It &o) { return it_ != o.it_; }

    A it_;
  };

  typedef It<typename std::vector<Resource>::iterator, Resource &> Iterator;
  typedef It<typename std::vector<Resource>::const_iterator, const Resource &>
      ConstIterator;

  typedef PrimaryKey PrimaryKeyType;

  Table(uint64_t size = 0) : elements(size) {}

  uint64_t size() const { return elements.size(); }

  Iterator begin() { return Iterator(elements.begin()); }
  Iterator end() { return Iterator(elements.end()); }

  ConstIterator begin() const { return ConstIterator(elements.begin()); }
  ConstIterator end() const { return ConstIterator(elements.end()); }

  const Resource *find(const typename PrimaryKey::KeyType &id) const {
    auto it =
        std::lower_bound(elements.begin(), elements.end(), id, IdFinder());

    if (it == elements.end() || id != PrimaryKey::key(*it)) {
      return nullptr;
    }

    return &*it;
  }

  virtual void
  each(std::function<void(const Resource *const &)> pred) const override {
    for (auto e : elements) {
      pred(&e);
    }
  }

protected:
  std::vector<Resource> elements;

  struct IdFinder {
    bool operator()(const typename PrimaryKey::KeyType &id, const Resource &r) {
      return id < PrimaryKey::key(r);
    }

    bool operator()(const Resource &r, const typename PrimaryKey::KeyType &id) {
      return PrimaryKey::key(r) < id;
    }
  };
};
}

#endif
