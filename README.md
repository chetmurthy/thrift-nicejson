# thrift-nicejson: A Nice JSON protocol for Thrift

This library provides a "nice" JSON wire-protocol for Thrift.
That means you can define data-types/messages in Thrift, turn a crank,
and get out de/marshallers and a protocol stack that talk all of
Thrift's protocols, and also talk *idiomatic* JSON.

For example, the (comes with) Thrift Calculator tutorial example has
an `i32 add(1:i32 num1, 2:i32 num2)` method, which the client invokes
as `add(1, 1)`.  With this library, you can do that with POST:

```
% POST -c 'application/x-thrift' http://localhost:9090/
Please enter content (application/x-thrift) to be POSTed:
{"body":{"num1":1,"num2":1},"name":"add","seqid":0,"type":"call"}
<EOF>
```

replies with

```
{"body":{"success":2},"name":"add","seqid":0,"type":"reply"}
```

The implementation is in C++, and via FFI it can be made available to
other Thrift languages, e.g. currently Python and Ocaml.

## Installation

This library requires a relatively recent Thrift installation (version
0.10.0 or newer) and *also* a Thrift source-tree (for access to a few
files from the compiler).  I've only tested the build against source
that matches the binary Thrift installation, but perhaps it'll work if
there's a mismatch.

Build/install is straightforward, and is fully autotool-ized:

```
% ./configure --with-thrift-srcdir=<thrift-source-dir-here>
% make all check
% make install
```

Of course, if you wish to install, it's probably wise to pick a
nonstandard location, e.g. ```$HOME/tmp/thrift-nicejson``` via

```
% ./configure --prefix=$HOME/tmp/thrift-nicejson --with-thrift-srcdir=<thrift-source-dir-here>
```

Examples can be found in `src/test/cpp` and `src/test/py`.  In
particular, `src/test/cpp` demonstrates most of the capabilities of
the library.

## Why does Thrift need "nice JSON" serialization?

I'm a big fan of Apache Thrift, for many reasons:

1. modular, so you can modify bits of the underlying infrastructure
   without forking the entire thing.  I've
  
  * ported (C++) Thrift to run on Infiniband ibverbs
  * modified the way server- and client-side wrappers are initialized,
    so that a connection established from process P1 to process P2,
    can be used by process P2 to send RPCs to process P1
  * and so that the connection was initially established using
    socketpair(2) and not connect(2).
	
2. Supports my favorite programming language (Objective Caml)

But there's *one* big, big missing feature (for me): Thrift doesn't

1. have a human-readable wire-representation like Protocol Buffers'
"Compact Text Format", or (with Protocol Buffers 3) JSON.

2. And furthermore, if you have a (binary) serialized thrift message,
there's no quick, simple way to dump it in a format where all the
fields are there

3. and where the format is one that can be re-marshaled back into
   Thrift's binary serialized format.
   
In short, Thrift needs a human-readable wire-format, and a "type
library" facility so that one can use to "disassemble"
binary-serialized messages to that format.

This library provides those facilities.

## What this library does **not** do

This library supports de/serializing Thrift objects to JSON, in a
format that is strictly determined by the IDL.  So there's no
flexibility of naming fields, nor of data-types (though one might
imagine adding that).

## Licensing

All files are governed by the Apache license (included herein in file
LICENSE) except for json.hpp, which is governed by the MIT License,
and a number of GNU autotools files, which are all governed by their
own licenses, typically GPL.

## Credits and Thanks

First, this code is descended by modification from code and examples
found in Apache Thrift.  In many filies I've left the copyright
headers as-is, b/c as far as I'm concerned, I'm happy to cede
ownership of this code to Apache, with the following exceptions:

* The file "json.hpp" was copied with permission from
  [JSON for Modern C++](https://github.com/nlohmann/json) created by
  Neils Lohmann, and he continues to own it.  He licenses that file
  under the MIT license (included in the source). This thing is bloody
  lovely marvel.

* And of course, there aare a bunch of GNU files, owned by their
  respective owners.

