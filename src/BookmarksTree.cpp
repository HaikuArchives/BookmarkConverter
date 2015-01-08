/*
 * BookmarksTree.cpp
 * Copyright (c) 2015 Markus Himmel. All rights reserved.
 * Distributed under the terms of the MIT license.
 */

#include <String.h>
#include "BookmarksTree.h"

BookmarksEntry::~BookmarksEntry()
{
}

BookmarksFolder::BookmarksFolder()
	:
	std::vector<BookmarksEntry*>(),
	fName()
{
}

BookmarksFolder::~BookmarksFolder()
{
	std::vector<BookmarksEntry*>::iterator it;

	for (it = this->begin(); it != this->end(); it++)
		delete (*it);
}

bool BookmarksFolder::IsBookmark()
{
	return false;
}

bool BookmarksFolder::IsFolder()
{
	return true;
}

const char* BookmarksFolder::GetName()
{
	return fName.String();
}

void BookmarksFolder::SetName(const char* name)
{
	fName = name;
}

Bookmark::Bookmark()
	:
	fURL(),
	fTitle(),
	fKeywords()
{
}

Bookmark::~Bookmark()
{
}

bool Bookmark::IsBookmark()
{
	return true;
}

bool Bookmark::IsFolder()
{
	return false;
}

const char* Bookmark::GetURL()
{
	return fURL.String();
}

void Bookmark::SetURL(const char* url)
{
	fURL = url;
}

const char* Bookmark::GetTitle()
{
	return fTitle.String();
}

void Bookmark::SetTitle(const char* title)
{
	fTitle = title;
}

const char* Bookmark::GetKeywords()
{
	return fKeywords.String();
}

void Bookmark::SetKeywords(const char* keywords)
{
	fKeywords = keywords;
}
