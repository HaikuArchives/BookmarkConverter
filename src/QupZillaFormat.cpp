/*
 * QupZillaFormat.cpp
 * Copyright (c) 2015 Markus Himmel. All rights reserved.
 * Distributed under the terms of the MIT license.
 */

#include <fstream>
#include <iostream>

#include <Directory.h>
#include <Entry.h>
#include <File.h>
#include <FindDirectory.h>
#include <Json.h>
#include <Path.h>

#include "BookmarksTree.h"
#include "BookmarksFormat.h"

QupZillaInput::QupZillaInput()
{
}

QupZillaInput::~QupZillaInput()
{
}

BookmarksEntry* QupZillaInput::Input(const char* source)
{
	BFile file(source, B_READ_ONLY);
	if (file.InitCheck() != B_OK)
		return NULL;

	off_t size;
	if (file.GetSize(&size) != B_OK)
		return NULL;

	char* buffer = new (std::nothrow) char[size];
	if (buffer == NULL)
		return NULL;

	if (file.Read(buffer, size) != size) {
		delete[] buffer;
		return NULL;
	}

	BString json(buffer);
	delete[] buffer;
	BMessage decoded;

	if (BJson::Parse(json, decoded) != B_OK)
		return NULL;

	BMessage roots;
	if (decoded.FindMessage("roots", &roots) != B_OK)
		return NULL;

	BookmarksFolder* top = new BookmarksFolder(), * child = NULL;

	BMessage folder;
	if (roots.FindMessage("bookmark_menu", &folder) == B_OK) {
		child = readDirectory(folder);
		if (child != NULL)
			top->push_back(child);
	}

	if (roots.FindMessage("bookmark_menu", &folder) == B_OK) {
		child = readDirectory(folder);
		if (child != NULL)
			top->push_back(child);
	}

	if (roots.FindMessage("other", &folder) == B_OK) {
		child = readDirectory(folder);
		if (child != NULL)
			top->push_back(child);
	}

	return top;
}

BookmarksFolder* QupZillaInput::readDirectory(BMessage& info)
{
	BMessage children;
	if (info.FindMessage("children", &children) != B_OK)
		return NULL;

	BookmarksFolder* thisDir = new BookmarksFolder();

	int currentIndex = 0;
	BMessage currentChild;
	while (children.FindMessage((BString() << currentIndex).String(),
		&currentChild) == B_OK) {
		currentIndex++;
		const char* type;
		if (currentChild.FindString("type", &type) != B_OK)
			continue;

		BString typeS(type);
		if (typeS == "url") {
			Bookmark* child = readItem(currentChild);
			if (child != NULL)
				thisDir->push_back(child);
		} else if (typeS == "folder") {
			BookmarksFolder* child = readDirectory(currentChild);
			if (child != NULL)
				thisDir->push_back(child);
		} else
			continue;
	}

	const char* name;
	if (info.FindString("name", &name) == B_OK)
		thisDir->SetName(name);
	else
		thisDir->SetName("Unknown");

	return thisDir;
}

Bookmark* QupZillaInput::readItem(BMessage& info)
{
	Bookmark* result = new Bookmark();

	const char* tmp;
	if (info.FindString("name", &tmp) == B_OK)
		result->SetTitle(tmp);
	else
		result->SetTitle("Unknown");

	if (info.FindString("url", &tmp) == B_OK)
		result->SetURL(tmp);
	else
		result->SetURL("Unknown");

	if (info.FindString("keyword", &tmp) == B_OK)
		result->SetKeywords(tmp);

	return result;
}

const int kBaseOffset = 3;

void QupZillaOutput::Output(BookmarksEntry* entry, const char* destination)
{
	BPath dir;
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &dir) == B_OK) {
		BString path(dir.Path());
		path << "/Qt/.config/qupzilla/profiles/default/bookmarks.json";
		BEntry dest(destination, false);
		BEntry def(path, false);
		if (dest == def) {
			if (def.Exists()) {
				BDirectory parent;
				def.GetParent(&parent);
				int replaceIndex = 0;
				while (BEntry(&parent, (BString("bookmarks-old-")
					<< replaceIndex << ".json").String()).Exists())
					replaceIndex++;
				def.Rename(BString("bookmarks-old-") << replaceIndex
					<< ".json");
			}
		}
		else
			std::cerr << "Put the generated file into " << path.String()
				<< " for use with QupZilla." << std::endl;

	}

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


	*fDestination << "{" << std::endl << " \"roots\": {" << std::endl
		<< "  \"bookmark_bar\": ";

	if (entry->IsFolder())
		static_cast<BookmarksFolder*>(entry)->SetName("Import");

	HandleItem(entry, 0, true);

	*fDestination << std::endl << " }," << std::endl << " \"version\": 1"
		<< std::endl << "}" << std::endl;
}

void QupZillaOutput::HandleItem(BookmarksEntry* entry, int indent, bool first)
{
	if (entry->IsFolder())
		OutputDirectory(*static_cast<BookmarksFolder*>(entry), indent, first);
	else if (entry->IsBookmark())
		OutputBookmark(*static_cast<Bookmark*>(entry), indent, first);
}

void QupZillaOutput::OutputDirectory(BookmarksFolder& dir, int indent,
	bool first)
{
	BString ind;
	ind.Append(' ', indent * fIndentSize + kBaseOffset);
	
	if (!first)
		*fDestination << ", ";

	*fDestination << std::endl
		<< ind << "{" << std::endl << ind << " \"children\": [";

	std::vector<BookmarksEntry*>::iterator it;
	for (it = dir.begin(); it != dir.end(); it++)
		HandleItem(*it, indent + 1, it == dir.begin());

	if (dir.size() > 0)
		*fDestination << std::endl << ind << " ";
	*fDestination	<< "]," << std::endl
					<< ind << " \"name\": \"" << dir.GetName() << "\","
					<< std::endl
					<< ind << " \"type\": \"folder\"" << std::endl
					<< ind << "}";
}

void QupZillaOutput::OutputBookmark(Bookmark& bookmark, int indent, bool first)
{
	BString ind;
	ind.Append(' ', indent * fIndentSize + kBaseOffset);

	if (!first)
		*fDestination << ", ";

	*fDestination << std::endl << ind << "{" << std::endl;
	if (BString(bookmark.GetKeywords()) != "")
		*fDestination << ind << " \"keyword\": \"" << bookmark.GetKeywords()
			<< "\"," << std::endl;

	*fDestination	<< ind << " \"name\": \"" << bookmark.GetTitle() << "\","
					<< std::endl
					<< ind << " \"type\": \"url\"," << std::endl
					<< ind << " \"url\": \"" << bookmark.GetURL() << "\""
					<< std::endl
					<< ind << "}";
}

void QupZillaOutput::SetIndentSize(int spaces)
{
	if (spaces >= 0)
		fIndentSize = spaces;
}

int QupZillaOutput::GetIndentSize()
{
	return fIndentSize;
}

QupZillaOutput::QupZillaOutput()
	:
	fIndentSize(2)
{
}

QupZillaOutput::~QupZillaOutput()
{
	delete fDestination;
}
