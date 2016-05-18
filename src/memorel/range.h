#ifndef MEMOREL_RANGE_H_
#define MEMOREL_RANGE_H_

#include "memorel/collection.h"

namespace memorel {
struct Bounds {
  Bounds(uint64_t begin = 0, uint64_t end = 0) : begin(begin), end(end) {}

  uint64_t begin;
  uint64_t end;
};

template<typename Model>
class Range : public Enumerable<Model, Collection> {
  public:
    typedef std::vector<const Model*> Records;

    Range(Bounds b, const Records& r) : bounds(b), records(r) { }

    virtual void each (std::function<void(const Model&)> callback) const override {
      for (auto i = bounds.begin; i < bounds.end; i++) {
        callback(*records[i]);
      }
    }

  protected:
    Bounds bounds;
    const Records& records;
};
}

#endif
