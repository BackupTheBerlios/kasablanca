//
// C++ Interface: kbitem
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

class RemoteFileInfo;

/**
@author mkulke
*/
class kbitem : public QListViewItem
{
public:
	kbitem(RemoteFileInfo *rfi, QListView* parent, QListViewItem* after);
	kbitem(QListView* parent, QListViewItem* after);
	int compare (QListViewItem * i, int col, bool ascending) const;
	~kbitem();
	QString m_path;
	QString m_date;
	uint m_size;
	uint m_date_int;
	QString m_file;

};

#endif
