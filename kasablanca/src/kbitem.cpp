//
// C++ Implementation: kbitem
//
// Description: 
//
//
// Author: mkulke <sikor_sxe@radicalapproach.de>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <qdatetime.h>
#include "remotefileinfo.h"


#include "kbitem.h"


kbitem::kbitem(RemoteFileInfo *rfi, QListView* parent, QListViewItem* after) : QListViewItem(parent, after)
{
	m_file = rfi->fileName();
	m_path = rfi->dirPath(), 
	m_date =	rfi->date(), 
	m_size =	rfi->size(),
	m_date_int = rfi->date_int();

	setText(0, m_file);
	setText(1, QString::number(m_size));
	setText(2, m_date);
}

kbitem::kbitem(QListView* parent, QListViewItem* after) : QListViewItem(parent, after)
{
}

kbitem::~kbitem()
{
}

int kbitem::compare(QListViewItem * i, int col, bool ascending) const
{
	if ((this->rtti() == 1001) and (i->rtti() == 1002)) 
	{
		if (ascending) return -1;
		else return 1;
	}
	else if ((this->rtti() == 1002) and (i->rtti() == 1001)) 
	{
		if (ascending) return 1;
		else return -1; 
	}
		
	if (col == 1)
	{
		int x = this->text(1).toInt();
		int y = i->text(1).toInt();
		if (x == y) return 0;
		if (x < y) return -1;
		if (x > y) return 1;
	}
	if (col == 2)
	{
		uint x = this->m_date_int;
		uint y = static_cast<kbitem*>(i)->m_date_int;
		if (x == y) return 0;
		if (x < y) return -1;
		if (x > y) return 1;
	}
	return QListViewItem::compare(i, col, ascending);
}

