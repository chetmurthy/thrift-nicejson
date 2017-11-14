OCAMLCFLAGS = -thread -I ../../lib/ocaml
PACKAGES = -package result,threads,oUnit
RESULT = ocaml_somecode

check-local: all
	./somecode_tests.byte

all-local: somecode_tests.byte

somecode_tests.byte:
	$(OCAMLFIND) ocamlc -custom $(OCAMLCFLAGS) $(PACKAGES) -linkpkg -linkall -o somecode_tests.byte ../../lib/ocaml/$(RESULT).cma  -cclib -L../../lib/ocaml somecode_tests.ml -cclib -lstdc++
