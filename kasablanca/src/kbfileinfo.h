//
// C++ Interface: kbfileinfo
//
// Description: 
//
//
// Author: Magnus Kulke <mkulke@magnusmachine>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KBFILEINFO_H
#define KBFILEINFO_H

#include <qfileinfo.h>

class kbitem;

/**
@author Magnus Kulke
*/
class KbFileInfo : public QFileInfo
{
public:

	KbFileInfo(kbitem* item, QString workingdir);
	KbFileInfo(const QString & d, const QString & fileName, uint size, QString date, uint date_int);
	~KbFileInfo();
	uint Size() { return m_size; };
	QString Date() { return m_date; };
	uint DateInt() { return m_date_int; };
private:
   uint m_size;
	uint m_date_int;
	QString m_date;
};

#endif
