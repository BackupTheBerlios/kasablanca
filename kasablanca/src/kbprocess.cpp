//
// C++ Implementation: kbprocess
//
// Description: 
//
//
// Author: Magnus Kulke <mkulke@magnusmachine>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "kbprocess.h"

kbprocess::kbprocess(QObject *parent, const char *name)
 : QProcess(parent, name)
{
	connect(this, SIGNAL(readyReadStdout()), this, SLOT(SLOT_ReadReadyStdout()));
}

kbprocess::~kbprocess()
{
	tryTerminate();
	kill();
}

bool kbprocess::writeStdin(QString s)
{
	if (!isRunning()) return false;
	writeToStdin(s + '\n');
	return true;
}

void kbprocess::SLOT_ReadReadyStdout()
{
	emit readyToRead(this);
}
#include "kbprocess.moc"
