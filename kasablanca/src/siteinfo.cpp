/***************************************************************************
                          siteinfo.cpp  -  description
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

#include <stdio.h>
#include <string> // STL!
#include "string.h"

#include "siteinfo.h"

siteinfo::siteinfo() 
{
    mp_dir = NULL;
    mp_info = NULL;
}
siteinfo::~siteinfo()
{
}

void siteinfo::SetInfo(const char *info)
{
    m_info.assign(info);
    //mp_info = info;
}

void siteinfo::SetPass(const char *pass)
{
    m_pass.assign(pass);
    //mp_pass = pass;
}

void siteinfo::SetUser(const char *user)
{
    //mp_user = user;
    m_user.assign(user);
}

void siteinfo::SetDir(const char *dir)
{
    m_dir.assign(dir);
    if (m_dir.rfind('/') != (m_dir.length() - 1)) m_dir.append("/");
}

void siteinfo::SetName(const char *name)
{
    m_name.assign(name);
    //m_name.assign(name);
}

void siteinfo::SetPasv(int pasv)
{
    m_pasv = pasv;
}

void siteinfo::SetTls(int tls)
{
    m_tls = tls;
}

//void siteinfo::SetCommand(const char *command)
//{
//    m_command.assign(command);
//    //mp_command = command;
//}

const char *siteinfo::GetInfo()
{
    return m_info.c_str();
    //return mp_info;
}

const char *siteinfo::GetUser()
{
    return m_user.c_str();
    //return mp_user;
}

const char *siteinfo::GetPass()
{
    return m_pass.c_str();
}

const char *siteinfo::GetName()
{
    return m_name.c_str();
}

const char *siteinfo::GetDir()
{
    return m_dir.c_str();
}

//const char *siteinfo::GetCommand()
//{
//    return m_command.c_str();
//}

int siteinfo::GetPasv()
{
    return m_pasv;
}

int siteinfo::GetTls()
{
    return m_tls;
}

int siteinfo::CheckContent()
{
    return (!m_pass.empty())
        && (!m_name.empty())
		&& (!m_info.empty())
		&& (!m_user.empty());
}

void siteinfo::Clear()
{
	m_name = "";
	m_pass = "";
	m_user = "";
	m_info = "";
	m_dir = "";
	m_pasv = 0;
	m_tls = 0;
}

void siteinfo::setIsGroup(bool g)
{
}

bool siteinfo::isGroup()
{
    return(false);
}
