#include "catch2/catch.hpp"
#include "Messenger.h"

#include <iostream>

namespace {
struct Workers {
  enum { Foo, Bar, Count };
};

struct Ping {
  int count;
};

struct Pong {
  int count;
};

void foo(const Ping& p) {
  std::cout << "ping " << p.count << std::endl;
}

void bar(const Pong& p) {
  std::cout << "pong " << p.count << std::endl;
}

struct Foo {
  void foo(const Ping &p) {
    std::cout << "ping " << p.count << std::endl;
  }

  void bar(const Pong &p) {
    std::cout << "pong " << p.count << std::endl;
  }
};

template<class T>
void testFunc(std::function<void(const T&)> f, T v) {
  f(v);
}

} // namespace

TEST_CASE("messenger", "[std]" ) {
  Foo foo;
  testFunc<Ping>([&foo](const Ping& p) -> void { foo.foo(p); }, Ping{});
}


// TEST_CASE("messenger", "[messenger]" ) {
//   Messenger messenger(Workers::Count, false);

//   messenger.registerMessageType<Ping>(Workers::Foo);
//   messenger.registerMessageType<Pong>(Workers::Bar);
//   Foo f;
//   messenger.registerListener(foo);
//   messenger.registerListener(bar);

//   messenger.sendMessage(Ping{1});
//   messenger.sendMessage(Ping{2});
//   messenger.sendMessage(Ping{3});
//   messenger.sendMessage(Pong{4});
// }
