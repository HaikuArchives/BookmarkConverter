/*
 * BookmarksFormat.h
 * Copyright (c) 2015 Markus Himmel. All rights reserved.
 * Distributed under the terms of the MIT license.
 */

#ifndef BOOKMARKS_FORMAT_H
#define BOOKMARKS_FORMAT_H

#include <iostream>

class BFile;
class BDirectory;
class BookmarksEntry;
class BookmarksFolder;
class Bookmark;

class BookmarksOutput {
public:
	virtual void Output(BookmarksEntry* entry, const char* destination) = 0;
};

class BookmarksInput {
public:
	virtual BookmarksEntry* Input(const char* source) = 0;
};

class HTMLOutput : public BookmarksOutput {
public:
	HTMLOutput();
	~HTMLOutput();

	void Output(BookmarksEntry* entry, const char* destination);

	void SetIndentSize(int spaces);
	int GetIndentSize();

private:
	std::ostream* fDestination;
	int fIndentSize;

	void HandleItem(BookmarksEntry* entry, int indent);
	void OutputDirectory(BookmarksFolder& dir, int indent);
	void OutputBookmark(Bookmark& bookmark, int indent);
};


class ChromeOutput : public BookmarksOutput {
public:
	ChromeOutput();
	~ChromeOutput();

	void Output(BookmarksEntry* entry, const char* destination);

	void SetIndentSize(int spaces);
	int GetIndentSize();

private:
	std::ostream* fDestination;
	int fNextID;
	int fIndentSize;

	void HandleItem(BookmarksEntry* entry, int indent, bool first);
	void OutputDirectory(BookmarksFolder& dir, int indent, bool first);
	void OutputBookmark(Bookmark& bookmark, int indent, bool first);
};

class BeOutput : public BookmarksOutput {
public:
	BeOutput();
	~BeOutput();

	void Output(BookmarksEntry* entry, const char* destination);

private:
	void HandleItem(BookmarksEntry* entry, bool first, BDirectory& dest);
	void OutputDirectory(BookmarksFolder& entry, bool first, BDirectory& dest);
	void OutputBookmark(Bookmark& entry, BDirectory& dest);
};

class BeInput : public BookmarksInput {
public:
	BeInput();
	~BeInput();

	BookmarksEntry* Input(const char* source);

private:
	Bookmark* readFile(BFile& file);
	BookmarksFolder* readDirectory(BDirectory& dir, const char* name);
};

/*
class QupZillaInput : public BookmarksInput {
public:
	QupZillaInput();
	~QupZillaInput();

	BookmarksEntry* Input(const char* source);
};
*/

#endif // BOOKMARKS_FORMAT_H
