
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

module TNiceJSONProtocol = struct
  open Thrift
  module P = Protocol

  let struct_name typelib service name ty =
    if ty == P.CALL then service_struct_name_args typelib service name
    else if ty == P.REPLY then service_struct_name_result typelib service name
    else raise (P.E(P.UNKNOWN, "struct_name: unexpected messageType"))

  let messageType_to_string = function
    | P.CALL -> "call"
    | P.REPLY -> "reply"
    | P.EXCEPTION -> "exception"
    | _ -> assert false

  let string_to_messageType = function
    | "call" -> P.CALL
    | "reply" -> P.REPLY
    | "exception" -> P.EXCEPTION
    | _ -> assert false

module AE = Application_Exn
  let string_to_TApplicationExceptionType = function
    | "UNKNOWN" -> AE.UNKNOWN
    | "UNKNOWN_METHOD" -> AE.UNKNOWN_METHOD
    | "INVALID_MESSAGE_TYPE" -> AE.INVALID_MESSAGE_TYPE
    | "WRONG_METHOD_NAME" -> AE.WRONG_METHOD_NAME
    | "BAD_SEQUENCE_ID" -> AE.BAD_SEQUENCE_ID
    | "MISSING_RESULT" -> AE.MISSING_RESULT
    | "INTERNAL_ERROR" -> AE.INTERNAL_ERROR
    | "PROTOCOL_ERROR" -> AE.PROTOCOL_ERROR
    | "INVALID_TRANSFORM" -> AE.INVALID_TRANSFORM
    | "INVALID_PROTOCOL" -> AE.INVALID_PROTOCOL
    | "UNSUPPORTED_CLIENT_TYPE" -> AE.UNSUPPORTED_CLIENT_TYPE
    | _ -> raise (P.E(P.UNKNOWN, "string_to_TApplicationExceptionType: unexpected string"))

  let _TApplicationExceptionType_to_string = function
    | AE.UNKNOWN -> "UNKNOWN"
    | AE.UNKNOWN_METHOD -> "UNKNOWN_METHOD"
    | AE.INVALID_MESSAGE_TYPE -> "INVALID_MESSAGE_TYPE"
    | AE.WRONG_METHOD_NAME -> "WRONG_METHOD_NAME"
    | AE.BAD_SEQUENCE_ID -> "BAD_SEQUENCE_ID"
    | AE.MISSING_RESULT -> "MISSING_RESULT"
    | AE.INTERNAL_ERROR -> "INTERNAL_ERROR"
    | AE.PROTOCOL_ERROR -> "PROTOCOL_ERROR"
    | AE.INVALID_TRANSFORM -> "INVALID_TRANSFORM"
    | AE.INVALID_PROTOCOL -> "INVALID_PROTOCOL"
    | AE.UNSUPPORTED_CLIENT_TYPE -> "UNSUPPORTED_CLIENT_TYPE"

  type mode =
      MODE_NONE
    | WRITING_MESSAGE of string * P.message_type * int * Buffer.t * TBinaryProtocol.t
    | READING_MESSAGE of string * P.message_type * int * TBytesTransport.read_t * TBinaryProtocol.t
    | BROKEN

  let readjson trans =
    let refill b max =
      trans#read b 0 1 in
    let lb = Lexing.from_function refill in
    let lst = Yojson.init_lexer () in
    Yojson.Safe.from_lexbuf lst ~stream:true lb

