TESTS = $(wildcard tests/*.java)
SRC = extract.cpp
SRC2 = extract_ranjan.cpp
CXX_FLAGS = -O2 -std=c++11 -Wall -Wno-c++11-extensions
LD_FLAGS = -lboost_regex -lboost_filesystem -lboost_system -L/opt/boost/lib
OUT = ./extract
OUT2 = ./extract_ranjan

default: run
build: $(OUT)
clean:
	rm -f *~ $(OUT) $(OUT2) *.o

$(OUT): $(SRC)
	g++ $(CXX_FLAGS) $(SRC) $(LD_FLAGS) -o $(OUT)

$(OUT2): $(SRC2)
	g++ $(CXX_FLAGS) $(SRC2) $(LD_FLAGS) -o $(OUT2)

run: $(OUT) $(OUT2)
ifeq ($(strip ${REPEAT}),)
	@echo "Sriram Test"
	@for f in $(TESTS); do $(OUT) $$f ${DEBUG} ; done
	@echo
else
	@echo "Sriram Test"
	@echo "== Repeating every test ${REPEAT} times =="
	@echo
	@for f in $(TESTS); do $(OUT) $$f ${DEBUG} --repeat=${REPEAT}; done
	@echo
endif
ifeq ($(strip ${REPEAT}),)
	@echo "Ranjan Test"
	@for f in $(TESTS); do $(OUT2) $$f ${DEBUG} ; done
	@echo
else
	@echo "Ranjan Test"
	@echo "== Repeating every test ${REPEAT} times =="
	@echo
	@for f in $(TESTS); do $(OUT2) $$f ${DEBUG} --repeat=${REPEAT}; done
	@echo
endif


ranjan: $(OUT2)
ifeq ($(strip ${REPEAT}),)
	@echo
	@for f in $(TESTS); do $(OUT2) $$f ${DEBUG} ; done
	@echo
else
	@echo
	@echo "== Repeating every test ${REPEAT} times =="
	@echo
	@for f in $(TESTS); do $(OUT2) $$f ${DEBUG} --repeat=${REPEAT}; done
	@echo
endif
