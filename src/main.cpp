/*
 * main.cpp
 * Copyright (c) 2015 Markus Himmel. All rights reserved.
 * Distributed under the terms of the MIT license.
 */

#include <algorithm>
#include <iostream>
#include <limits>

#include <Directory.h>
#include <Entry.h>
#include <File.h>
#include <FindDirectory.h>
#include <NodeInfo.h>
#include <Path.h>
#include <String.h>

#include <fs_attr.h>

#include "BookmarksTree.h"
#include "BookmarksFormat.h"

int helpMessage(int code, BookmarksOutput* a, BookmarksInput* b)
{
	delete a;
	delete b;
	std::cout	<< "Converts WebPositive bookmarks to various formats."
				<< std::endl << std::endl
				<< "Usage: bookmarkconverter [option] [path]"
				<< std::endl << std::endl
				<< "    -f --format FORMAT    "
				<< "Chooses the format (HTML, CHROME)" << std::endl
				<< "    -h --help             "
				<< "Displays this message" << std::endl << std::endl
				<< "    path                  "
				<< "Optional. Sets the path to the bookmarks folder."
				<< std::endl << "                           "
				<< "Default: ~/config/settings/WebPositive/Bookmarks";
	return code;
}

int main(int argc, char* argv[])
{
	BookmarksOutput* output = NULL;
	BookmarksInput* input = NULL;
	BString source;
	BString destination;
	int mode;
	int curarg = 0;
	while (curarg + 1 < argc) {
		curarg++;
		BString current(argv[curarg]);
		if (current == "-h" || current == "--help")
			return helpMessage(0, output, input);
		else if (current == "-t" || current == "--to") {
			if (curarg == argc - 1)
				return helpMessage(1, output, input);
			else {
				curarg++;
				BString format(argv[curarg]);
				if (format.ICompare("html") == 0)
					output = new HTMLOutput();
				else if (format.ICompare("chrome") == 0)
					output = new ChromeOutput();
				else
					return helpMessage(2, output, input);
			}
		} else if (current == "-f" || current == "--from") {
			if (curarg == argc - 1)
				return helpMessage(6, output, input);
			else {
				curarg++;
				BString format(argv[curarg]);
				if (format.ICompare("be") == 0)
					input = new BeInput();
				else
					return helpMessage(7, output, input);
			}
		} else if (current == "-i") {
			if (curarg == argc - 1)
				return helpMessage(4, output, input);
			else {
				curarg++;
				source = argv[curarg];
			}
		} else if (current == "-o") {
			if (curarg == argc - 1)
				return helpMessage(5, output, input);
			else {
				curarg++;
				destination = argv[curarg];
			}
		} else {
			return helpMessage(3, output, input);
		}
	}

	if (input == NULL)
		input = new BeInput();

	if (output == NULL)
		output = new HTMLOutput();

	BookmarksEntry* read = input->Input(source.String());
	if (read != NULL)
		output->Output(read, destination.String());
	else
		std::cerr << "There was an error reading the input";

	delete input;
	delete output;
	return 0;
}

