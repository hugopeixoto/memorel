#include "memorel/has_many.h"
#include "memorel/belongs_to.h"
#include "memorel/relation.h"
#include "memorel/postgresql.h"

#include "hugopeixoto/enumerable_ostream.h"

#include <iostream>

struct User {
  uint64_t id;
  std::string name;
  Optional<uint64_t> company_id;
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

  auto name = [](auto m) { return m.name; };

  std::cout << "users: " << users.map(name) << std::endl;
  std::cout << "companies: " << companies.map(name) << std::endl;

  for (auto u : users) {
    std::cout << u.name << " works for "
              << users.company(u).map(name).orDefault("no one")
              << std::endl;
  }

  for (auto c : companies) {
    std::cout << c.name << " employs "
              << companies.users.get(c).map(name)
              << std::endl;
  }

  return 0;
}
