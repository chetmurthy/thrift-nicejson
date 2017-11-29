
open Cmdliner
open Misc

open Thrift
open Plugin_types

module TFileTransport = struct
module T = Transport
class read_t i =
object (self)
  val mutable opened = true
  inherit Transport.t
  method isOpen = opened
  method opn = ()
  method close = close_in i; opened <- false
  method read buf off len =
    if opened then
      try
        really_input i buf off len; len
      with _ -> raise (T.E (T.UNKNOWN, ("TChannelTransport: Could not read "^(string_of_int len))))
    else
      raise (T.E (T.NOT_OPEN, "TChannelTransport: Channel was closed"))
  method write buf off len = assert false
  method flush = assert false
end
end

let read_typelib ~framed fname =
  apply_to_in_channel (fun ic ->
    let xprt0 = new TFileTransport.read_t ic in
    let xprt = if framed then new TFramedTransport.t xprt0 else xprt0 in
    let proto = new TBinaryProtocol.t xprt in
    let gi = read_generatorInput proto in
    gi) fname

let gen_f framed fname =
  let gi = read_typelib ~framed:framed fname in
  ()

let version = "0.001"
let opts_sect = "OPTIONS"

let gen_cmd =
  let doc = "Generate from binary-serialized plugin data" in
  let man = [] in
  let framed =
    let doc = "data is framed" in
    Arg.(value & opt bool true & info ["framed"] ~doc) in

  let filename =
    let doc = "file containing the binary Thrift object" in
    Arg.(value & pos 0 file "" & info [] ~docv:"BINARY-FILE" ~doc) in

  Term.(const gen_f $ framed $ filename),
  Term.info "thrift-gen-newtypelib" ~version ~docs:opts_sect ~doc ~man
    

let main () =
  match Term.eval ~catch:true gen_cmd with
  | `Error _ -> exit 1 | _ -> exit 0
;;

if invoked_with "thrift-gen-newtypelib" then
  main()
;;
