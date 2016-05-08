all: no

no : lang.cc
	g++ lang.cc -std=c++11 -o no -O2

fmt : lang.cc
	clang-format -i lang.cc

clean:
	rm no

.PHONY: all clean fmt
