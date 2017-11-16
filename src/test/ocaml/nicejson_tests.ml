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

(* Run the tests in test suite *)
let _ = 
  run_test_tt_main ("all_tests" >::: [ basic_tests ; ser_tests ])
;;
