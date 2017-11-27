(* Copyright 2016 Chetan Murthy *)

open Thrift
open Plugin_types
open SerDes

module GI : (SERDES_SIG with type t = generatorInput) = T(struct
  type t = generatorInput
  let writer wr proto = wr#write proto
  let reader = read_generatorInput
end)

let output_json gi =
  let prog = gi#grab_program in
  let namespaces = prog#grab_namespaces in
  let cpp_ns =
    if not(Hashtbl.mem namespaces "cpp") then
      failwith "cannot find namespace cpp" ;
    Hashtbl.find namespaces "cpp" in
  let typelib_ns =
    if not(Hashtbl.mem namespaces "typelib") then
      cpp_ns
    else Hashtbl.find namespaces "typelib" in

  let out_path = gi#grab_program#grab_out_path in
  let name = gi#grab_program#grab_name in
  let destdir = Printf.sprintf "%s/gen-json-typelib" out_path in
  let dest = Printf.sprintf "%s/%s.%s.json" destdir typelib_ns name in
  Unix.system (Printf.sprintf "mkdir -p \"%s\"" destdir) ;
  let ser = GI.ser gi in
  let js = Nicejson.json_from_binary "apache.thrift.plugin.plugin" "GeneratorInput" ser in
  let oc = open_out dest in
  Yojson.Safe.pretty_to_channel oc js

let main () =
  Nicejson.prepend_typelib_directory "../../test/cpp/gen-typelib" ;
  let xprt0 = new TChannelTransport.t (stdin, stdout) in
  let xprt = new TFramedTransport.t xprt0 in
  let proto = new TBinaryProtocol.t xprt in
  let gi = read_generatorInput proto in
  Printf.fprintf stderr "read generator input for %s\n" gi#grab_program#grab_name; flush stderr ;
  output_json gi

let _ = main () ;;
