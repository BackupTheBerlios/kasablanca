//
// C++ Implementation: kbfileinfo
//
// Description: 
//
//
// Author: Magnus Kulke <mkulke@magnusmachine>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <qdir.h>
#include "kbitem.h"

#include "kbfileinfo.h"

KbFileInfo::KbFileInfo(kbitem* item, QString workingdir) : QFileInfo(QDir(workingdir), item->File())
{
	m_date_int = item->DateInt();
	m_date = item->Date();
	m_size = item->Size();
}

KbFileInfo::KbFileInfo(const QString & d, const QString & fileName, uint size, QString date, uint date_int)
 : QFileInfo(QDir(d), fileName)
{
	m_date_int = date_int;
	m_date = date;
	m_size = size;
}

KbFileInfo::~KbFileInfo()
{
}


