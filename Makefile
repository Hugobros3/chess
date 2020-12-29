all: chess_core helper

chess_core:
	artic chess.art ../runtime/platforms/artic/* --emit-llvm --emit-c-interface -g

helper:
	clang++ chess.ll main.cpp -o chess -g -Wall -Wpedantic -Wextra -std=c++17
