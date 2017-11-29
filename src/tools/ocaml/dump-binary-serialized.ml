
open Cmdliner
open Misc

let dump_f permissive typelib type_ inputfile =
  let serdata = file_contents inputfile in
  let js = Nicejson.json_from_binary typelib type_ serdata in
  Yojson.Safe.pretty_to_channel stdout js

let version = "0.001"
let opts_sect = "OPTIONS"

let dump_cmd =
  let doc = "Ocaml implmenetation of a Thrift binary format deserializer (to JSON)" in
  let man = [] in
  let permissive =
    let doc = "permissive demarshalling" in
    Arg.(value & flag & info ["permissive"] ~doc) in

  let filename =
    let doc = "file containing the binary Thrift object" in
    Arg.(value & pos 0 file "" & info [] ~docv:"BINARY-FILE" ~doc) in

  let type_ =
    let doc = "Thrift type of serialized file" in
    Arg.(value & opt string "" & info ["type"] ~docv:"TYPE" ~doc) in

  let typelib =
    let doc = "Thrift typelib in which to find type" in
    Arg.(value & opt string "" & info ["typelib"] ~docv:"TYPELIB" ~doc) in

  Term.(const dump_f $ permissive $ typelib $ type_ $ filename),
  Term.info "dump-binary-serialized" ~version ~docs:opts_sect ~doc ~man
    

let main () =
  match Term.eval ~catch:true dump_cmd with
  | `Error _ -> exit 1 | _ -> exit 0
;;

if invoked_with "dump-binary-serialized" then
  main()
;;
