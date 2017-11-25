(* Copyright 2016 Chetan Murthy *)

open OUnit2

let basic_tests = "basic_tests" >:::
  [

    "nicejson_foo" >::
      (fun ctxt ->
	assert_equal "45" (Ocaml_nicejson.nicejson_foo 45)) ;

    "nicejson_bar" >::
      (fun ctxt ->
	assert_equal ("foo", 42) (Ocaml_nicejson.nicejson_bar "foo" 42)) ;

    "prepend" >::
      (fun ctxt ->
	Ocaml_nicejson.prepend_typelib_directory "../cpp/gen-typelib"
      ) ;

    "bad_install" >::
      (fun ctxt ->
	Nicejson.prepend_typelib_directory "../cpp/gen-typelib" ;
	assert_raises (Failure "error: no typelib foo found on path")
	  (fun () -> Nicejson.require_typelib "foo") ;
      ) ;

    "bad_require" >::
      (fun ctxt ->
	Nicejson.prepend_typelib_directory "../cpp/gen-typelib" ;
	assert_raises (Failure "Expected '{'; got 'a'.")
	  (fun () -> Nicejson.install_typelib "foo" "argle") ;
      ) ;

    "ok_require" >::
      (fun ctxt ->
	Nicejson.prepend_typelib_directory "../cpp/gen-typelib" ;
        Nicejson.require_typelib "apache.thrift.plugin.plugin" ;
        Nicejson.require_typelib "apache.thrift.plugin.plugin" ;
      ) ;
    
    "struct_names" >::
      (fun ctxt ->
	Nicejson.prepend_typelib_directory "../cpp/gen-typelib" ;
	assert_equal "S2_foo_args" (Nicejson.service_struct_name_args "thrift_test.test" "S2" "foo") ;
	assert_equal "Calculator_add_args" (Nicejson.service_struct_name_args "tutorial.tutorial" "Calculator" "add") ;
      )
  ]

open Thrift
open Test_types
open TBytesTransport
open SerDes

module Bar : (SERDES_SIG with type t = bar) = T(struct
  type t = bar
  let writer wr proto = wr#write proto
  let reader = read_bar
end)

let roundtrip typelib ty ser expected =
  Nicejson.require_typelib typelib ;
  let js = Nicejson.json_from_binary typelib ty ser in
  assert_equal js expected ;
  let ser2 = Nicejson.binary_from_json typelib ty js in
  assert_equal ser ser2

let ser_tests = "ser_tests" >:::
  [
    "ser_Bar" >::
      (fun ctxt ->
	Nicejson.prepend_typelib_directory "../cpp/gen-typelib" ;
	let b = new bar in
	b#set_a 1l ;
	b#set_b "ugh" ;
	let ser = Bar.ser b in
	roundtrip "thrift_test.test" "Bar" ser (Yojson.Safe.from_string {|{"a":1,"b":"ugh"}|})
      )
  ]

module Refiller = struct
type t =
  {
    buf : string ;
    mutable ofs : int ;
  }
let mk s = { buf = s ; ofs = 0 }
let refill r =
  fun dst max ->
  assert (Bytes.length dst >= 1) ;
  if r.ofs >= String.length r.buf then 0 else begin
    Bytes.set dst 0 (String.get r.buf r.ofs) ;
    r.ofs <- 1 + r.ofs ;
    1
  end
let rest r =
  let buflen = String.length r.buf in
  String.sub r.buf r.ofs (buflen - r.ofs)
end

let yojson_tests = "yojson_tests" >:::
  [
    "simple" >::
      (fun ctxt ->
	let lb = Lexing.from_string "{}argle" in
	let lst = Yojson.init_lexer () in
	let j = Yojson.Safe.from_lexbuf lst ~stream:true lb in
	assert_equal (`Assoc[]) j
      ) ;
    "refiller" >::
      (fun ctxt ->
	let r = Refiller.mk "{}argle" in
	let lb= Lexing.from_function (Refiller.refill r) in
	let lst = Yojson.init_lexer () in
	let j = Yojson.Safe.from_lexbuf lst ~stream:true lb in
	assert_equal (`Assoc[]) j ;
	assert_equal "argle" (Refiller.rest r) ;
      ) ;
  ]

let proto_tests = "proto_tests" >:::
  [
  ]

(* Run the tests in test suite *)
let _ = 
  run_test_tt_main ("all_tests" >::: [ basic_tests ; ser_tests ; yojson_tests ; proto_tests ])
;;
