/***************************************************************************
                          site.h  -  description
                             -------------------
    begin                : Die Aug 20 2002
    copyright            : (C) 2002 by Magnus Kulke
    email                : sikor_sxe at radicalapproach dot de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SITE_H
#define SITE_H

using namespace std;

#include <string> // STL!

/**
  *@author Magnus Kulke
  */

class siteinfo {
public:
	siteinfo();
	~siteinfo();
	const char *GetInfo();
	const char *GetUser();
	const char *GetPass();
	const char *GetDir();
	const char *GetName();
	const char *GetDefaultDirectory() { return m_defaultdirectory.c_str(); };
	int GetAlternativeFxp() { return m_alternativefxp; };
	int GetPasv();
	int GetTls();
	void SetPasv(int pasv);
	void SetTls(int tls);
	void SetDefaultDirectory(const char * path) { m_defaultdirectory = path; };
	void SetAlternativeFxp(int i) { m_alternativefxp = i; };
	void SetUser(const char *user);
	void SetInfo(const char *info);
	void SetPass(const char *pass);
	void SetDir(const char *dir);
	void SetName(const char *name);
	int CheckContent();
	void Clear();
        bool isGroup();
        void setIsGroup(bool g);
private:
	char *mp_info;
	char *mp_dir;
	char *mp_user;
	char *mp_pass;
	string m_user;
	string m_info;
	string m_name;
	string m_dir;
	string m_pass;
	string m_defaultdirectory;
	int m_alternativefxp;
	int m_pasv;
	int m_tls;
        bool b_isGroup;
};

#endif

