/***************************************************************************
                          remotefileinfo.h  -  description
                             -------------------
    begin                : Mi Dez 3 2003
    copyright            : (C) 2003 by mkulke
    email                : sikor_sxe@radicalapproach.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef REMOTEFILEINFO_H
#define REMOTEFILEINFO_H

#include <qfileinfo.h>

/**
  *@author mkulke
  */

class RemoteFileInfo : public QFileInfo  {
public:
    RemoteFileInfo(); 
    RemoteFileInfo(const QString & d, const QString & fileName, uint size, QString date, uint date_int);
    ~RemoteFileInfo();
    uint size();
    QString date();
	uint date_int();
private:
   uint m_size;
	uint m_date_int;
	QString m_date;
};

#endif
