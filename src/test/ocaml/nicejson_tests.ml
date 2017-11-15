(* Copyright 2016 Chetan Murthy *)

open OUnit2

let all = "all_tests" >:::
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
  ]
  
(* Run the tests in test suite *)
let _ = 
  run_test_tt_main all
;;
