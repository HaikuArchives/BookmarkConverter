/*
 * main.cpp
 * Copyright (c) 2015 Markus Himmel. All rights reserved.
 * Distributed under the terms of the MIT license.
 */

#include <iostream>

#include <String.h>

#include "BookmarksTree.h"
#include "BookmarksFormat.h"

int helpMessage(int code, BookmarksOutput* a, BookmarksInput* b)
{
	delete a;
	delete b;
	std::cout	<< "Converts browser bookmarks between a selection of formats."
				<< std::endl << std::endl
				<< "Usage: bookmarkconverter [options]"
				<< std::endl << std::endl
				<< "    -f --from    FORMAT   "
				<< "The source format (WEBPOSITIVE, QUPZILLA)" << std::endl
				<< "    -t --to      FORMAT   "
				<< "The destination format (HTML, CHROME, WEBPOSITIVE)"
				<< std::endl
				<< "    -i           PATH     "
				<< "The location of the input" << std::endl
				<< "    -o           PATH     "
				<< "Where the output will be written" << std::endl
				<< "    -h --help             "
				<< "Displays this help message." << std::endl;
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
				else if (format.ICompare("webpositive") == 0)
					output = new BeOutput();
				else
					return helpMessage(2, output, input);
			}
		} else if (current == "-f" || current == "--from") {
			if (curarg == argc - 1)
				return helpMessage(6, output, input);
			else {
				curarg++;
				BString format(argv[curarg]);
				if (format.ICompare("webpositive") == 0)
					input = new BeInput();
				else if (format.ICompare("qupzilla") == 0)
					input = new QupZillaInput();
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
		std::cerr << "There was an error reading the input" << std::endl;

	delete input;
	delete output;
	return 0;
}

