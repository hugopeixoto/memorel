#ifndef MEMOREL_HAS_MANY_H_
#define MEMOREL_HAS_MANY_H_

#include "memorel/table.h"
#include "memorel/id.h"
#include "memorel/index.h"
#include "memorel/partially_ordered_tuple.h"

#include "hugopeixoto/enumerable.h"

#include <vector>
#include <algorithm>

namespace memorel {
template <typename Source, typename PrimaryKey, typename Target,
          typename ForeignKey>
class HasMany {
public:
  typedef typename ForeignKey::KeyType KeyType;

  HasMany() {}
  HasMany(const Table<Target> &targets) { Load(targets); }

  void Load(const Table<Target> &targets) { index.Load(targets); }

  memorel::Range<Target> operator()(const Source &source) const {
    return index(KeyType(PrimaryKey::key(source)));
  }

  template <template <typename> class Container>
  Collection<const Target *>
  fetch_all(const Enumerable<const Source *, Container> &sources) const {
    std::vector<const Target *> all_targets;

    sources.each([this, &all_targets](const auto &source) {
      index(PrimaryKey::key(*source)).each([&all_targets](const auto &target) {
        all_targets.push_back(target);
      });
    });

    std::vector<partially_ordered_tuple<
        1, typename Table<Target>::PrimaryKeyType::KeyType, const Target *>>
        mapped_values(all_targets.size());

    uint64_t i = 0;
    for (const auto &target : all_targets) {
      std::get<0>(mapped_values[i]) =
          Table<Target>::PrimaryKeyType::key(*target);
      std::get<1>(mapped_values[i]) = target;

      ++i;
    }

    std::sort(mapped_values.begin(), mapped_values.end());
    mapped_values.erase(std::unique(mapped_values.begin(), mapped_values.end()),
                        mapped_values.end());

    Collection<const Target *> targets;

    targets.resize(mapped_values.size());
    for (size_t i = 0; i < mapped_values.size(); ++i) {
      targets[i] = std::get<1>(mapped_values[i]);
    }

    return targets;
  }

  auto fetch(const Source &source) const { return this->operator()(source); }

protected:
  Index<Target, ForeignKey> index;
};
}

#endif
