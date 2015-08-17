memorel
=======

Load relational data into memory.


Example usage:

```sql
create table companies(id serial primary key, name varchar);
create table users(id serial primary key, name varchar, company_id integer);
insert into companies (name) VALUES ('ndrive'), ('github'), ('google');
insert into users (name, company_id) VALUES ('hugo', 1), ('john', 2), ('jane', 3), ('kate', 2);
```

```c++
#include "memorel/has_many.h"
#include "memorel/belongs_to.h"
#include "memorel/relation.h"
#include "memorel/postgresql.h"

#include "hugopeixoto/enumerable_ostream.h"

#include <iostream>

struct User {
  uint64_t id;
  std::string name;
  Nullable<uint64_t> company_id;
};

struct Company {
  uint64_t id;
  std::string name;
};

struct Users : public memorel::Relation<User> {
  OptionalBelongsTo<Company, &User::company_id> company;
};

struct Companies : public memorel::Relation<Company> {
  OptionalHasMany<User, &User::company_id> users;
};

template <> memorel::LoadInformation<User> memorel::Info() {
  return {"select id, name, company_id from users order by id;",
          [](auto result, auto &resource) {
    return Fetch(result, resource.id, resource.name, resource.company_id);
  }};
}

template <> memorel::LoadInformation<Company> memorel::Info() {
  return {"select id, name from companies order by id;",
          [](auto result, auto &resource) {
    return Fetch(result, resource.id, resource.name);
  }};
}

int main() {
  Users users;
  Companies companies;

  PGconn *connection = PQconnectdb("dbname=memorel host=localhost");

  Load(connection, users);
  Load(connection, companies);

  users.company.Load(companies);
  companies.users.Load(users);

  std::cout << "users: " << users.map([](auto u) { return u->name; })
            << std::endl;

  std::cout << "companies: " << companies.map([](auto c) { return c->name; })
            << std::endl;

  std::cout << "users' company: "
            << users.map([&](auto u) {
                 return std::make_pair(u->name, users.company.get(*u)->name);
               }) << std::endl;

  std::cout << "companies' users: "
            << companies.map([&](auto c) {
                 return std::make_pair(
                     c->name, companies.users.fetch(*c)
                                  .map([](auto u) { return u->name; }));
               }) << std::endl;

  return 0;
}
```

Output:

```
users: (hugo, john, jane, kate)
companies: (ndrive, github, google)
users' company: ((hugo, ndrive), (john, github), (jane, google), (kate, github))
companies' users: ((ndrive, (hugo)), (github, (john, kate)), (google, (jane)))
```
