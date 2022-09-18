run: build
	./a.out $(arg)

build: a.out

a.out: 1.cpp utils.cpp utils.h
	clang++ 1.cpp utils.cpp

clean:
	rm out_*.txt *.out
