(* Copyright 2016 Chetan Murthy *)

open Thrift
open Plugin_types
open SerDes

module GI : (SERDES_SIG with type t = generatorInput) = T(struct
  type t = generatorInput
  let writer wr proto = wr#write proto
  let reader = read_generatorInput
end)

let main () =
  Nicejson.prepend_typelib_directory "../../test/cpp/gen-typelib" ;
  let xprt0 = new TChannelTransport.t (stdin, stdout) in
  let xprt = new TFramedTransport.t xprt0 in
  let proto = new TBinaryProtocol.t xprt in
  let gi = read_generatorInput proto in
  Printf.fprintf stderr "read generator input\n"; flush stderr ;
  let ser = GI.ser gi in
  let js = Nicejson.json_from_binary "apache.thrift.plugin.plugin" "GeneratorInput" ser in
  Yojson.Safe.pretty_to_channel stdout js

let _ = main () ;;
