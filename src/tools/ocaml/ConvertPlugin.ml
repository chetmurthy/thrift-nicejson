
open Sexplib
open Sexplib.Std
open Misc
open Thrift
module PT = Plugin_types
type t_program_id =  int64 [@@deriving sexp, yojson]
type t_type_id =  int64 [@@deriving sexp, yojson]
type t_service_id =  int64 [@@deriving sexp, yojson]
type t_const_id =  int64 [@@deriving sexp, yojson]

module T_base = struct
    type t = PT.T_base.t =
    | TYPE_VOID
    | TYPE_STRING
    | TYPE_BOOL
    | TYPE_I8
    | TYPE_I16
    | TYPE_I32
    | TYPE_I64
    | TYPE_DOUBLE
    | TYPE_BINARY [@@deriving sexp, yojson]

    let to_i = PT.T_base.to_i
    let of_i = PT.T_base.of_i
end

module Requiredness = struct
  type t = PT.Requiredness.t =
    | T_REQUIRED
    | T_OPTIONAL
    | T_OPT_IN_REQ_OUT [@@deriving sexp, yojson]
  let to_i = PT.Requiredness.to_i
  let of_i = PT.Requiredness.of_i
end

module LAssoc = struct
  type ('a, 'b) _t = ('a * 'b) list [@@deriving sexp, yojson]

    let t_of_sexp = _t_of_sexp
    let sexp_of_t = sexp_of__t

  let to_yojson f1 f2 m = _t_to_yojson f1 f2 m
  let of_yojson f1 f2 l =
    match _t_of_yojson f1 f2 l with
      Error s -> Error s
    | Ok l -> Ok l
end

type ('a, 'b) map_t = ('a, 'b) Hashtbl.t [@@deriving sexp]
let map_t_to_list m =
  let acc = ref [] in
  Hashtbl.iter (fun k v -> push acc (k,v)) m ;
  List.sort Pervasives.compare !acc
let map_t_of_list l =
  let h = Hashtbl.create 23 in
  List.iter (fun (k,v) -> Hashtbl.add h k v) l ;
  h

let map_t_to_yojson kf vf m = LAssoc.to_yojson kf vf (map_t_to_list m)
let map_t_of_yojson kf vf j =
  match LAssoc.of_yojson kf vf j with
    Error s -> Error s
  | Ok l -> Ok (map_t_of_list l)

let conv_map convk convv m =
  let h = Hashtbl.create 23 in
  Hashtbl.iter (fun k v ->
    Hashtbl.add h (convk k) (convv v))
    m ;
  h

module TypeMetadata = struct
type t = {
  name : string ;
  program_id : t_program_id ;
  annotations : (string, string) map_t option ;
  doc : string option ;
} [@@deriving sexp, yojson]
let conv (pt : PT.typeMetadata) = {
  name = pt#grab_name ;
  program_id = pt#grab_program_id ;
  annotations = pt#get_annotations ;
  doc = pt#get_doc
}
end
  
module TBaseType = struct
type t = {
  metadata : TypeMetadata.t ;
  value : T_base.t ;
} [@@deriving sexp, yojson]
let conv (pt : PT.t_base_type) = {
  metadata = TypeMetadata.conv pt#grab_metadata ;
  value = pt#grab_value ;
}
end

module TList = struct
type t = {
  metadata : TypeMetadata.t ;
  cpp_name : string option ;
  elem_type : t_type_id ;
} [@@deriving sexp, yojson]
let conv (pt : PT.t_list) = {
  metadata = TypeMetadata.conv pt#grab_metadata ;
  cpp_name = pt#get_cpp_name ;
  elem_type = pt#grab_elem_type ;
}
end

module TSet = struct
type t = {
  metadata : TypeMetadata.t ;
  cpp_name : string option ;
  elem_type :  t_type_id ;
} [@@deriving sexp, yojson]
let conv pt = {
  metadata = TypeMetadata.conv pt#grab_metadata ;
  cpp_name = pt#get_cpp_name ;
  elem_type = pt#grab_elem_type ;
}
end
  
module TMap = struct
type t = {
  metadata : TypeMetadata.t ;
  cpp_name : string option ;
  key_type : t_type_id ;
  val_type : t_type_id ;
} [@@deriving sexp, yojson]
let conv pt = {
  metadata = TypeMetadata.conv pt#grab_metadata ;
  cpp_name = pt#get_cpp_name ;
  key_type = pt#grab_key_type ;
  val_type = pt#grab_val_type ;
}
end

