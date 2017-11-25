namespace cpp thrift_test
namespace typelib thrift_test

enum E {
  A
  B
  C
}

struct Bar {
  4: required i32 a ,
  5: required string b,
  6: optional i8 f6,
  7: optional i16 f7,
  8: optional i64 f8,
  9: optional double f9,
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

struct Ha {
  1: E e
}

exception InvalidOperation {
  1: i32 whatOp,
  2: string why
}

exception InvalidOperation2 {
  1: i32 whatOp,
  2: string why
}

service S1 {
  void ping(),
  i32 add(1:i32 num1, 2:i32 num2),
  i32 foo(1:i32 logid, 2:Bar w) throws (1:InvalidOperation ouch)
}

service S2 {
  void ping(),
  i32 foo(1:i32 n, 2:Bar w) throws (1:InvalidOperation ouch, 2:InvalidOperation2 ouch2),
  Bar goo(),
  oneway void hoo()
}
 
// test out TApplicationException marshalling
enum TApplicationExceptionType {
  UNKNOWN = 0,
  UNKNOWN_METHOD = 1,
  INVALID_MESSAGE_TYPE = 2,
  WRONG_METHOD_NAME = 3,
  BAD_SEQUENCE_ID = 4,
  MISSING_RESULT = 5,
  INTERNAL_ERROR = 6,
  PROTOCOL_ERROR = 7,
  INVALID_TRANSFORM = 8,
  INVALID_PROTOCOL = 9,
  UNSUPPORTED_CLIENT_TYPE = 10
}
exception TApplicationException {
  1: string message,
  2: TApplicationExceptionType type
}
