/***************************************************************************
                          fileitem.cpp  -  description
                             -------------------
    begin                : Die Sep 23 2003
    copyright            : (C) 2003 by mkulke
    email                : sikor_sxe@radicalapproach.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "fileitem.h"

fileitem::fileitem(QListView * parent, QListViewItem * after, QString file, QString path, QString date, uint size, uint date_int) : kbitem(parent, after)
{
    setText(0,file);

    QString s;
    s.setNum(size);
    setText(1, s);

    setText(2, date);

    m_path = path;
    m_file = file;
    m_date = date;
    m_size = size;

	m_date_int = date_int; 
}

fileitem::~fileitem()
{
}

int fileitem::rtti() const
{
    return 1002;
}
