//
// C++ Implementation: kbdirinfo
//
// Description: 
//
//
// Author: Magnus Kulke <mkulke@magnusmachine>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <list>

#include "kbdirinfo.h"

using namespace std;

KbDirInfo::KbDirInfo(KbFileInfo info) : KbFileInfo(info)
{
}

KbDirInfo::KbDirInfo(QString path) : KbFileInfo(path)
{
}

KbDirInfo::~KbDirInfo()
{
}

KbDirInfo* KbDirInfo::AddDirectory(KbFileInfo info)
{
	if ((info.fileName() == ".") || (info.fileName() == "..")) return NULL;
	KbDirInfo* dir = new KbDirInfo(info);
	m_dirlist.push_back(dir);
	return dir;
}

void KbDirInfo::AddFile(KbFileInfo file)
{
	m_filelist.push_back(file);
}