module SerDes_AE = SerDes.T(struct
  type t = AE.t
  let writer wr proto = wr#write proto
  let reader = AE.read
end)

  class t trans typelib service =
  object (self)
    inherit P.t trans
    val mutable mode_ = MODE_NONE

    method private must_be_none =
      match mode_ with
	MODE_NONE -> ()
      | _ -> raise (P.E(P.UNKNOWN, "mode mismatch: must be NONE"))

    method private must_be_writing =
      match mode_ with
	WRITING_MESSAGE (a,b,c,d,e) -> (a,b,c,d,e)
      | _ -> raise (P.E(P.UNKNOWN, "mode mismatch: must be WRITING"))

    method private must_be_reading =
      match mode_ with
	READING_MESSAGE (a,b,c,d,e) -> (a,b,c,d,e)
      | _ -> raise (P.E(P.UNKNOWN, "mode mismatch: must be READING"))

    method writeMessageBegin (name, ty, seqid) =
      let buf = Buffer.create 23 in
      let transport = new TBytesTransport.write_t buf in
      let proto = new TBinaryProtocol.t transport in
      mode_ <- WRITING_MESSAGE(name, ty, seqid, buf, proto)

    method writeMessageEnd =
      let (name, ty, seqid, buf, proto) = self#must_be_writing in
      mode_ <- BROKEN ;
      let j =
	if ty = P.EXCEPTION then
	  let ae = SerDes_AE.des (Buffer.contents buf) in
	  `Assoc[
	    "message", `String ae#get_message;
	    "type", `String(_TApplicationExceptionType_to_string ae#get_type)
	  ]
	else
	  let structname = struct_name typelib service name ty in
	  json_from_binary typelib structname (Buffer.contents buf) in
      let msg = `Assoc [
	"body", j ;
	"name", `String name ;
	"type", `String(messageType_to_string ty) ;
	"seqid", `Int seqid ;
      ] in
      let msgs = Yojson.Safe.to_string msg in
      trans#write msgs 0 (String.length msgs) ;
      mode_ <- MODE_NONE

    method writeStructBegin x =
      let (_, _, _, _, proto) = self#must_be_writing in
      proto#writeStructBegin x

    method writeStructEnd =
      let (_, _, _, _, proto) = self#must_be_writing in
      proto#writeStructEnd

    method writeFieldBegin x =
      let (_, _, _, _, proto) = self#must_be_writing in
      proto#writeFieldBegin x

    method writeFieldEnd =
      let (_, _, _, _, proto) = self#must_be_writing in
      proto#writeFieldEnd

    method writeFieldStop =
      let (_, _, _, _, proto) = self#must_be_writing in
      proto#writeFieldStop

    method writeMapBegin x =
      let (_, _, _, _, proto) = self#must_be_writing in
      proto#writeMapBegin x

    method writeMapEnd =
      let (_, _, _, _, proto) = self#must_be_writing in
      proto#writeMapEnd

    method writeListBegin x =
      let (_, _, _, _, proto) = self#must_be_writing in
      proto#writeListBegin x

    method writeListEnd =
      let (_, _, _, _, proto) = self#must_be_writing in
      proto#writeListEnd

    method writeSetBegin x =
      let (_, _, _, _, proto) = self#must_be_writing in
      proto#writeSetBegin x

    method writeSetEnd =
      let (_, _, _, _, proto) = self#must_be_writing in
      proto#writeSetEnd

    method writeBool x =
      let (_, _, _, _, proto) = self#must_be_writing in
      proto#writeBool x

    method writeByte x =
      let (_, _, _, _, proto) = self#must_be_writing in
      proto#writeByte x

    method writeI16 x =
      let (_, _, _, _, proto) = self#must_be_writing in
      proto#writeI16 x

    method writeI32 x =
      let (_, _, _, _, proto) = self#must_be_writing in
      proto#writeI32 x

    method writeI64 x =
      let (_, _, _, _, proto) = self#must_be_writing in
      proto#writeI64 x

    method writeDouble x =
      let (_, _, _, _, proto) = self#must_be_writing in
      proto#writeDouble x

    method writeString x =
      let (_, _, _, _, proto) = self#must_be_writing in
      proto#writeString x

    method writeBinary x =
      let (_, _, _, _, proto) = self#must_be_writing in
      proto#writeBinary x

      (* reading methods *)
    method readMessageBegin =
      self#must_be_none ;
      let j = readjson trans in
      let msg_alist = match j with `Assoc l -> l
	| _ -> raise (P.E(P.UNKNOWN, "bad JSON received -- not an assoc")) in
      if not (List.mem_assoc "type" msg_alist) then
	raise (P.E(P.UNKNOWN, "TNiceJSONProtocol: bad JSON, missing type element")) ;
      let s_messageType = match List.assoc "type" msg_alist with `String s -> s
	| _ -> raise (P.E(P.UNKNOWN, "TNiceJSONProtocol: bad JSON, type element must be string")) in
      let messageType = string_to_messageType s_messageType in

      if not(List.mem_assoc "name" msg_alist) then
	raise (P.E(P.UNKNOWN, "TNiceJSONProtocol: bad JSON, missing name element")) ;
      let name = match List.assoc "name" msg_alist with `String s -> s
	| _ -> raise (P.E(P.UNKNOWN, "TNiceJSONProtocol: bad JSON, name element must be string")) in

      if not(List.mem_assoc "seqid" msg_alist) then
	raise (P.E(P.UNKNOWN, "TNiceJSONProtocol: bad JSON, missing seqid element")) ;
      let seqid = match List.assoc "seqid" msg_alist with `Int s -> s
	| _ -> raise (P.E(P.UNKNOWN, "TNiceJSONProtocol: bad JSON, seqid element must be integer")) in

      if not(List.mem_assoc "body" msg_alist) then
	raise (P.E(P.UNKNOWN, "TNiceJSONProtocol: bad JSON, missing body element")) ;
      
      let body = List.assoc "body" msg_alist in
      let body_alist = match body with `Assoc l -> l
	| _ -> raise (P.E(P.UNKNOWN,"TNiceJSONProtocol: bad JSON, body element is not Assoc")) in
      let ser =
	if messageType <> P.EXCEPTION then
	  let structname = struct_name typelib service name messageType in
	  binary_from_json typelib structname body
	else
	  let ae =
	    if not(List.mem_assoc "message" body_alist && List.mem_assoc "type" body_alist) then
	      AE.create AE.UNKNOWN "malformed exception"
	    else
	      match List.assoc "message" body_alist, List.assoc "type" body_alist with
	      | `String message, `String type_ ->
		 AE.create (string_to_TApplicationExceptionType type_) message
	      | _ -> AE.create AE.UNKNOWN "malformed exception(2)" in
	  SerDes_AE.ser ae in
      let transport = new TBytesTransport.read_t ser in
      let proto = new TBinaryProtocol.t transport in
      mode_ <- READING_MESSAGE(name, messageType, seqid, transport, proto) ;
      (name, messageType, seqid)

    method readMessageEnd =
      self#must_be_reading ;
      mode_ <- MODE_NONE

    method readStructBegin =
      let (_, _, _, _, proto) = self#must_be_reading in
      proto#readStructBegin


    method readStructEnd =
      let (_, _, _, _, proto) = self#must_be_reading in
      proto#readStructEnd

    method readFieldBegin =
      let (_, _, _, _, proto) = self#must_be_reading in
      proto#readFieldBegin

    method readFieldEnd =
      let (_, _, _, _, proto) = self#must_be_reading in
      proto#readFieldEnd

    method readMapBegin =
      let (_, _, _, _, proto) = self#must_be_reading in
      proto#readMapBegin

    method readMapEnd =
      let (_, _, _, _, proto) = self#must_be_reading in
      proto#readMapEnd

    method readListBegin =
      let (_, _, _, _, proto) = self#must_be_reading in
      proto#readListBegin

    method readListEnd =
      let (_, _, _, _, proto) = self#must_be_reading in
      proto#readListEnd

    method readSetBegin =
      let (_, _, _, _, proto) = self#must_be_reading in
      proto#readSetBegin

    method readSetEnd =
      let (_, _, _, _, proto) = self#must_be_reading in
      proto#readSetEnd

    method readBool =
      let (_, _, _, _, proto) = self#must_be_reading in
      proto#readBool
	
    method readByte =
      let (_, _, _, _, proto) = self#must_be_reading in
      proto#readByte

    method readI16 =
      let (_, _, _, _, proto) = self#must_be_reading in
      proto#readI16

    method readI32 =
      let (_, _, _, _, proto) = self#must_be_reading in
      proto#readI32

    method readI64 =
      let (_, _, _, _, proto) = self#must_be_reading in
      proto#readI64

    method readDouble =
      let (_, _, _, _, proto) = self#must_be_reading in
      proto#readDouble

    method readString =
      let (_, _, _, _, proto) = self#must_be_reading in
      proto#readString

    method readBinary =
      let (_, _, _, _, proto) = self#must_be_reading in
      proto#readBinary

        (* skippage *)
    method skip typ =
      let (_, _, _, _, proto) = self#must_be_reading in
      proto#skip typ
  end

end
