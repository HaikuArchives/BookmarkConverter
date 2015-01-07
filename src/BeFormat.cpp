/*
 * BeFormat.cpp
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

Bookmark* BeInput::readFile(BFile& file)
{

	BNodeInfo nodeInfo(&file);

	char buffer[B_MIME_TYPE_LENGTH];
	nodeInfo.GetType(buffer);

	if (BString(buffer) != "application/x-vnd.Be-bookmark")
		return NULL;

	Bookmark* result = new Bookmark();
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

BookmarksFolder* BeInput::readDirectory(BDirectory& dir, const char* name)
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
				entry = readDirectory(subdir, buffer);
			else
				entry = readDirectory(subdir, "Unknown");
		}
		else if (current.IsFile()) {
			Bookmark* file;
			BFile subfile(&current, B_READ_ONLY);
			entry = readFile(subfile);
		}

		if (entry != NULL)
			result->push_back(entry);
	}

	return result;
}

BeInput::BeInput()
{
}

BeInput::~BeInput()
{
}

BookmarksEntry* BeInput::Input(const char* source)
{
	BString path(source);
	if (path == "") {
		BPath dir;
		if (find_directory(B_USER_SETTINGS_DIRECTORY, &dir) == B_OK) {
			path = dir.Path();
			path << "/WebPositive/Bookmarks/";
		}
	}
	BDirectory parent(path);
	if (parent.InitCheck() != B_OK)
		return NULL;

	return readDirectory(parent, NULL);
}
