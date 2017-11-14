
OCAMLCFLAGS = -thread
OCAMLMKLIB_FLAGS= -lstdc++
CXXFLAGS += -g2 -I. -I`$(OCAMLFIND) query cppffigen` -I$(OCAMLLIB) -std=gnu++11 $(AM_CXXFLAGS)

PACKAGES = -package result,threads,oUnit

RESULT = ocaml_somecode

BUILT_SOURCES = $(RESULT).ml $(RESULT).mli  $(RESULT)_stubs.cc
cleanfiles = $(BUILT_SOURCES)

ML = $(RESULT).ml some_code.ml
MLI = $(RESULT).mli

CMO= $(RESULT).cmo some_code.cmo
CMX= $(RESULT).cmx some_code.cmx
CMI= $(RESULT).cmi some_code.cmi
OBJECTS = $(CMO) $(CMX) $(CMI)


all-local: $(RESULT).cma $(RESULT).cmxa dll$(RESULT).so

$(RESULT).cma $(RESULT).cmxa dll$(RESULT).so: $(OBJECTS) $(RESULT)_stubs.o somecode.o
	    $(OCAMLMKLIB) -verbose -o $(RESULT) $(CMO) $(CMX) $(RESULT)_stubs.o somecode.o $(OCAMLMKLIB_FLAGS)


$(RESULT)_stubs.o: $(RESULT)_stubs.cc
	g++ -c -fPIC ${CXXFLAGS} -DPIC -o $(RESULT)_stubs.o $(RESULT)_stubs.cc

somecode.o: somecode.cc
	g++ -c -fPIC ${CXXFLAGS} -DPIC -o $@ $<

$(RESULT).ml $(RESULT).mli $(RESULT)_stubs.cc: somecode-ffi.idl
	cppffigen --output ml < somecode-ffi.idl > $(RESULT).ml
	cppffigen --output mli < somecode-ffi.idl > $(RESULT).mli
	cppffigen --output cpp < somecode-ffi.idl > $(RESULT)_stubs.cc

.ml.cmo:
	$(OCAMLFIND) ocamlc $(OCAMLCFLAGS) $(PACKAGES) -c $<

.ml.cmx:
	$(OCAMLFIND) ocamlopt $(OCAMLCFLAGS) $(PACKAGES) -c $<

.mli.cmi:
	$(OCAMLFIND) ocamlc $(OCAMLCFLAGS) $(PACKAGES) -c $<

clean-local:
	rm -f *.a *.cma *.cmi *.cmo *.cmx *.cmxa *.o *.so \
		$(RESULT).ml $(RESULT).mli $(RESULT)_stubs.cc

ocaml_somecode.cmo: ocaml_somecode.cmi
ocaml_somecode.cmx: ocaml_somecode.cmi
