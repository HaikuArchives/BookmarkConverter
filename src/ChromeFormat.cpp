/*
 * ChromeFormat.cpp
 * Copyright (c) 2015 Markus Himmel. All rights reserved.
 * Distributed under the terms of the MIT license.
 */

#include <iostream>
#include <fstream>

#include <String.h>

#include "BookmarksFormat.h"
#include "BookmarksTree.h"

void ChromeOutput::Output(BookmarksEntry* entry, const char* destination)
{
	if (fDestination != &std::cout)
		delete fDestination;

	if (destination != NULL) {
		fDestination = new std::ofstream(destination);
		if (!fDestination->good()) {
			delete fDestination;
			fDestination = &std::cout;
		}
	} else
		fDestination = &std::cout;


	*fDestination << "{" << std::endl << "   \"roots\": {" << std::endl
		<< "      \"bookmark_bar\": ";

	if (entry->IsFolder())
		static_cast<BookmarksFolder*>(entry)->SetName("WebPositive Export");

	HandleItem(entry, 3, true);

	*fDestination << std::endl << "   }," << std::endl << "   \"version\": 1"
		<< std::endl << "}" << std::endl;
}

void ChromeOutput::HandleItem(BookmarksEntry* entry, int indent, bool first)
{
	if (entry->IsFolder())
		OutputDirectory(*static_cast<BookmarksFolder*>(entry), indent, first);
	else if (entry->IsBookmark())
		OutputBookmark(*static_cast<Bookmark*>(entry), indent, first);
}

void ChromeOutput::OutputDirectory(BookmarksFolder& dir, int indent,
	bool first)
{
	BString ind;
	ind.Append(' ', indent * fIndentSize);
	
	if (!first)
		*fDestination << ", ";

	*fDestination << "{" << std::endl << ind << "\"children\": [ ";

	std::vector<BookmarksEntry*>::iterator it;
	for (it = dir.begin(); it != dir.end(); it++)
		HandleItem(*it, indent + 1, it == dir.begin());

	BString sInd;
	sInd.Append(' ', (indent - 1) * fIndentSize);

	*fDestination	<< " ]," << std::endl
					<< ind << "\"id\": \"" << fNextID << "\"," << std::endl
					<< ind << "\"name\": \"" << dir.GetName() << "\","
					<< std::endl
					<< ind << "\"type\": \"folder\"" << std::endl
					<< sInd << "}";

	fNextID++;
}

void ChromeOutput::OutputBookmark(Bookmark& bookmark, int indent, bool first)
{
	BString ind;
	ind.Append(' ', indent * fIndentSize);

	BString sInd;
	sInd.Append(' ', (indent - 1) * fIndentSize);

	if (!first)
		*fDestination << ", ";

	*fDestination	<< "{" << std::endl
					<< ind << "\"id\": \"" << fNextID << "\"," << std::endl
					<< ind << "\"name\": \"" << bookmark.GetTitle() << "\","
					<< std::endl
					<< ind << "\"type\": \"url\"," << std::endl
					<< ind << "\"url\": \"" << bookmark.GetURL() << "\""
					<< std::endl
					<< sInd << "}";

	fNextID++;
}

void ChromeOutput::SetIndentSize(int spaces)
{
	if (spaces >= 0)
		fIndentSize = spaces;
}

int ChromeOutput::GetIndentSize()
{
	return fIndentSize;
}

ChromeOutput::ChromeOutput()
	:
	fIndentSize(3),
	fNextID(1)
{
}

ChromeOutput::~ChromeOutput()
{
	delete fDestination;
}
