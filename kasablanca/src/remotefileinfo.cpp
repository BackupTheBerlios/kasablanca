/***************************************************************************
                          remotefileinfo.cpp  -  description
                             -------------------
    begin                : Mi Dez 3 2003
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

#include "remotefileinfo.h"
#include "qdir.h"

RemoteFileInfo::RemoteFileInfo()
{
}

RemoteFileInfo::RemoteFileInfo(const QString & d, const QString & fileName, uint size, QString date, uint date_int) : QFileInfo(QDir(d), fileName)
{
	m_date_int = date_int;
    m_date = date;
    m_size = size;
}
RemoteFileInfo::~RemoteFileInfo(){
}

uint RemoteFileInfo::size()
{
    return m_size;
}

QString RemoteFileInfo::date()
{
    return m_date;
}

uint RemoteFileInfo::date_int()
{
	return m_date_int;
}
