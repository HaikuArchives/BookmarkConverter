/*
 * BeFormat.cpp
 * Copyright (c) 2015 Markus Himmel. All rights reserved.
 * Distributed under the terms of the MIT license.
 */

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
			if (name == "META:url")
				result->SetURL((const char *)data);
			else if (name == "META:title")
				result->SetTitle((const char *)data);
			else if (name == "META:keyw")
				result->SetKeywords((const char *)data);
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
	BDirectory parent(path);
	if (parent.InitCheck() != B_OK)
		return NULL;

	return readDirectory(parent, NULL);
}

BeOutput::BeOutput()
{
}

BeOutput::~BeOutput()
{
}

void BeOutput::Output(BookmarksEntry* entry, const char* destination)
{
	BPath loc;
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &loc) == B_OK) {
		BString path(loc.Path());
		path << "/WebPositive/Bookmarks/";
		std::cerr << "Put the generated files into " << path.String()
			<< " for use with WebPositive." << std::endl;
	}


	BDirectory dir(destination);
	if (dir.InitCheck() == B_OK)
		HandleItem(entry, true, dir);
	else {
		BDirectory created;
		dir.CreateDirectory(destination, &created);
		if (created.InitCheck() == B_OK)
			HandleItem(entry, true, created);
	}
}

void BeOutput::HandleItem(BookmarksEntry* entry, bool first, BDirectory& dest)
{
	if (entry->IsFolder())
		OutputDirectory(*static_cast<BookmarksFolder*>(entry), first, dest);
	else if (entry->IsBookmark())
		OutputBookmark(*static_cast<Bookmark*>(entry), dest);
}

void BeOutput::OutputDirectory(BookmarksFolder& entry, bool first,
	BDirectory& dest)
{
	BDirectory next = dest;
	if (!first) {
		BDirectory tryCreate;
		status_t result = dest.CreateDirectory(entry.GetName(), &tryCreate);
		if (result == B_OK || result == B_FILE_EXISTS)
			next = tryCreate;
	}
	
	BookmarksFolder::iterator it;
	for (it = entry.begin(); it != entry.end(); it++)
		HandleItem(*it, false, next);
}

void BeOutput::OutputBookmark(Bookmark& entry, BDirectory& dest)
{
	BFile sink;
	if (dest.CreateFile(entry.GetTitle(), &sink, false) != B_OK)
		return;

	sink.WriteAttr("META:title", 'CSTR', 0, entry.GetTitle(),
		BString(entry.GetTitle()).Length() + 1);
	sink.WriteAttr("META:url", 'CSTR', 0, entry.GetURL(),
		BString(entry.GetURL()).Length() + 1);

	if (BString(entry.GetKeywords()) != "") {
		sink.WriteAttr("META:keyw", 'CSTR', 0, entry.GetKeywords(),
			BString(entry.GetKeywords()).Length() + 1);
	}

	BNodeInfo info(&sink);
	info.SetType("application/x-vnd.Be-bookmark");
}

