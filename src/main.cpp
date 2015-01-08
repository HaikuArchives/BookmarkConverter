/*
 * main.cpp
 * Copyright (c) 2015 Markus Himmel. All rights reserved.
 * Distributed under the terms of the MIT license.
 */

#include <algorithm>
#include <iostream>

#include <String.h>
#include <Directory.h>

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
	int mode;
	int curarg = 0;
	BString paths[2];
	int curpath = 0;

	while (curarg + 1 < argc) {
		curarg++;
		BString current(argv[curarg]);
		if (current == "-h" || current == "--help")
			return helpMessage(0, output, input);
		else if (current == "-f" || current == "--format") {
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
				else if (format.ICompare("qupzilla") == 0)
					output = new QupZillaOutput();
				else
					return helpMessage(2, output, input);
			}
		} else if (current == "--webpositive-import") {
			input = new BeInput();
			paths[0] = "";
			curpath = std::max(curpath, 1);
		} else if (current == "--qupzilla-import") {
			input = new QupZillaInput();
			paths[0] = "";
			curpath == std::max(curpath, 1);
		} else if (curpath >= 2)
			return helpMessage(5, output, input);
		else
			paths[curpath++] = argv[curarg];

	}

	if (input == NULL) {
		BDirectory test(paths[0].String());
		input = (test.InitCheck() == B_OK) ?
			new BeInput() : new QupZillaInput();
	}

	if (output == NULL)
		return helpMessage(4, output, input);

	BookmarksEntry* read = input->Input(paths[0].String());
	if (read != NULL)
		output->Output(read, paths[1].String());
	else
		std::cerr << "There was an error reading the input" << std::endl;

	delete input;
	delete output;
	return 0;
}

