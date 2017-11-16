class write_t :
  Buffer.t ->
  object
    val buffer : Buffer.t
    method close : unit
    method flush : unit
    method isOpen : bool
    method opn : unit
    method read : string -> int -> int -> int
    method readAll : string -> int -> int -> int
    method write : string -> int -> int -> unit
  end
class read_t :
  string ->
  object
    val buffer : string
    val mutable cursor : int
    method close : unit
    method flush : unit
    method isOpen : bool
    method opn : unit
    method read : string -> int -> int -> int
    method readAll : string -> int -> int -> int
    method write : string -> int -> int -> unit
  end
