/*
 * HTMLOutput.cpp
 * Copyright (c) 2015 Markus Himmel. All rights reserved.
 * Distributed under the terms of the MIT license.
 */

#include <fstream>
#include <iostream>

#include <String.h>

#include "BookmarksFormat.h"
#include "BookmarksTree.h"

void HTMLOutput::Output(BookmarksEntry* entry, const char* destination)
{
	delete fDestination;

	if (destination != NULL) {
		fDestination = new std::ofstream(destination);
		if (!fDestination->good()) {
			delete fDestination;
			fDestination = &std::cout;
		}
	} else
		fDestination = &std::cout;

	*fDestination	<< "<!DOCTYPE NETSCAPE-Bookmark-file-1>" << std::endl
				<< "<!-- This is an automatically generated file." << std::endl
				<< "     It will be read and overwritten." << std::endl
				<< "     DO NOT EDIT! -->" << std::endl
				<< "<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html;"
				<< " charset=UTF-8\">" << std::endl
				<< "<TITLE>Bookmarks</TITLE>" << std::endl
				<< "<H1>Bookmarks</H1>" << std::endl
				<< "<DL><p>" << std::endl;

	HandleItem(entry, 0);

	*fDestination << "</DL><p>" << std::endl;
}

void HTMLOutput::HandleItem(BookmarksEntry* entry, int indent)
{
	if (entry->IsFolder()) {
		OutputDirectory(*static_cast<BookmarksFolder*>(entry), indent);
	} else if (entry->IsBookmark()) {
		OutputBookmark(*static_cast<Bookmark*>(entry), indent);
	}
}

void HTMLOutput::OutputDirectory(BookmarksFolder& dir, int indent)
{
	BString ind;
	ind.Append(' ', indent * fIndentSize);

	if (indent > 0)
		*fDestination << ind << "<DT><H3>" << dir.GetName() << "</H3>" << std::endl
			<< ind << "<DL><p>" << std::endl;

	std::vector<BookmarksEntry*>::iterator it;
	for (it = dir.begin(); it != dir.end(); it++) {
		HandleItem(*it, indent + 1);
	}

	if (indent > 0)
		*fDestination << ind << "</DL><p>" << std::endl;
}

void HTMLOutput::OutputBookmark(Bookmark& bookmark, int indent)
{
	BString ind;
	ind.Append(' ', indent * fIndentSize);
	*fDestination << ind << "<DT><A HREF=\"" << bookmark.GetURL() << "\">"
		<< bookmark.GetTitle() << "</A>" << std::endl;
}

void HTMLOutput::SetIndentSize(int spaces)
{
	if (spaces >= 0)
		fIndentSize = spaces;
}

int HTMLOutput::GetIndentSize()
{
	return fIndentSize;
}

HTMLOutput::HTMLOutput()
	:
	fIndentSize(4),
	fDestination(NULL)
{
}

HTMLOutput::~HTMLOutput()
{
	delete fDestination;
}
