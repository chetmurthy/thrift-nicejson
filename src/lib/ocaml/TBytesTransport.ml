(* Copyright 2016 Chetan Murthy *)

open Thrift
module T = Transport

class write_t b =
object (self)
  val buffer = b
  inherit T.t
  method isOpen = true
    method opn = ()
    method close = assert false
    method read dst ofs len = assert false
    method write src ofs len =
      Buffer.add_substring buffer src ofs len
    method flush = ()
  end

class read_t b =
object (self)
  val buffer = b
  val mutable cursor = 0
  inherit T.t
  method isOpen = true
    method opn = ()
    method close = assert false
    method read dst ofs len =
      let len = min len (String.length b - cursor) in
      String.blit buffer cursor dst ofs len ;
      cursor <- cursor + len ;
      len
    method write _ _ _ = assert false
    method flush = assert false
  end
