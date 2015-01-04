/*
 * main.cpp
 * Copyright (c) 2015 Markus Himmel. All rights reserved.
 * Distributed under the terms of the MIT license.
 */

#include <Directory.h>
#include <Entry.h>
#include <File.h>
#include <String.h>
#include <NodeInfo.h>
#include <iostream>
#include <limits>
#include <fs_attr.h>

BString* handleAttribute(BString name, const void *data)
{
	if (name == "META:url") {
		std::cout << " HREF=\"" << (const char*)data << "\"";
	} else if (name == "META:title") {
		return new BString((const char *)data);
	} else if (name == "BEOS:ICON") {
		// ?
	} else if (name == "META:keyw") {
		// ?
	}
	return NULL;
}

status_t doFile(BFile* file, int indent)
{
	if (file == NULL)
		return B_BAD_VALUE;

	BNodeInfo nodeInfo(file);
	char buffer[B_MIME_TYPE_LENGTH];
	nodeInfo.GetType(buffer);

	if (BString(buffer) != BString("application/x-vnd.Be-bookmark"))
		return B_BAD_VALUE;

	BString ind;
	ind.Append(' ', indent);
	std::cout << ind << "<DT><A";

	BString* title;

	struct attr_info info;
	char attrBuffer[B_ATTR_NAME_LENGTH];
	while (file->GetNextAttrName(attrBuffer) == B_OK) {
		if (file->GetAttrInfo(attrBuffer, &info) != B_OK)
			continue;

		uint8_t* data = new (std::nothrow) uint8_t[info.size];
		if (data == NULL)
			continue;

		if (file->ReadAttr(attrBuffer, info.type, 0, data, info.size) ==
			info.size) {

			BString* maybeTitle = handleAttribute(BString(attrBuffer),
				data);

			if (maybeTitle != NULL)
				title = maybeTitle;
		}
		delete[] data;
	}
	
	if (title == NULL)
		title = new BString("Unknown");

	std::cout << ">" << *title << "</A>" << std::endl;

	delete title;
	return B_OK;
}

status_t getLastChanged(BDirectory* dir, time_t* out)
{
	if (dir == NULL || out == NULL)
		return B_BAD_VALUE;

	time_t latest = std::numeric_limits<time_t>::min(), cTime;
	BEntry current;
	bool any = false;

	while (dir->GetNextEntry(&current) == B_OK) {
		if (current.GetCreationTime(&cTime) == B_OK && cTime > latest) {
			any = true;
			latest = cTime;
		}
	}

	if (any) {
		*out = latest;
		return B_OK;
	}
	return B_ERROR;
}

status_t doDirectory(BDirectory* dir, const char* name, int indent)
{
	if (dir == NULL)
		return B_BAD_VALUE;

	BString ind;
	ind.Append(' ', indent);

	if (indent > 0) {
		std::cout << ind << "<DT><H3";
		time_t addDate;
		if (dir->GetCreationTime(&addDate) == B_OK) {
			std::cout << " ADD_DATE=\"" << addDate << "\" ";

			time_t lastModified = addDate;
			getLastChanged(dir, &lastModified);

			std::cout << "LAST_MODIFIED=\"" << lastModified << "\"";
		}
		std::cout << ">" << name << "</H3>" << std::endl << ind << "<DL><p>"
			<< std::endl;
	}

	BEntry current;
	dir->Rewind();
	while (dir->GetNextEntry(&current) == B_OK) {
		if (current.IsDirectory()) {
			BDirectory subdir(&current);

			char buffer[B_FILE_NAME_LENGTH];
			if (current.GetName(buffer) == B_OK)
				doDirectory(&subdir, buffer, indent + 4);
			else
				doDirectory(&subdir, "Unknown", indent + 4);
		}
		else if (current.IsFile()) {
			BFile subfile(&current, B_READ_ONLY);
			doFile(&subfile, indent + 4);
		}
	}

	if (indent > 0)
		std::cout << ind << "</DL><p>" << std::endl;

	return B_OK;
}

status_t extract(const char* path)
{
	std::cout	<< "<!DOCTYPE NETSCAPE-Bookmark-file-1>" << std::endl
				<< "<!-- This is an automatically generated file." << std::endl
				<< "     It will be read and overwritten." << std::endl
				<< "     DO NOT EDIT! -->" << std::endl
				<< "<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html;"
				<< " charset=UTF-8\">" << std::endl
				<< "<TITLE>Bookmarks</TITLE>" << std::endl
				<< "<H1>Bookmarks</H1>" << std::endl
				<< "<DL><p>" << std::endl;

	BDirectory parent(path);
	if (parent.InitCheck() != B_OK)
		return B_ERROR;

	doDirectory(&parent, NULL, 0);
	
	std::cout << "</DL><p>" << std::endl;
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
