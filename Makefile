TESTS = $(wildcard tests/*.java)
SRC = fix.cpp
CXX_FLAGS = -O2 -std=c++11 -Wall -Wno-c++11-extensions
LD_FLAGS = -lboost_regex -lboost_filesystem -lboost_system
OUT = ./fix

default: run
build: $(OUT)
clean:
	rm -f *~ $(OUT) *.o

$(OUT): $(SRC)
	g++ $(CXX_FLAGS) $(SRC) $(LD_FLAGS) -o $(OUT)

run: $(OUT)
ifeq ($(strip ${REPEAT}),)
	@echo
	@for f in $(TESTS); do $(OUT) $$f; done
	@echo
else
	@echo
	@echo "== Repeating every test ${REPEAT} times =="
	@echo
	@for f in $(TESTS); do $(OUT) $$f --repeat=${REPEAT}; done
	@echo
endif
