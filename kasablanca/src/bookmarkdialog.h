/***************************************************************************
 *   Copyright (C) 2004 by Magnus Kulke                                    *
 *   mkulke@magnusmachine                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef BOOKMARKDIALOG_H
#define BOOKMARKDIALOG_H

#include <Q_bookmarkdialog.h>
#include <kdialogbase.h>

#include "siteinfo.h"
#include "bookmarks.h"
#include <vector>

/**
@author Magnus Kulke
*/
class BookmarkDialog : public KDialogBase
{
Q_OBJECT
public:
    BookmarkDialog(QWidget *parent = 0, const char *name = 0);
    ~BookmarkDialog();
public slots:
    void SLOT_EntrySelected(int n);
    void SLOT_ApplyEntry();
    void SLOT_NewEntry();
    void SLOT_RemoveEntry();

private:
    int ApplyEntry(siteinfo * site);
    void RefreshEntry(siteinfo site);
    void EnableInput(bool b);
    KasablancaBookmarkDialog* ui_;
    vector<siteinfo> m_bookmarks;
    Bookmarks bookmarks;
};

#endif