module TTypedef = struct  
type t = {
  metadata : TypeMetadata.t ;
  type_ : t_type_id ;
  symbolic : string ;
  forward : bool ;
} [@@deriving sexp, yojson]
let conv pt = {
  metadata = TypeMetadata.conv pt#grab_metadata ;
  type_ = pt#grab_type ;
  symbolic = pt#grab_symbolic ;
  forward = pt#grab_forward ;
}
end
  
module TEnumValue = struct
type t = {
  name : string ;
  value : int32 ;
  annotations : (string, string) map_t option ; 
  doc : string option ;
} [@@deriving sexp, yojson]
let conv pt = {
  name = pt#grab_name ;
  value = pt#grab_value ;
  annotations = pt#get_annotations ;
  doc = pt#get_doc ;
}
end
  
module TEnum = struct
type t = {
  metadata : TypeMetadata.t ;
  constants : TEnumValue.t list ;
} [@@deriving sexp, yojson]
let conv pt = {
  metadata = TypeMetadata.conv pt#grab_metadata ;
  constants = List.map TEnumValue.conv pt#grab_constants
}
end

module TConstValue = struct
type t = 
  | Map_val  of (t, t) map_t
  | List_val of t list
  | String_val of string 
  | Integer_val of int64 
  | Double_val of float 
  | Identifier_val of string 
  | Enum_val of t_type_id [@@deriving sexp, yojson]

