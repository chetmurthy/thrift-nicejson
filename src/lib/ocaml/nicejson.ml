
let x = 1

let prepend_typelib_directory = Ocaml_nicejson.prepend_typelib_directory

let install_typelib a b =
  let rv = Ocaml_nicejson.install_typelib a b in
  if rv <> "" then failwith rv
  else ()

let require_typelib key =
  let rv = Ocaml_nicejson.require_typelib key in
  if rv <> "" then failwith rv
  else ()

let json_from_binary key typ serialized =
  let (rv, jss) = Ocaml_nicejson.json_from_binary key typ serialized in
  if rv <> "" then failwith rv ;
  Yojson.Safe.from_string jss

let binary_from_json key ty js =
  let jss = Yojson.Safe.to_string js in
  let rv, ser = Ocaml_nicejson.binary_from_json key ty jss in
  if rv <> "" then failwith rv ;
  ser

let service_struct_name_args k s o = Ocaml_nicejson.service_struct_name_args k s o
let service_struct_name_result k s o = Ocaml_nicejson.service_struct_name_result k s o

  (*

module TNiceJSONProtocol = struct
  open Thrift
  module P = Protocol

  type mode =
      MODE_NONE
    | WRITING_MESSAGE of string * message_type * int
    | READING_MESSAGE
    | BROKEN

  class t tran s=
  object (self)
    inherit P.t trans
    val mutable mode_ = MODE_NONE
    method writeMessageBegin (name, ty, seqid) =
      
      
    method virtual writeMessageEnd : unit
    method virtual writeStructBegin : string -> unit
    method virtual writeStructEnd : unit
    method virtual writeFieldBegin : string * t_type * int -> unit
    method virtual writeFieldEnd : unit
    method virtual writeFieldStop : unit
    method virtual writeMapBegin : t_type * t_type * int -> unit
    method virtual writeMapEnd : unit
    method virtual writeListBegin : t_type * int -> unit
    method virtual writeListEnd : unit
    method virtual writeSetBegin : t_type * int -> unit
    method virtual writeSetEnd : unit
    method virtual writeBool : bool -> unit
    method virtual writeByte : int -> unit
    method virtual writeI16 : int -> unit
    method virtual writeI32 : Int32.t -> unit
    method virtual writeI64 : Int64.t -> unit
    method virtual writeDouble : float -> unit
    method virtual writeString : string -> unit
    method virtual writeBinary : string -> unit
      (* reading methods *)
    method virtual readMessageBegin : string * message_type * int
    method virtual readMessageEnd : unit
    method virtual readStructBegin : string
    method virtual readStructEnd : unit
    method virtual readFieldBegin : string * t_type * int
    method virtual readFieldEnd : unit
    method virtual readMapBegin : t_type * t_type * int
    method virtual readMapEnd : unit
    method virtual readListBegin : t_type * int
    method virtual readListEnd : unit
    method virtual readSetBegin : t_type * int
    method virtual readSetEnd : unit
    method virtual readBool : bool
    method virtual readByte : int
    method virtual readI16 : int
    method virtual readI32: Int32.t
    method virtual readI64 : Int64.t
    method virtual readDouble : float
    method virtual readString : string
    method virtual readBinary : string
        (* skippage *)
    method skip typ =

  end
    
end
  *)
