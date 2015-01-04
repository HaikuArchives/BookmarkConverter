/*
 * main.cpp
 * Copyright (c) 2015 Markus Himmel. All rights reserved.
 * Distributed under the terms of the MIT license.
 */

#include <Directory.h>

#include <iostream>

status_t extract(const char* path)
{
	std::cout << path << std::endl;
	BDirectory parent(path);
}

int main(int argc, char* argv[])
{
	switch (argc) {
		case 1:
			break;
		case 2:
			extract(argv[1]);
			break;
		default:
			std::cerr << "Too many arguments." << std::endl;
			return 1;
	}
	return 0;
}
