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

class KbItem;

/**
@author Magnus Kulke
*/
class KbFileInfo : public QFileInfo
{
public:

	//KbFileInfo();
	KbFileInfo(QFileInfo qfi);
	KbFileInfo(const KbFileInfo & kfi);
	KbFileInfo(QString workingdir);
	KbFileInfo(KbItem* item, QString workingdir);
	KbFileInfo(const QString & d, const QString & fileName, ulong size, QString date, uint date_int);
	~KbFileInfo();
	ulong Size() { return m_size; };
	QString Date() { return m_date; };
	uint DateInt() { return m_date_int; };
	void SetDirPath(QString path); 
	void SetSize(ulong size) { m_size = size; };
	static bool PrioritySort(const KbFileInfo &f1, const KbFileInfo &f2);
private:
   ulong m_size;
	uint m_date_int;
	QString m_date;
};

#endif
