/***************************************************************************
 *   Copyright (C) 2004 by Magnus Kulke                                    *
 *   sikor_sxe@radicalapproach.de                                          *
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
#include "kbdirectory.h"

#include <list>

using namespace std;

kbdirectory::kbdirectory(QString dirname, QObject *parent, const char *name)
 : QObject(parent, name)
{
	m_name = dirname;
}

kbdirectory::~kbdirectory()
{
	qWarning("deleted: %s", m_name.latin1());
}

kbdirectory* kbdirectory::AddDirectory(QString name)
{
	kbdirectory* dir = new kbdirectory(name, this);
	m_dirlist.push_back(dir);
	return dir;
}

void kbdirectory::AddFile(QString name)
{
	m_filelist.push_back(name);
}

#include "kbdirectory.moc"
