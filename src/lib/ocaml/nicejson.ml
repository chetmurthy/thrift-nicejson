
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
