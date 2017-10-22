namespace cpp thrift_test

struct Bar {
  4: required i32 a ,
  5: required string b,
}

struct Foo {
  1: required i32 a ,
  2: required string b,
  3: optional Bar c,
  4: optional list<Bar> d,
  5: optional map<string, Bar> e,
  6: optional list<i32> f,
  7: optional set<i32> g,
  8: optional list<list<string>> h,
  9: optional list<set<i32>> i,
  10: optional map<string, set<i32>> j,
  11: optional map<string, i32> k,
}

struct Boo {
  1: required list<list<list<i32>>> l ;
  2: optional map<Bar, Bar> m ,
  3: optional map<i32, string> n,
}

struct Goo {
  1: required set<set<set<i32>>> l ;
}
