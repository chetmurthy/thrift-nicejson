OCAMLCFLAGS = -thread -I ../../lib/ocaml
PACKAGES = -package result,threads,oUnit
RESULT = ocaml_nicejson

check-local: all
	./nicejson_tests.byte

all-local: nicejson_tests.byte

nicejson_tests.byte:
	$(OCAMLFIND) ocamlc -custom $(OCAMLCFLAGS) $(PACKAGES) -linkpkg -linkall -o nicejson_tests.byte ../../lib/ocaml/$(RESULT).cma  -cclib -L../../lib/ocaml nicejson_tests.ml -cclib -lstdc++

clean:
	rm -rf oUnit* *.cm* *.byte
