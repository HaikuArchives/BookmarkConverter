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

class BookmarksFormat {
public:
	virtual void Output(BookmarksEntry* entry) = 0;
};

class HTMLFormat : public BookmarksFormat {
public:
	HTMLFormat();
	~HTMLFormat();

	void Output(BookmarksEntry* entry);

	void SetIndentSize(int spaces);
	int GetIndentSize();

private:
	int fIndentSize;

	void HandleItem(BookmarksEntry* entry, int indent);
	void OutputDirectory(BookmarksFolder& dir, int indent);
	void OutputBookmark(Bookmark& bookmark, int indent);
};


class ChromeFormat : public BookmarksFormat {
public:
	ChromeFormat();
	~ChromeFormat();

	void Output(BookmarksEntry* entry);

	void SetIndentSize(int spaces);
	int GetIndentSize();

private:
	int fNextID;
	int fIndentSize;

	void HandleItem(BookmarksEntry* entry, int indent, bool first);
	void OutputDirectory(BookmarksFolder& dir, int indent, bool first);
	void OutputBookmark(Bookmark& bookmark, int indent, bool first);
};


#endif // BOOKMARKS_FORMAT_H
