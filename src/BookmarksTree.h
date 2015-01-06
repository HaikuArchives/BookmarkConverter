/*
 * BookmarksTree.h
 * Copyright (c) 2015 Markus Himmel. All rights reserved.
 * Distributed under the terms of the MIT license.
 */

#ifndef BOOKMARKS_TREE_H
#define BOOKMARKS_TREE_H

#include <vector>

class BookmarksEntry {
public:
	virtual bool IsBookmark() = 0;
	virtual bool IsFolder() = 0;
};

class BookmarksFolder : public BookmarksEntry, std::vector<BookmarksEntry*> {
public:
	BookmarksFolder();
	~BookmarksFolder();

	bool IsBookmark();
	bool IsFolder();

	const char* GetName();
	void SetName(const char* name);

private:
	BString fName;
};

class Bookmark : public BookmarksEntry {
public:
	Bookmark();
	~Bookmark();

	bool IsBookmark();
	bool IsFolder();

	const char* GetURL();
	void SetURL(const char* url);

	const char* GetTitle();
	void SetTitle(const char* title);

	const char* GetKeywords();
	void SetKeywords(const char* keywords);

private:
	BString fURL;
	BString fTitle;
	BString fKeywords;
};
#endif // BOOKMARKS_TREE_H
