//
// C++ Interface: KbItem
//
// Description: 
//
//
// Author: mkulke <sikor_sxe@radicalapproach.de>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KBITEM_H
#define KBITEM_H

#include <qlistview.h>

class KbFileInfo;

/**
@author mkulke
*/
class KbItem : public QListViewItem
{
public:
	enum entrytype
	{
		dir = 1001,
		file
	};
	KbItem(KbFileInfo kfi, QListView* parent, QListViewItem* after);
	KbItem(QListView* parent, QListViewItem* after);
	int compare (QListViewItem * i, int col, bool ascending) const;
	~KbItem();
	
	QString File() { return m_file; };
	QString Path() { return m_path; };
	QString Date() { return m_date; };
	uint DateInt() { return m_date_int; };
	ulong Size() { return m_size; };
		
protected:
	QString m_path;
	QString m_date;
	ulong m_size;
	uint m_date_int;
	QString m_file;

};

#endif
