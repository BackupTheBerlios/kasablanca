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

#include <kiconloader.h>
#include <kglobal.h>
#include <qlistview.h>
#include <qfileinfo.h>

#include "ftpsession.h"
#include "kbfileinfo.h"
#include "remotefileinfo.h"

class FtpSession;

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
		  download_a_to_b = 1001,
		  download_b_to_a,
		  upload_a_to_b,
		  upload_b_to_a,
		  fxp_a_to_b,
		  fxp_b_to_a
    };
	 
	transferitem(QListView *taskview, QListViewItem *after, FtpSession *srcsession, FtpSession *dstsession, KbFileInfo* src, KbFileInfo* dst);
	//transferitem(QListView* taskview, QFileInfo filocal, RemoteFileInfo firemote, transfertype type);
	transferitem(QListView* taskview, QListViewItem* after, QFileInfo filocal, RemoteFileInfo firemote, transfertype type);
	transferitem(QListView* taskview, QListViewItem* after, RemoteFileInfo fifxpsrc, RemoteFileInfo fifxpdst, transfertype type);
   ~transferitem();

	QFileInfo m_filocal;
	RemoteFileInfo m_firemote, m_fifxpsrc, m_fifxpdst;
	int type();
	virtual void Transfer();
private:
	transfertype m_type;
protected:
	FtpSession *mp_srcsession, *mp_dstsession;
	KbFileInfo *mp_src, *mp_dst;
};

#endif
