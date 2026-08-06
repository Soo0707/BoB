#!/bin/bash

g++ tests/test.cpp\
	-o /mnt/tmpfs/test\
	-std=c++20\
	-O3\
	-Iinclude\
	-Itests/headers\
	-D_GLIBCXX_DEBUG\
	-Wshadow\
	-Wconversion\
	-march=native\
	-DNDEBUG\
	-g\
	-fno-omit-frame-pointer\
	-fsanitize=undefined\
	-fsanitize=leak\
	-fsanitize=address\
	#-fsanitize=thread
