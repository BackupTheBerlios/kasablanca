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
#ifndef TRANSFERDIR_H
#define TRANSFERDIR_H

#include <transferitem.h>

/**
@author Magnus Kulke
*/
class transferdir : public transferitem
{
public:
	transferdir(QListView* taskview, QFileInfo filocal, RemoteFileInfo firemote, transfertype type);
	transferdir(QListView* taskview, QListViewItem* after, QFileInfo filocal, RemoteFileInfo firemote, transfertype type);
	transferdir(QListView* taskview, QListViewItem* after, RemoteFileInfo fifxpsrc, RemoteFileInfo fifxpdst, transfertype type);
    ~transferdir();
	int rtti() const;

};

#endif
