
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
