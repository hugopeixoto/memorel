#ifndef MEMOREL_RELATION_H_
#define MEMOREL_RELATION_H_

#include "memorel/table.h"
#include "memorel/id.h"
#include "memorel/index.h"

namespace memorel {
template <typename Model, typename KeyType = uint64_t>
class Relation : public Table<Model> {
public:
  void load(const Table<Model> &t) { Table<Model>::operator=(t); }

  template <typename Target, KeyType Target::*member>
  using HasMany = ::memorel::HasMany<Model, KeyAssociation<Model, KeyType, &Model::id>,
                            Target, KeyAssociation<Target, KeyType, member>>;

  template <typename Target, Optional<KeyType> Target::*member>
  using OptionalHasMany =
      ::memorel::HasMany<Model, KeyAssociation<Model, KeyType, &Model::id>, Target,
                OptionalKeyAssociation<Target, KeyType, member>>;

  template <typename Target, KeyType Model::*member>
  using BelongsTo =
      ::memorel::BelongsTo<Model, KeyAssociation<Model, KeyType, member>, Target,
                  KeyAssociation<Target, KeyType, &Target::id>>;

  template <typename Target, Optional<KeyType> Model::*member>
  using OptionalBelongsTo =
      ::memorel::BelongsTo<Model, OptionalKeyAssociation<Model, KeyType, member>, Target,
                  KeyAssociation<Target, KeyType, &Target::id>>;

  template <typename T, T Model::*member>
  using Index = ::memorel::Index<Model, KeyAssociation<Model, T, member>>;
};
}

#endif
