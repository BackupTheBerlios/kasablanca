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
#ifndef KBDIRECTORY_H
#define KBDIRECTORY_H

#include <qobject.h>

#include <list>

using namespace std;

/**
this class holds the recursive content of a directory

@author Magnus Kulke
*/
class kbdirectory : public QObject
{
Q_OBJECT
public:
	kbdirectory(QString dirname, QObject *parent = 0, const char *name = 0);
	~kbdirectory();
	kbdirectory* AddDirectory(QString name);
	void AddFile(QString);
	QString Name() {return m_name;}
private:
	QString m_name;
	list<QString> m_filelist;
	list<kbdirectory*> m_dirlist;
public:
	list<kbdirectory*>* Dirlist() {return &m_dirlist;}
	list<QString>* Filelist() {return &m_filelist;}
};

#endif