let rec conv (pt : PT.t_const_value) =
  if pt#get_map_val <> None then
    Map_val (conv_map conv conv pt#grab_map_val)
  else if pt#get_list_val <> None then
    List_val(List.map conv pt#grab_list_val)
  else if pt#get_string_val <> None then
    String_val pt#grab_string_val
  else if pt#get_integer_val <> None then
    Integer_val pt#grab_integer_val
  else if pt#get_double_val <> None then
    Double_val pt#grab_double_val
  else if pt#get_identifier_val <> None then
    Identifier_val pt#grab_identifier_val
  else if pt#get_enum_val <> None then
    Enum_val pt#grab_enum_val
  else assert false
end

module TConst = struct
type t = {
  name : string ;
  type_ : t_type_id ;
  value : TConstValue.t ;
  doc : string option ;
} [@@deriving sexp, yojson]
let conv pt = {
  name = pt#grab_name ;
  type_ = pt#grab_type ;
  value = TConstValue.conv pt#grab_value ;
  doc = pt#get_doc ;
}
end

module TField = struct
type t = {
  name : string ;
  type_ : t_type_id ;
  key : int32 ;
  req : Requiredness.t ;
  value : TConstValue.t option ;
  reference : bool ;
  annotations : (string, string) map_t option ;
  doc : string option ;
} [@@deriving sexp, yojson]
let conv (pt : PT.t_field) = {
  name = pt#grab_name ;
  type_ = pt#grab_type ;
  key = pt#grab_key ;
  req = pt#grab_req ;
  value = map_option TConstValue.conv pt#get_value ;
  reference = pt#grab_reference ;
  annotations = pt#get_annotations ;
  doc = pt#get_doc ;
}
end

module TStruct = struct
type t = {
  metadata : TypeMetadata.t ;
  members : TField.t list ;
  is_union : bool ;
  is_xception : bool ;
} [@@deriving sexp, yojson]
let conv (pt : PT.t_struct) = {
  metadata = TypeMetadata.conv pt#grab_metadata ;
  members = List.map TField.conv pt#grab_members ;
  is_union = pt#grab_is_union ;
  is_xception = pt#grab_is_xception ;
}
end

module TFunction = struct
type t = {
  name :  string ;
  returntype : t_type_id ;
  arglist : t_type_id ;
  xceptions : t_type_id ;
  is_oneway : bool ;
  doc : string option ;
} [@@deriving sexp, yojson]
let conv pt = {
  name = pt#grab_name ;
  returntype = pt#grab_returntype ;
  arglist = pt#grab_arglist ;
  xceptions = pt#grab_xceptions ;
  is_oneway = pt#grab_is_oneway ;
  doc = pt#get_doc ;
}
end

module TService = struct
type t = {
  metadata : TypeMetadata.t ;
  functions : TFunction.t list ;
  extends_ : t_service_id option ;
} [@@deriving sexp, yojson]
let conv pt = {
  metadata = TypeMetadata.conv pt#grab_metadata ;
  functions = List.map TFunction.conv pt#grab_functions ;
  extends_ = pt#get_extends_ ;
}
end

module TType = struct
type t =
  | Base_type_val of TBaseType.t
  | Typedef_val of TTypedef.t
  | Enum_val of TEnum.t
  | Struct_val of TStruct.t
  | Xception_val of TStruct.t
  | List_val of TList.t
  | Set_val of TSet.t
  | Map_val of TMap.t
  | Service_val of TService.t [@@deriving sexp, yojson]
let conv (pt : PT.t_type) =
  if pt#get_base_type_val <> None then
    Base_type_val (TBaseType.conv pt#grab_base_type_val)
  else if pt#get_typedef_val <> None then
    Typedef_val (TTypedef.conv pt#grab_typedef_val)
  else if pt#get_enum_val <> None then
    Enum_val (TEnum.conv pt#grab_enum_val)
  else if pt#get_struct_val <> None then
    Struct_val (TStruct.conv pt#grab_struct_val)
  else if pt#get_xception_val <> None then
    Xception_val (TStruct.conv pt#grab_xception_val)
  else if pt#get_list_val <> None then
    List_val (TList.conv pt#grab_list_val)
  else if pt#get_set_val <> None then
    Set_val (TSet.conv pt#grab_set_val)
  else if pt#get_map_val <> None then
    Map_val (TMap.conv pt#grab_map_val)
  else if pt#get_service_val <> None then
    Service_val (TService.conv pt#grab_service_val)
  else assert false
end

module TScope = struct
type t = {
  types : t_type_id list ;
  constants: t_const_id list ; 
  services : t_service_id list ;
} [@@deriving sexp, yojson]
let conv pt = {
  types = pt#grab_types ;
  constants = pt#grab_constants ;
  services = pt#grab_services ;
}
end

module TypeRegistry = struct
type t = {
  types : (t_type_id, TType.t) map_t ; 
  constants : (t_const_id, TConst.t) map_t ;
  services : (t_service_id, TService.t) map_t ; 
} [@@deriving sexp, yojson]
let id x = x
let conv (pt : PT.typeRegistry) = {
  types = conv_map id TType.conv pt#grab_types ;
  constants = conv_map id TConst.conv pt#grab_constants ;
  services = conv_map id TService.conv pt#grab_services ; 
}
end

module TProgram = struct
type t = {
  name : string ;
  program_id : t_program_id ;
  path : string ;
  namespace_ : string ;
  out_path : string ;
  out_path_is_absolute : bool ;
  includes : t list ;
  include_prefix : string ;
  scope : TScope.t ;

  typedefs : t_type_id list ; 
  enums : t_type_id list ;
  consts : t_const_id list ;
  objects : t_type_id list ;
  services : t_service_id list ;

  namespaces :(string, string) map_t ; 
  cpp_includes : string list ;
  c_includes : string list;
  doc : string option ;
} [@@deriving sexp, yojson]
  let rec conv (pt : PT.t_program) = {
  name = pt#grab_name ;
  program_id = pt#grab_program_id ;
  path = pt#grab_path ;
  namespace_ = pt#grab_namespace_ ;
  out_path = pt#grab_out_path ;
  out_path_is_absolute = pt#grab_out_path_is_absolute ;
  includes = List.map conv pt#grab_includes ;
  include_prefix = pt#grab_include_prefix ;
  scope = TScope.conv pt#grab_scope ;

  typedefs = pt#grab_typedefs ;
  enums = pt#grab_enums ;
  consts = pt#grab_consts ;
  objects = pt#grab_objects ;
  services = pt#grab_services ;

  namespaces = pt#grab_namespaces ;
  cpp_includes = pt#grab_cpp_includes ;
  c_includes = pt#grab_c_includes ;
  doc = pt#get_doc ;
}
end

module GeneratorInput = struct
type t = {
 program : TProgram.t ;
 type_registry : TypeRegistry.t ;
 parsed_options : (string, string) map_t 
} [@@deriving sexp, yojson]
let conv (pt : PT.generatorInput) = {
  program = TProgram.conv pt#grab_program ;
  type_registry = TypeRegistry.conv pt#grab_type_registry ;
  parsed_options = pt#grab_parsed_options ;
}
end
  
