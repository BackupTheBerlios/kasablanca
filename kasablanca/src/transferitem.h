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
#ifndef TRANSFERITEM_H
#define TRANSFERITEM_H

#include <qlistview.h>
#include <qfileinfo.h>
#include "remotefileinfo.h"

/**
@author Magnus Kulke
*/
class transferitem : public QListViewItem
{
public:
	enum type
    {
        file = 1001,
        dir
    };
	enum transfertype
    {
        download = 1001,
        upload,
        fxp,
		  remove
    };
	transferitem(QListView* taskview, QFileInfo filocal, RemoteFileInfo firemote, transfertype type);
	transferitem(QListView* taskview, QListViewItem* after, QFileInfo filocal, RemoteFileInfo firemote, transfertype type);
   ~transferitem();

	QFileInfo m_filocal;
	RemoteFileInfo m_firemote;
	int type();
private:
	transfertype m_type;
};

#endif
