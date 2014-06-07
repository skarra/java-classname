TESTS = $(wildcard tests/*.java)
SRC = fix.cpp
LD_FLAGS = -lboost_regex
OUT = ./fix

default: run
clean:
	rm -f *~ $(OUT) *.o

$(OUT): $(SRC)
	g++ $(SRC) $(LD_FLAGS) -o $(OUT)

run: $(OUT)
	@for f in $(TESTS); do $(OUT) $$f; done
