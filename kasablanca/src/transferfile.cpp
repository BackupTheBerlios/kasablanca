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

#include "transferfile.h"

/*transferfile::transferfile(QListView* taskview, QFileInfo filocal, RemoteFileInfo firemote, transfertype type)
 : transferitem(taskview, filocal, firemote, type)
{
}*/

transferfile::transferfile(QListView *taskview, QListViewItem *after, FtpSession *srcsession, FtpSession *dstsession, KbFileInfo *src, KbFileInfo *dst) : transferitem(taskview, after, srcsession, dstsession, src, dst)
{
	setPixmap(0, KGlobal::iconLoader()->loadIcon("files",KIcon::Small));
}

transferfile::transferfile(QListView* taskview, QListViewItem* after, QFileInfo filocal, RemoteFileInfo firemote, transfertype type) : transferitem(taskview, after, filocal, firemote, type)
{
}

transferfile::transferfile(QListView* taskview, QListViewItem* after, RemoteFileInfo fifxpsrc, RemoteFileInfo fifxpdst, transfertype type) : transferitem(taskview, after, fifxpsrc, fifxpdst, type)
{
}

transferfile::~transferfile()
{
}

int transferfile::rtti() const
{
    return transferitem::file;
}

void transferfile::Transfer()
{
	qWarning("INFO: transfer file %s from %s to %s", mp_src->fileName().latin1(), mp_srcsession->name(), mp_dstsession->name());
}


