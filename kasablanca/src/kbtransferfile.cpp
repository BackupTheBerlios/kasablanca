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
 
#include "kbtransferfile.h"

KbTransferFile::KbTransferFile(QListView *taskview, QListViewItem *after, FtpSession *srcsession, FtpSession *dstsession, KbFileInfo *src, KbFileInfo *dst) : KbTransferItem(taskview, after, srcsession, dstsession, src, dst)
{
	setPixmap(0, KGlobal::iconLoader()->loadIcon("files",KIcon::Small));
	m_time_old = -1;
	m_xfered_old = 0;
}

KbTransferFile::KbTransferFile(QListViewItem *root, QListViewItem *after, FtpSession *srcsession, FtpSession *dstsession, KbFileInfo *src, KbFileInfo *dst) : KbTransferItem(root, after, srcsession, dstsession, src, dst)
{
	setPixmap(0, KGlobal::iconLoader()->loadIcon("files",KIcon::Small));
	m_time_old = -1;
	m_xfered_old = 0;
}

KbTransferFile::~KbTransferFile()
{
}

int KbTransferFile::rtti() const
{
    return KbTransferItem::file;
}

void KbTransferFile::Info()
{
	qWarning("INFO: transfer file from %s to %s", mp_srcsession->name(), mp_dstsession->name());
	qWarning("INFO: mp_src->fileName() = %s", mp_src->fileName().latin1());
	qWarning("INFO: mp_dst->fileName() = %s", mp_dst->fileName().latin1());
	qWarning("INFO: mp_src->dirPath() = %s", mp_src->dirPath(true).latin1());
	qWarning("INFO: mp_dst->dirPath() = %s", mp_dst->dirPath(true).latin1());
}

void KbTransferFile::ShowProgress()
{
	int time = m_time.elapsed();
	int time_dif = time - m_time_old;
	if (time_dif == 0) time_dif = 1;
	ulong xfer_dif = m_xfered - m_xfered_old;

	setText(1, 
		"[" +
		QString::number((m_xfered + mp_dst->Size()) >> 10) +
		"kb of " +
		QString::number(mp_src->Size() >> 10) +
		"kb] [" +
		QString::number(((m_xfered + mp_dst->Size())* 100 ) / (mp_src->Size() + 1)) + 
		"%] [" +
		QString::number(xfer_dif / time_dif) +
		" kb/s]"
	);
	
	m_time_old = time;
	m_xfered_old = m_xfered;
}


