
OCAMLCFLAGS = -thread
OCAMLMKLIB_FLAGS= -L`pwd`/../cpp -lnicejson  $(thrift_LIBS) $(BOOST_FILESYSTEM_LDADD) $(BOOST_SYSTEM_LDADD)  -lstdc++
CXXFLAGS += -g2 -I../cpp -I. -I`$(OCAMLFIND) query cppffigen` -I$(OCAMLLIB) -std=gnu++11 $(AM_CXXFLAGS) \
	-fPIC $(thrift_CFLAGS) $(AM_CXXFLAGS)

PACKAGES = -package result,threads,oUnit,thrift,yojson

RESULT = ocaml_nicejson

BUILT_SOURCES = $(RESULT).ml $(RESULT).mli  $(RESULT)_stubs.cc
cleanfiles = $(BUILT_SOURCES)

ML = $(RESULT).ml nicejson.ml
MLI = $(RESULT).mli

CMO= $(RESULT).cmo nicejson.cmo
CMX= $(RESULT).cmx nicejson.cmx
CMI= $(RESULT).cmi nicejson.cmi
OBJECTS = $(CMO) $(CMX) $(CMI)


all-local: $(RESULT).cma $(RESULT).cmxa dll$(RESULT).so

$(RESULT).cma $(RESULT).cmxa dll$(RESULT).so: $(OBJECTS) $(RESULT)_stubs.o nicejson_intf.o
	    $(OCAMLMKLIB) -verbose -o $(RESULT) $(CMO) $(CMX) $(RESULT)_stubs.o nicejson_intf.o $(OCAMLMKLIB_FLAGS)


$(RESULT)_stubs.o: $(RESULT)_stubs.cc
	g++ -c -fPIC ${CXXFLAGS} -DPIC -o $(RESULT)_stubs.o $(RESULT)_stubs.cc

nicejson_intf.o: nicejson_intf.cc
	g++ -c -fPIC ${CXXFLAGS} -DPIC -o $@ $<

$(RESULT).ml $(RESULT).mli $(RESULT)_stubs.cc: nicejson-ffi.idl
	cppffigen --output ml < nicejson-ffi.idl > $(RESULT).ml
	cppffigen --output mli < nicejson-ffi.idl > $(RESULT).mli
	cppffigen --output cpp < nicejson-ffi.idl > $(RESULT)_stubs.cc

.ml.cmo:
	$(OCAMLFIND) ocamlc $(OCAMLCFLAGS) $(PACKAGES) -c $<

.ml.cmx:
	$(OCAMLFIND) ocamlopt $(OCAMLCFLAGS) $(PACKAGES) -c $<

.mli.cmi:
	$(OCAMLFIND) ocamlc $(OCAMLCFLAGS) $(PACKAGES) -c $<

clean-local:
	rm -f *.a *.cma *.cmi *.cmo *.cmx *.cmxa *.o *.so \
		$(RESULT).ml $(RESULT).mli $(RESULT)_stubs.cc

ocaml_nicejson.cmi: ocaml_nicejson.mli
ocaml_nicejson.cmo: ocaml_nicejson.cmi
ocaml_nicejson.cmx: ocaml_nicejson.cmi
nicejson.cmo nicejson.cmx: ocaml_nicejson.cmi
nicejson.cmi: nicejson.ml

.NOTPARALLEL:
