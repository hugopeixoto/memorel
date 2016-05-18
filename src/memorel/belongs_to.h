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

  void Load(const Table<Target> &targets) {
    index.Load(targets);
  }

  auto get(const Source &source) const {
    return index(ForeignKey::key(source)).foldl(
        Optional<Target>(),
        [](auto acc, auto e) {
          return acc.orElse(Optional<Target>(e));
        });
  }

  auto operator()(const Source& source) const {
    return get(source);
  }

protected:
  Index<Target, PrimaryKey> index;
};
}

#endif
