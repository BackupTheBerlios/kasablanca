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
#include "transferitem.h"

transferitem::transferitem(QListView* taskview, QFileInfo filocal, RemoteFileInfo firemote, transfertype type)
 : QListViewItem(taskview, taskview->lastItem())
{
	m_type = type;
	m_filocal = filocal;
	m_firemote = firemote;

	if (type == transferitem::download) setText(0, "download " + firemote.fileName());
	if (type == transferitem::upload) setText(0, "upload " + firemote.fileName());
}

transferitem::transferitem(QListView* taskview, QListViewItem* after, QFileInfo filocal, RemoteFileInfo firemote, transfertype type)
 : QListViewItem(taskview, after)
{
	m_type = type;
	m_filocal = filocal;
	m_firemote = firemote;

	if (type == transferitem::download) setText(0, "download " + firemote.fileName());
	if (type == transferitem::upload) setText(0, "upload " + firemote.fileName());
}

transferitem::~transferitem()
{
}

int transferitem::type()
{
	return m_type;
}
