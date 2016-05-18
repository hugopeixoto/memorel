#ifndef MEMOREL_INDEX_H_
#define MEMOREL_INDEX_H_

#include <memory>
#include <map>
#include <vector>
#include <algorithm>
#include <type_traits>
#include "memorel/table.h"
#include "memorel/range.h"

namespace memorel {
template <typename Model, typename Key> class Index {
public:
  typedef typename Key::KeyType KeyType;

  void Load(const Table<Model> &records) {
    std::vector<Pair> mapped_values(records.size());

    uint64_t i = 0;
    for (const auto &record : records) {
      auto key = Key::key(record);

      if (!is_none(key)) {
        std::get<0>(mapped_values[i]) = key;
        std::get<1>(mapped_values[i]) = &record;

        ++i;
      }
    }

    mapped_values.resize(i);

    std::sort(mapped_values.begin(), mapped_values.end());

    records_.resize(mapped_values.size());
    for (uint64_t i = 0; i < mapped_values.size(); ++i) {
      records_[i] = std::get<1>(mapped_values[i]);
      auto &index = index_[std::get<0>(mapped_values[i])];
      index.begin++;
      index.end = i;
    }

    for (auto &kv : index_) {
      kv.second.begin = kv.second.end + 1 - kv.second.begin;
      kv.second.end++;
    }
  }

  memorel::Range<Model> operator() (const Optional<KeyType>& key) const {
    return memorel::Range<Model>(
        key.map([&](auto v) { return index_for(v); }).orDefault(Bounds()),
        records_);
  }

  memorel::Range<Model> operator()(const KeyType &key) const {
    return memorel::Range<Model>(index_for(key), records_);
  }

  memorel::Bounds index_for(const KeyType &key) const {
    auto it = index_.find(key);

    if (it != index_.end()) {
      return it->second;
    } else {
      return memorel::Bounds();
    }
  }

protected:
  typedef std::tuple<KeyType, const Model *> Pair;
  std::vector<const Model *> records_;
  std::map<KeyType, memorel::Bounds> index_;
};
}

#endif
