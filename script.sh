#!/bin/sh
gcc -Wall client.c -o client -lm
valgrind --leak-check=full ./client
