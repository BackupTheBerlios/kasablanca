//
// C++ Interface: bookmarks
//
// Description: 
//
//
// Author: Big Biff <bigbiff@chunkyfilms.org>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BOOKMARKS_H
#define BOOKMARKS_H

#include "siteinfo.h"
#include <vector>

/**
@author Big Biff
*/
class Bookmarks{
public:
    Bookmarks();
    ~Bookmarks();
    vector<siteinfo> getBookmarks();
    int setBookmarks(vector<siteinfo> bmarks);
private:
    vector<siteinfo> m_bookmarks;
    int ParseBookmarks();
    int WriteBookmarks();
};

#endif
