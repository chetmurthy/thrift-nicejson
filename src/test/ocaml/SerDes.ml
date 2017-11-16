open Thrift
open TBytesTransport

module type SERDES_ARG_SIG =
sig
  type t
  val writer : t -> Protocol.t -> unit
  val reader : Protocol.t -> t
end
  
module type SERDES_SIG =
sig
  type t
    val ser : t -> string
    val des : string -> t
end

module T(T : SERDES_ARG_SIG) : (SERDES_SIG with type t = T.t) = struct
  type t = T.t
  let ser o =
    let buffer = Buffer.create 23 in
    let tx = new write_t buffer in
    let proto = new TBinaryProtocol.t tx in
    T.writer o proto ;
    Buffer.contents buffer

  let des b =
    let tx = new read_t b in
    let proto = new TBinaryProtocol.t tx in
    T.reader proto
end
