namespace cpp thrift_thrift
namespace d thrift_thrift
namespace dart thrift_thrift
namespace java thrift_thrift
namespace php thrift_thrift
namespace perl thrift_thrift
namespace haxe thrift_thrift

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

enum TType {
  T_STOP       = 0,
  T_VOID       = 1,
  T_BOOL       = 2,
  T_BYTE       = 3,
  T_I08        = 3,
  T_I16        = 6,
  T_I32        = 8,
  T_U64        = 9,
  T_I64        = 10,
  T_DOUBLE     = 4,
  T_STRING     = 11,
  T_UTF7       = 11,
  T_STRUCT     = 12,
  T_MAP        = 13,
  T_SET        = 14,
  T_LIST       = 15,
  T_UTF8       = 16,
  T_UTF16      = 17
}

enum BaseType {
  TYP_STOP       = 0,
  TYP_VOID       = 1,
  TYP_BOOL       = 2,
  TYP_BYTE       = 3,
  TYP_I08        = 3,
  TYP_I16        = 6,
  TYP_I32        = 8,
  TYP_U64        = 9,
  TYP_I64        = 10,
  TYP_DOUBLE     = 4,
  TYP_STRING     = 11,
  TYP_UTF7       = 11,
  TYP_UTF8       = 16,
  TYP_UTF16      = 17
}

struct ListType {
  1: required Type & elemType ;
}

struct SetType {
  1: required Type & elemType ;
}

struct MapType {
  1: required Type & domType ;
  2: required Type & rngType ;
}

struct MessageRef {
  1: required string name ;
}

union TypeExp {
  1: BaseType t_base ;
  2: ListType & t_list ;
  3: SetType & t_set ;
  4: MapType & t_map ;
  5: MessageRef & t_struct ;
}

struct Type {
  1: required TType ttype ;
  2: required TypeExp texp ;
}

enum DefaultRequired {
  NEITHER, DEFAULT, REQUIRED
}

struct Field {
  1: required Type ty ;
  2: required bool pointer ;
  3: required DefaultRequired defreq ;
  4: required string fldname ;
  5: required i32 fldid ;
}

struct MessageByName {
  1: required map<string, Field> fields_byname ;
}

struct MessageById {
  1: required map<i32, Field> fields_byid ;
}

struct MessageSet {
  1: required map<string, MessageByName> messages ;
}

struct FullMessageSet {
  1: required map<string, MessageByName> messages_byname ;
  2: required map<string, MessageById> messages_byid ;
}
