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
  
(* Run the tests in test suite *)
let _ = 
  run_test_tt_main all
;;
