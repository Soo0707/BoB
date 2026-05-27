#!/bin/bash

g++ test.cpp\
	-o /mnt/tmpfs/test\
	-std=c++20\
	-Iheaders\
	-Wno-changes-meaning\
	-fno-omit-frame-pointer\
	-fsanitize=address\
	-fsanitize=undefined
