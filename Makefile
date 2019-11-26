all:
	gcc -O2 -o fari src/main.c src/fari.c src/file.c src/fork.c src/json.c src/lib/cJSON.c
debug:
	gcc -std=c90 -pedantic-errors -Wall -Wextra -Wswitch-default -Wwrite-strings -pipe -Og -g -o fari src/main.c src/fari.c src/file.c src/fork.c src/json.c src/lib/cJSON.c
