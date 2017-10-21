namespace cpp thrift_test

struct Bar {
  4: required i32 a ,
  5: required string b,
}

struct Foo {
  1: required i32 a ,
  2: required string b,
  3: required Bar c,
  4: required list<Bar> d,
  5: required map<string, Bar> e,
  6: required list<i32> f,
  7: required set<i32> g,
  8: required list<list<string>> h,
  9: required list<set<i32>> i,
  10: required map<string, set<i32>> j,
}

struct Boo {
  1: required list<list<list<i32>>> l ;
}
