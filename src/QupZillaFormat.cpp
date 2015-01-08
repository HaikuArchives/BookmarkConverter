/*
 * QupZillaFormat.cpp
 * Copyright (c) 2015 Markus Himmel. All rights reserved.
 * Distributed under the terms of the MIT license.
 */

#include <Json.h>
#include <File.h>
#include <Path.h>
#include <FindDirectory.h>

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
	BString path(source);
	if (path == "") {
		BPath dir;
		if (find_directory(B_USER_SETTINGS_DIRECTORY, &dir) == B_OK) {
			path = dir.Path();
			path << "/Qt/.config/qupzilla/profiles/default/bookmarks.json";
		}
	}

	BFile file(path.String(), B_READ_ONLY);
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

	if (BJson::Parse(decoded, json) != B_OK)
		return NULL;

	BMessage roots;
	if (decoded.FindMessage("roots", &roots) != B_OK)
		return NULL;

	BookmarksFolder* top = new BookmarksFolder(), * child = NULL;

	BMessage folder;
	if (roots.FindMessage("bookmark_bar", &folder) == B_OK) {
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

