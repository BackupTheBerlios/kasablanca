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

KbFileInfo::KbFileInfo() : QFileInfo()
{

}

KbFileInfo::KbFileInfo(QString workingdir) : QFileInfo(QDir(workingdir), ".")
{

}

KbFileInfo::KbFileInfo(QFileInfo qfi) : QFileInfo(qfi)
{
	m_date_int = 
		qfi.lastModified().date().year() * 10000 +
		qfi.lastModified().date().month() * 100 +
		qfi.lastModified().date().day();
	m_date = qfi.lastModified().toString("MMM dd yyyy");
	m_size = qfi.size();
}

KbFileInfo::KbFileInfo(const KbFileInfo & kfi) : QFileInfo(kfi)
{
	m_date_int = kfi.m_date_int;
	m_date = kfi.m_date;
	m_size = kfi.m_size;
}

KbFileInfo::KbFileInfo(KbItem* item, QString workingdir) : QFileInfo(QDir(workingdir), item->File())
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

void KbFileInfo::SetDirPath(QString path)
{
	setFile(QDir(path), fileName());
} 

