/*
 * BookmarksFormat.h
 * Copyright (c) 2015 Markus Himmel. All rights reserved.
 * Distributed under the terms of the MIT license.
 */

#ifndef BOOKMARKS_FORMAT_H
#define BOOKMARKS_FORMAT_H

class BookmarksFormat {
public:
	void Output(BooksmarksEntry* entry) = 0;
}

class HTMLFormat : public BookmarksFormat {
public:
	void Output(BookmarksEntry* entry);

	void SetIndentSize(int spaces);
	int GetIndentSize();

private:
	int fIndentSize;
}

/*
class ChromeFormat {
public:
	void Output(BookmarksEntry* entry);
}
*/

#endif // BOOKMARKS_FORMAT_H
