/*
 * BookmarksFormat.h
 * Copyright (c) 2015 Markus Himmel. All rights reserved.
 * Distributed under the terms of the MIT license.
 */

#ifndef BOOKMARKS_FORMAT_H
#define BOOKMARKS_FORMAT_H

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
	HTMLFormat();
	~HTMLFormat();

	void Output(BookmarksEntry* entry, const char* destination);

	void SetIndentSize(int spaces);
	int GetIndentSize();

private:
	int fIndentSize;

	void HandleItem(BookmarksEntry* entry, int indent);
	void OutputDirectory(BookmarksFolder& dir, int indent);
	void OutputBookmark(Bookmark& bookmark, int indent);
};


class ChromeOutput : public BookmarksOutput {
public:
	ChromeFormat();
	~ChromeFormat();

	void Output(BookmarksEntry* entry, const char* destination);

	void SetIndentSize(int spaces);
	int GetIndentSize();

private:
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
};

class BeInput : public BookmarksInput {
public:
	BeInput();
	~BeInput();

	BookmarksEntry* Input(const char* source);
};

class QupZillaInput : public BookmarksInput {
public:
	QupZillaInput();
	~QupZillaInput();

	BookmarksEntry* Input(const char* source);
};

class WebPositiveInput : public BookmarksInput {
public:
	WebPositiveInput();
	~QupZillaInput();

	BookmarksEntry* Input(const char* source);
};


#endif // BOOKMARKS_FORMAT_H
