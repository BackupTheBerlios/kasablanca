//
// C++ Interface: kbprocess
//
// Description: 
//
//
// Author: Magnus Kulke <mkulke@magnusmachine>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KBPROCESS_H
#define KBPROCESS_H

#include <qstring.h>
#include <qprocess.h>

/**
@author Magnus Kulke
*/
class kbprocess : public QProcess
{
Q_OBJECT
public:
   kbprocess(QObject *parent = 0, const char *name = 0);
   ~kbprocess();
	bool writeStdin(QString s);
};

#endif
