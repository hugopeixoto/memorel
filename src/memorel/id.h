#ifndef MEMOREL_ID_H_
#define MEMOREL_ID_H_

#include "hugopeixoto/nullable.h"

namespace memorel {
template <typename Resource, typename T, T Resource::*internal_member>
class KeyAssociation {
public:
  typedef T KeyType;

  static const KeyType key(const Resource &r) { return r.*internal_member; }
};

template <typename Resource, typename T, Nullable<T> Resource::*internal_member>
class NullableKeyAssociation {
public:
  typedef Nullable<T> KeyType;

  static const KeyType key(const Resource &r) { return (r.*internal_member); }
};
}

#endif
