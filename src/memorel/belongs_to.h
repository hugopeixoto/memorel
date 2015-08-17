#ifndef MEMOREL_BELONGS_TO_H_
#define MEMOREL_BELONGS_TO_H_

#include "memorel/table.h"
#include "memorel/index.h"

namespace memorel {
template <typename Source, typename ForeignKey, typename Target,
          typename PrimaryKey>
class BelongsTo {
public:
  typedef ForeignKey RelationType;

  BelongsTo() {}

  BelongsTo(const Table<Target> &targets) { Load(targets); }

  void Load(const Table<Target> &targets) { index.Load(targets); }

  const Target *get(const Source &source) const {
    const Target *target = nullptr;

    index(ForeignKey::key(source)).each([&target](auto t) { target = t; });

    return target;
  }

protected:
  Index<Target, PrimaryKey> index;
};
}

#endif
