#!/bin/bash

g++ tests/test.cpp\
	-o /mnt/tmpfs/test\
	-std=c++20\
	-O3\
	-g\
	-Iinclude\
	-Itests/headers\
	-fno-omit-frame-pointer\
	-fsanitize=address\
	-fsanitize=undefined\
	-fsanitize=leak\
	-D_GLIBCXX_DEBUG\
	-Wshadow\
	-Wconversion\
	-march=native\
	#-fsanitize=thread\
	#-DNDEBUG
