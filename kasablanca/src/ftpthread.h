/***************************************************************************
 *   Copyright (C) 2004 by Magnus Kulke                                    *
 *   mkulke@magnusmachine                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef FTPTHREAD_H
#define FTPTHREAD_H

#include <list>
#include <qthread.h>
#include "eventhandler.h"
#include "remotefileinfo.h"

using namespace std;

class ftplib;
class QObject;
class kbdirectory;

typedef list<RemoteFileInfo> rfilist;
typedef pair<int, bool> xferpair;

/**
@author Magnus Kulke
*/
class FtpThread : public QThread 
{
public:
   FtpThread();
   ~FtpThread();
	static void CallbackLog(char *log, void *arg, bool out);
	static int CallbackXfer(int xfered, void *arg);
	void SetEventReceiver(QObject* eventreceiver);
	void ClearQueue();
	bool Connect(QString host);
	bool Login(QString user, QString pass);
	bool Quit();
	bool Pwd();
	bool Chdir(QString path);
	bool Cdup();
	bool Dir();
	bool Scandir(kbdirectory* dir);
	bool Rm(QString name);
	bool Rmdir(QString name);
	bool Authtls();
	bool Pasv(bool flag);
	bool EncryptData(bool flag);
	bool Get(QString src, QString dst, unsigned long resume = 0);
	bool Put(QString src, QString dst, unsigned long resume = 0);
	bool Mkdir(QString path);
	bool Rename(QString src, QString dst);
	bool Raw(QString cmd);
private:
	void run();
	void Event(EventHandler::EventType type, void *data = NULL);
	bool FormatFilelist(const char *filename,
		QString current,
		list<RemoteFileInfo> *filetable,
		list<RemoteFileInfo> *dirtable
	);
	void InitInternals();
	void Connect_thread(); 
	void Login_thread(); 
	void Pwd_thread();
	void Quit_thread();
	void Chdir_thread();
	void Cdup_thread();
	void Dir_thread();
	void Scandir_thread();
	void Delete_thread();
	void Rm_thread();
	void Rmdir_thread();
	void Authtls_thread();
	void Dataencoff_thread();
	void Dataencon_thread();
	void Get_thread();
	void Put_thread();
	void Mkdir_thread();
	void Rename_thread();
	void Raw_thread();
	bool Scandir_recurse(kbdirectory* dir);
	bool Delete_recurse(QString name);
	bool ConnectionLost();
private:
	enum task
	{
		connect = 0,
		negotiateencryption,
		login,
		quit,
		pwd,
		chdir,
		cdup,
		dir,
		scandir,
		rm,
		rmdir,
		authtls,
		dataencoff,
		dataencon,
		get,
		mkdir,
		rename,
		raw,
		put
	};
	QMutex* mp_mutex;
	QObject* mp_eventreceiver;
	ftplib* mp_ftp;
	QString m_host;
	QString m_user;
	QString m_pass;
	QString m_pwd;
	bool m_dataencrypted;
	kbdirectory* m_scandir;
	rfilist m_dirlist, m_filelist;
	pair<rfilist, rfilist> m_dircontent;
	list<QString> m_chdirlist;
	list<QString> m_rmlist;
	list<QString> m_rmdirlist;
	list<QString> m_getsrclist;
	list<QString> m_getdstlist;
	list<QString> m_putsrclist;
	list<QString> m_putdstlist;
	list<unsigned long> m_putresumelist;
	list<QString> m_mkdirlist;
	list<unsigned long> m_getresumelist;
	list<QString> m_renamesrclist;
	list<QString> m_renamedstlist;
	list<QString> m_rawlist;
	QValueList<task> m_tasklist;
public:
	QString m_linebuffer;
};

#endif
