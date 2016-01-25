target=3rd
cc=clang++

$(target) :  3rd.cpp parser_helper.cpp
	$(cc) -std=c++11 -o $@ $+
