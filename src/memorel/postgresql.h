#ifndef MEMREL_POSTGRESQL_H_
#define MEMREL_POSTGRESQL_H_

#include <string>
#include <cstring>
#include <cstdlib>
#include <postgresql/libpq-fe.h>

namespace memorel {
typedef std::function<const char *(uint32_t)> ColumnFetcher;

template <typename T, bool should_throw>
bool load_resources(Relation<T> &resources, PGconn *connection,
                    const std::string &query,
                    std::function<bool(ColumnFetcher, T &)> loader) {
  auto result = PQexec(connection, query.c_str());

  resources.load(Table<T>(PQntuples(result)));

  uint64_t idx = 0;
  for (auto &resource : resources) {
    bool ok =
        loader([result, idx](uint32_t i) {
                 if (PQgetisnull(result, idx, i)) {
                   return static_cast<const char *>(nullptr);
                 } else {
                   return static_cast<const char *>(PQgetvalue(result, idx, i));
                 }
               },
               resource);

    if (!ok) {
      printf("failed loading resource: %s\n", query.c_str());

      if (should_throw) {
        throw 1;
      }

      return false;
    }

    ++idx;
  }

  return true;
}

template <typename T>
using LoadInformation =
    std::tuple<std::string, std::function<bool(ColumnFetcher, T &)>>;

template <typename T> LoadInformation<T> Info();

template <typename T, bool should_throw = true>
bool Load(PGconn *connection, Relation<T> &resources) {
  auto load_info = Info<T>();

  return load_resources<T, should_throw>(
      resources, connection, std::get<0>(load_info), std::get<1>(load_info));
}

template <typename T> bool Fetch(const char *value, T &field) {
  if (value == nullptr) {
    return false;
  }

  field = value;
  return true;
}

template <> bool Fetch(const char *value, uint64_t &field) {
  if (value == nullptr) {
    field = std::numeric_limits<uint64_t>::max();
  } else {
    field = strtol(value, nullptr, 10);
  }
  return true;
}

template <> bool Fetch(const char *value, double &field) {
  if (value == nullptr) {
    return false;
  }

  field = strtod(value, nullptr);
  return true;
}

template <> bool Fetch(const char *value, bool &field) {
  if (value == nullptr) {
    return false;
  }

  field = !strcmp(value, "t");
  return true;
}

template <typename T> bool Fetch(const char *value, Optional<T> &field) {
  if (value == nullptr) {
    field = Optional<T>();
    return true;
  } else {
    T tmp;
    bool ok = Fetch(value, tmp);
    field = Optional<T>(tmp);

    return ok;
  }
}

template <uint32_t I = 0, typename T>
bool Fetch(ColumnFetcher fetcher, T &field) {
  return Fetch(fetcher(I), field);
}

template <uint32_t I = 0, typename T, typename... Args>
bool Fetch(ColumnFetcher fetcher, T &field, Args &... a) {
  return Fetch(fetcher(I), field) && Fetch<I + 1>(fetcher, a...);
}
}

#endif
