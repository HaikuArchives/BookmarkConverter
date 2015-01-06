/*
 * ChromeFormat.cpp
 * Copyright (c) 2015 Markus Himmel. All rights reserved.
 * Distributed under the terms of the MIT license.
 */

#include <iostream>

#include <String.h>

#include "BookmarksFormat.h"
#include "BookmarksTree.h"

void ChromeFormat::Output(BookmarksEntry* entry)
{
	std::cout << "{" << std::endl << "   \"roots\": {" << std::endl
		<< "      \"bookmark_bar\": ";

	if (entry->IsFolder())
		static_cast<BookmarksFolder*>(entry)->SetName("WebPositive Export");

	HandleItem(entry, 3, true);

	std::cout << std::endl << "   }," << std::endl << "   \"version\": 1"
		<< std::endl << "}" << std::endl;
}

void ChromeFormat::HandleItem(BookmarksEntry* entry, int indent, bool first)
{
	if (entry->IsFolder()) {
		OutputDirectory(*static_cast<BookmarksFolder*>(entry), indent, first);
	} else if (entry->IsBookmark()) {
		OutputBookmark(*static_cast<Bookmark*>(entry), indent, first);
	}
}

void ChromeFormat::OutputDirectory(BookmarksFolder& dir, int indent,
	bool first)
{
	BString ind;
	ind.Append(' ', indent * fIndentSize);
	
	if (!first)
		std::cout << ", ";

	std::cout << "{" << std::endl << ind << "\"children\": [ ";

	std::vector<BookmarksEntry*>::iterator it;
	for (it = dir.begin(); it != dir.end(); it++) {
		HandleItem(*it, indent + 1, it == dir.begin());
	}

	BString sInd;
	sInd.Append(' ', (indent - 1) * fIndentSize);

	std::cout	<< " ]," << std::endl
				<< ind << "\"id\": \"" << fNextID << "\"," << std::endl
				<< ind << "\"name\": \"" << dir.GetName() << "\","
					<< std::endl
				<< ind << "\"type\": \"folder\"" << std::endl
				<< sInd << "}";

	fNextID++;
}

void ChromeFormat::OutputBookmark(Bookmark& bookmark, int indent, bool first)
{
	BString ind;
	ind.Append(' ', indent * fIndentSize);

	BString sInd;
	sInd.Append(' ', (indent - 1) * fIndentSize);

	if (!first)
		std::cout << ", ";

	std::cout	<< "{" << std::endl
				<< ind << "\"id\": \"" << fNextID << "\"," << std::endl
				<< ind << "\"name\": \"" << bookmark.GetTitle() << "\","
					<< std::endl
				<< ind << "\"type\": \"url\"," << std::endl
				<< ind << "\"url\": \"" << bookmark.GetURL() << "\""
					<< std::endl
				<< sInd << "}";

	fNextID++;
}

void ChromeFormat::SetIndentSize(int spaces)
{
	if (spaces >= 0)
		fIndentSize = spaces;
}

int ChromeFormat::GetIndentSize()
{
	return fIndentSize;
}

ChromeFormat::ChromeFormat()
	:
	fIndentSize(3),
	fNextID(1)
{
}

ChromeFormat::~ChromeFormat()
{
}
