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

Bookmark* doFile(BFile& file)
{
	Bookmark* result = new Bookmark();

	BNodeInfo nodeInfo(&file);

	char buffer[B_MIME_TYPE_LENGTH];
	nodeInfo.GetType(buffer);

	if (BString(buffer) != "application/x-vnd.Be-bookmark")
		return NULL;

	BString title;

	struct attr_info info;
	char attrBuffer[B_ATTR_NAME_LENGTH];
	while (file.GetNextAttrName(attrBuffer) == B_OK) {
		if (file.GetAttrInfo(attrBuffer, &info) != B_OK)
			continue;

		uint8_t* data = new (std::nothrow) uint8_t[info.size];
		if (data == NULL)
			continue;

		if (file.ReadAttr(attrBuffer, info.type, 0, data, info.size) ==
			info.size) {
			BString name(attrBuffer);
			if (name == "META:url") {
				result->SetURL((const char *)data);
			} else if (name == "META:title") {
				result->SetTitle((const char *)data);
			} else if (name == "META:keyw") {
				result->SetKeywords((const char *)data);
			}
		}
		delete[] data;
	}

	return result;
}

BookmarksFolder* doDirectory(BDirectory& dir, const char* name)
{
	BookmarksFolder* result = new BookmarksFolder();
	result->SetName(name);

	BEntry current;
	dir.Rewind();
	while (dir.GetNextEntry(&current) == B_OK) {
		BookmarksEntry* entry = NULL;
		if (current.IsDirectory()) {
			BDirectory subdir(&current);

			char buffer[B_FILE_NAME_LENGTH];
			if (current.GetName(buffer) == B_OK)
				entry = doDirectory(subdir, buffer);
			else
				entry = doDirectory(subdir, "Unknown");
		}
		else if (current.IsFile()) {
			Bookmark* file;
			BFile subfile(&current, B_READ_ONLY);
			entry = doFile(subfile);
		}

		if (entry != NULL)
			result->push_back(entry);
	}

	return result;
}

status_t extract(const char* path, const char* destination,
	BookmarksOutput* format)
{
	BDirectory parent(path);
	if (parent.InitCheck() != B_OK)
		return B_ERROR;

	BookmarksFolder* result = doDirectory(parent, NULL);

	format->Output(result, destination);

	delete result;

	return B_OK;
}

int helpMessage(int code, BookmarksOutput* toDelete)
{
	delete toDelete;
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
	BookmarksOutput* chosen = NULL;
	BString source;
	BString destination;
	int mode;
	int curarg = 0;
	while (curarg + 1 < argc) {
		curarg++;
		BString current(argv[curarg]);
		if (current == "-h" || current == "--help")
			return helpMessage(0, chosen);
		else if (current == "-t" || current == "--to") {
			if (curarg == argc - 1)
				return helpMessage(1, chosen);
			else {
				curarg++;
				BString format(argv[curarg]);
				if (format.ICompare("html") == 0)
					chosen = new HTMLOutput();
				else if (format.ICompare("chrome") == 0)
					chosen = new ChromeOutput();
				else
					return helpMessage(2, chosen);
			}
		} else if (current == "-i") {
			if (curarg == argc - 1)
				return helpMessage(4, chosen);
			else {
				curarg++;
				source = argv[curarg];
			}
		} else if (current == "-o") {
			if (curarg == argc - 1)
				return helpMessage(5, chosen);
			else {
				curarg++;
				destination = argv[curarg];
			}
		} else {
			return helpMessage(3, chosen);
		}
	}

	if (source == "") {
		BPath dir;
		if (find_directory(B_USER_SETTINGS_DIRECTORY, &dir) == B_OK) {
			source = dir.Path();
			source << "/WebPositive/Bookmarks/";
		}
	}

	if (chosen == NULL)
		chosen = new HTMLOutput();

	extract(source.String(), destination.String(), chosen);

	delete chosen;
	return 0;
}

