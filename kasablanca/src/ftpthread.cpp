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
#include "ftpthread.h"

#include <list>
#include "ftplib.h"
#include "kbdirectory.h"
#include <qapplication.h>
#include <qevent.h>
#include "eventhandler.h"
#include "remotefileinfo.h"
#include <qdir.h>
#include <iostream>

using namespace std;

/* 
class description:

the class accepts ftp operations by public methods. when such a method is called,
a certain type value is appended to a list. in the threads main loop (triggered when 
start() is called) the list is processed by calling the ftplibpp functions. depending
on the result of an operation a certain event is posted to the eventreceiver. when
there is a value to be returned, it's named out_. 
*/


FtpThread::FtpThread() : QThread()
{
	mp_eventreceiver = NULL;
	mp_ftp = new ftplib();
	
	mp_ftp->SetCallbackArg(this);
	mp_ftp->SetCallbackLogFunction(FtpThread::CallbackLog);
	
	m_host = "";
	m_user = "";
	m_pass = "";
	
	out_path = "";
}

FtpThread::~FtpThread()
{
	delete mp_ftp;
}

/* callback function for the logs */

void FtpThread::CallbackLog(char *log, void *arg, bool out)
{
	FtpThread* ftp = static_cast<FtpThread*>(arg);

	if (out)
	{	
		ftp->out_outlog+=log;
	
		if (ftp->out_outlog.endsWith("\r\n")) 
		{
			ftp->out_outlog.replace("\r\n", "\n");
			ftp->Event(EventHandler::outlog);
		}
	}
	else 
	{
		ftp->out_inlog = log;
		ftp->out_inlog.replace("\r\n", "\n");
		ftp->Event(EventHandler::inlog);
	}
}

/* set the receiver for the events the thread posts when a certain ftp operation is done */

void FtpThread::SetEventReceiver(QObject* eventreceiver)
{
	mp_eventreceiver = eventreceiver;
}

/* connect to host */

bool FtpThread::Connect(QString host)
{
	if (running()) return false;
	else
	{	
		m_tasklist.append(FtpThread::connect);
		m_host = host;
		return true;
	}
}

/* login with user and pass */

bool FtpThread::Login(QString user, QString pass)
{
	if (running()) return false;
	else
	{
		m_tasklist.append(FtpThread::login);
		m_user = user;
		m_pass = pass;
		return true;
	}
}

/* quit the ftp session*/

bool FtpThread::Quit()
{
	if (running()) return false;
	else
	{
		m_tasklist.append(FtpThread::quit);
		return true;
	}
}

/* get file */

bool FtpThread::Get(QString src, QString dst, unsigned long resume)
{
	if (running()) return false;
	else
	{
		m_tasklist.append(FtpThread::get);
		m_getsrclist.push_back(src);
		m_getdstlist.push_back(dst);
		m_getresumelist.push_back(resume);
		return true;
	}
}

/* put file */

bool FtpThread::Put(QString src, QString dst, unsigned long resume)
{
	if (running()) return false;
	else
	{
		m_tasklist.append(FtpThread::put);
		m_putsrclist.push_back(src);
		m_putdstlist.push_back(dst);
		m_putresumelist.push_back(resume);
		return true;
	}
}

/* rename file */

bool FtpThread::Rename(QString src, QString dst)
{
	if (running()) return false;
	else
	{
		m_tasklist.append(FtpThread::rename);
		m_renamesrclist.push_back(src);
		m_renamedstlist.push_back(dst);
		return true;
	}
}

/* set wether the data channel is encrypted or not */

bool FtpThread::Pasv(bool flag)
{
	if (running()) return false;
	else
	{
		if (flag) mp_ftp->SetConnmode(ftplib::pasv);
		else mp_ftp->SetConnmode(ftplib::port);
		return true;
	}
}

/* set wether the data channel is encrypted or not */

bool FtpThread::EncryptData(bool flag)
{
	if (running()) return false;
	else
	{
		if (flag) m_tasklist.append(FtpThread::dataencon);
		else m_tasklist.append(FtpThread::dataencoff);
		return true;
	}
}

/* retrieve the current workind dir */

bool FtpThread::Pwd()
{
	if (running()) return false;
	else
	{
		m_tasklist.append(FtpThread::pwd);
		return true;
	}
}

/* change working dir to the given path */

bool FtpThread::Chdir(QString path)
{
	if (running()) return false;
	else
	{
		m_tasklist.append(FtpThread::chdir);
		m_chdirlist.push_back(path);
		return true;
	}
}

/* creates a directory */

bool FtpThread::Mkdir(QString path)
{
	if (running()) return false;
	else
	{
		m_tasklist.append(FtpThread::mkdir);
		m_mkdirlist.push_back(path);
		return true;
	}
}

/* step above in the working dir */

bool FtpThread::Cdup()
{
	if (running()) return false;
	else
	{
		m_tasklist.append(FtpThread::cdup);
		return true;
	}
}

/* retrieve dir contents */

bool FtpThread::Dir()
{
	if (running()) return false;
	else
	{
		m_tasklist.append(FtpThread::dir);
		return true;
	}
}

/* attempt negotiating an encrypted session */

bool FtpThread::Authtls()
{
	if (running()) return false;
	else
	{
		m_tasklist.append(FtpThread::authtls);
		return true;
	}
}

/* delete a file  */

bool FtpThread::Rm(QString name)
{
	if (running()) return false;
	else
	{
		m_tasklist.append(FtpThread::rm);
		m_rmlist.push_back(name);
		return true;
	}
}

/* delete a directory  */

bool FtpThread::Rmdir(QString name)
{
	if (running()) return false;
	else
	{
		m_tasklist.append(FtpThread::rmdir);
		m_rmdirlist.push_back(name);
		return true;
	}
}
	
/* issue raw command  */

bool FtpThread::Raw(QString cmd)
{
	if (running()) return false;
	else
	{
		m_tasklist.append(FtpThread::raw);
		m_rawlist.push_back(cmd);
		return true;
	}
}

/* scan the directory recursively */

bool FtpThread::Scandir(kbdirectory* dir)
{
	if (running()) return false;
	else
	{
		m_tasklist.append(FtpThread::scandir);
		out_scandir = dir;
		return true;
	}
}
/* the thread methods */

void FtpThread::Connect_thread()
{
	int result;
		
	result = mp_ftp->Connect(m_host.latin1());

	if (result) Event(EventHandler::connect_success);
	else 
	{
		ClearQueue();
		Event(EventHandler::connect_failure);
	}
}

void FtpThread::Authtls_thread()
{
	int result;
	
	result = mp_ftp->NegotiateEncryption();

	if (result) Event(EventHandler::authtls_success);
	else 
	{
		ClearQueue();
		Event(EventHandler::authtls_failure);
	}
}

void FtpThread::Login_thread()
{
	int result;
	
	result = mp_ftp->Login(m_user.latin1(), m_pass.latin1());
	
	if (result) Event(EventHandler::login_success);
	else
	{
		ClearQueue();
		Event(EventHandler::login_failure);
	}
}

void FtpThread::Dataencoff_thread()
{
	int result;
	
	result = mp_ftp->SetDataEncryption(ftplib::unencrypted);

	if (result) Event(EventHandler::encryptdataoff_success);
	else 
	{
		if (ConnectionLost()) Event(EventHandler::connectionlost);
		else Event(EventHandler::encryptdataoff_failure);
	}
}

void FtpThread::Dataencon_thread()
{
	int result;
	
	result = mp_ftp->SetDataEncryption(ftplib::secure);

	if (result) Event(EventHandler::encryptdataon_success);
	else 
	{
		if (ConnectionLost()) Event(EventHandler::connectionlost);
		else Event(EventHandler::encryptdataon_failure);
	}
}

void FtpThread::Quit_thread()
{
	int result;
	
	result = mp_ftp->Quit();
	
	if (result) Event(EventHandler::quit_success);
	else 
	{
		if (ConnectionLost()) Event(EventHandler::connectionlost);
		else Event(EventHandler::quit_failure);
	}
}

void FtpThread::Pwd_thread()
{
	char buffer[1024];
	int result;

	result = mp_ftp->Pwd(buffer, 1024);
	
	if (result) 
	{
		out_path = buffer;
		Event(EventHandler::pwd_success);
	}
	else 
	{
		if (ConnectionLost()) Event(EventHandler::connectionlost);
		else Event(EventHandler::pwd_failure);
	}
}

void FtpThread::Chdir_thread()
{
	int result;
	
	QString path = m_chdirlist.front();
	m_chdirlist.pop_front();
	
	result = mp_ftp->Chdir(path.latin1());

	if (result) Event(EventHandler::chdir_success);
	else 
	{
		if (ConnectionLost()) Event(EventHandler::connectionlost);
		else Event(EventHandler::chdir_failure);
	}
}

void FtpThread::Mkdir_thread()
{
	int result;
	
	QString path = m_mkdirlist.front();
	m_mkdirlist.pop_front();
	
	result = mp_ftp->Mkdir(path.latin1());

	if (result) Event(EventHandler::mkdir_success);
	else 
	{
		if (ConnectionLost()) Event(EventHandler::connectionlost);
		else Event(EventHandler::mkdir_failure);
	}
}

void FtpThread::Cdup_thread()
{
	int result;
	
	result = mp_ftp->Cdup();

	if (result) Event(EventHandler::cdup_success);
	else 
	{
		if (ConnectionLost()) Event(EventHandler::connectionlost);
		else Event(EventHandler::cdup_failure);
	}
}

void FtpThread::Rm_thread()
{
	int result;
	
	QString name = m_rmlist.front();
	m_rmlist.pop_front();
	
	result = mp_ftp->Delete(name.latin1());

	if (result) Event(EventHandler::rm_success);
	else 
	{
		if (ConnectionLost()) Event(EventHandler::connectionlost);
		else Event(EventHandler::rm_failure);
	}
}

void FtpThread::Get_thread()
{
	int result;
	
	QString src = m_getsrclist.front();
	m_getsrclist.pop_front();
	QString dst = m_getdstlist.front();
	m_getdstlist.pop_front();
	unsigned long resume = m_getresumelist.front();
	m_getresumelist.pop_front();
	
	if (resume == 0) result = mp_ftp->Get(dst.latin1(), src.latin1(), ftplib::image);
	else result = mp_ftp->Get(dst.latin1(), src.latin1(), ftplib::image, resume);

	if (result) Event(EventHandler::get_success);
	else 
	{
		if (ConnectionLost()) Event(EventHandler::connectionlost);
		else Event(EventHandler::get_failure);
	}
}

void FtpThread::Put_thread()
{
	int result;
	
	QString src = m_putsrclist.front();
	m_putsrclist.pop_front();
	QString dst = m_putdstlist.front();
	m_putdstlist.pop_front();
	unsigned long resume = m_putresumelist.front();
	m_putresumelist.pop_front();
	
	if (resume == 0) result = mp_ftp->Put(src.latin1(), dst.latin1(), ftplib::image);
	else result = mp_ftp->Put(src.latin1(), dst.latin1(), ftplib::image, resume);

	if (result) Event(EventHandler::put_success);
	else 
	{
		if (ConnectionLost()) Event(EventHandler::connectionlost);
		else Event(EventHandler::put_failure);
	}
}

void FtpThread::Rename_thread()
{
	int result;
	
	QString src = m_renamesrclist.front();
	m_renamesrclist.pop_front();
	QString dst = m_renamedstlist.front();
	m_renamedstlist.pop_front();
	
	result = mp_ftp->Rename(src.latin1(),dst.latin1());

	if (result) Event(EventHandler::rename_success);
	else 
	{
		if (ConnectionLost()) Event(EventHandler::connectionlost);
		else Event(EventHandler::rename_failure);
	}
}

void FtpThread::Dir_thread()
{
	int result;
	QString dirname;
	 
	dirname = QDir::homeDirPath() + 
		"/.kasablanca/" +
		QString::number((int) time(NULL) & 0xffff) + 
		".dir";
	
	result = mp_ftp->Dir(dirname.latin1(), "");

	if (result) 
	{		
		out_dirlist.clear();
		out_filelist.clear();		
		FormatFilelist(dirname.latin1(), out_path, &out_dirlist, &out_filelist); 
		Event(EventHandler::dir_success);
	}
	else 
	{
		if (ConnectionLost()) Event(EventHandler::connectionlost);
		else Event(EventHandler::dir_failure);
	}
	
	QFile::remove(dirname);
}

void FtpThread::Scandir_thread()
{
	bool result = true;

	list<kbdirectory*>::iterator dirIterator;
	
	for(dirIterator = out_scandir->Dirlist()->begin(); dirIterator != out_scandir->Dirlist()->end(); dirIterator++)
	{
		result = Scandir_recurse(*dirIterator);
	}
	
	if (result) Event(EventHandler::scandir_success);
	else 
	{
		if (ConnectionLost()) Event(EventHandler::connectionlost);
		else Event(EventHandler::scandir_failure);
	}
}

void FtpThread::Rmdir_thread()
{
	bool result;
	
	QString name = m_rmdirlist.front();
	m_rmdirlist.pop_front();
	
	result = Delete_recurse(name.latin1());

	if (result) Event(EventHandler::rmdir_success);
	else 
	{
		if (ConnectionLost()) Event(EventHandler::connectionlost);
		else Event(EventHandler::rmdir_failure);
	}
}

void FtpThread::Raw_thread()
{
	bool result;
	
	QString cmd = m_rawlist.front();
	m_rawlist.pop_front();
	
	result = mp_ftp->Raw(cmd.latin1());

	if (result) Event(EventHandler::raw_success);
	else 
	{
		if (ConnectionLost()) Event(EventHandler::connectionlost);
		else Event(EventHandler::raw_failure);
	}
}

/* the thread's main loop */

void FtpThread::run()
{
	while (!m_tasklist.empty())
	{
		task t = m_tasklist.front();
		m_tasklist.pop_front();
	
		switch (t)
		{
			case FtpThread::connect:
				Connect_thread();
				break;
			case FtpThread::login:
				Login_thread();
				break;
			case FtpThread::quit:
				Quit_thread();
				break;
			case FtpThread::pwd:
				Pwd_thread();
				break;
			case FtpThread::chdir:
				Chdir_thread();
				break;
			case FtpThread::cdup:
				Cdup_thread();
				break;
			case FtpThread::dir:
				Dir_thread();
				break;
			case FtpThread::scandir:
				Scandir_thread();
				break;
			case FtpThread::rm:
				Rm_thread();
				break;
			case FtpThread::rmdir:
				Rmdir_thread();
				break;
			case FtpThread::authtls:
				Authtls_thread();
				break;
			case FtpThread::dataencon:
				Dataencon_thread();
				break;
			case FtpThread::dataencoff:
				Dataencoff_thread();
				break;
			case FtpThread::get:
				Get_thread();
				break;
			case FtpThread::mkdir:
				Mkdir_thread();
				break;
			case FtpThread::rename:
				Rename_thread();
				break;
			case FtpThread::raw:
				Raw_thread();
				break;
			case FtpThread::put:
				Put_thread();
				break;
			default:
				Event(EventHandler::error);
				break;
		}
	}
	Event(EventHandler::finished);
}

/* event is posted to the eventreceiver */

void FtpThread::Event(EventHandler::EventType type)
{
	if (mp_eventreceiver == NULL) qWarning("mp_eventreceiver is NULL");
	else qApp->postEvent(mp_eventreceiver, new QCustomEvent(type));		
}

/* parses the dir file */

bool FtpThread::FormatFilelist(const char *filename, 
	QString current, 
	list<RemoteFileInfo> *dirtable, 
	list<RemoteFileInfo> *filetable
)
{
	int i, blocks, space, month_int = 1;
	unsigned int loc, fileloc, datebegin, sizebegin = 0;
	string buffer, filestring;

	RemoteFileInfo fi;
	QFileInfo dp;

	FILE* dirfile;

	char file[1024];

	char month[][5] = {
		"... ", "Jan ", "Feb ", "Mar ", "Apr ", "May ", "Jun ", "Jul ", "Aug ",
		"Sep ", "Oct ", "Nov ", "Dec "
	};

	dirfile = fopen(filename, "r");
	if (dirfile == NULL)
	{
		qWarning("failed open dirfile");
		return false;
	}

	while (fgets(file, 1024, dirfile) != NULL)
	{
		*(strchr(file,'\n')) = '\0';
		buffer = file;

		for (i = 1; i < 13; i++)
		{
			loc = buffer.find(month[i], 0);
			if (loc != string::npos)
			{
				month_int = i;
				break;
			}
		}
		if( loc == string::npos )
		{
			#ifdef DEBUG
				qWarning("no month entry found");
			#endif
			loc = buffer.length();
		}

		datebegin = loc;

		space = 1;
		blocks = 0;
		fileloc = 0;
		while (loc < buffer.length())
		{
				if (space)
				{
					// look for a space
					if (buffer[loc] == ' ')
					{
						blocks++;
						space = !space;
					}
				}
				else
				{
					// look for a non-space
					if (buffer[loc] != ' ')
					{
						if (blocks > 2)
						{
							fileloc = loc;
							break;
						}
						space = !space;
					}
				}
				loc += 1;
		}
		if (blocks != 3)
		{
				#ifdef DEBUG
				qWarning("ignoring invalid line in dirlist");
				#endif
		}
		else
		{
			QString date(buffer.substr( datebegin, loc - datebegin - 1).c_str());

			int day_int = date.section( ' ', 1, 1 ).toInt();
			int year_int = date.section( ' ', -1, -1 ).toInt();

			if (day_int == 0) day_int = date.section( ' ', 1, 2 ).toInt();

			if (year_int == 0)
			{
				year_int = QDate::currentDate().year();
				if (month_int > QDate::currentDate().month()) year_int--;
			}

			uint date_int = year_int * 10000 + month_int * 100 + day_int;

			space = 0;
			blocks = 0;
			loc = datebegin;
			while (loc > 0)
			{
				if (space)
				{
					if (buffer[loc] == ' ')
					{
						blocks++;
						space = !space;
					}
				}
				else
				{
					if (buffer[loc] != ' ')
					{
						if (blocks > 1)
						{
							sizebegin = loc + 1;
							break;
						}
						space = !space;
					}
				}
				loc--;
			}

			int size = atoi(buffer.substr( sizebegin, datebegin - sizebegin).c_str());
			
			filestring.erase();
			filestring.append(buffer, fileloc, buffer.length() - fileloc);
			if ((filestring.compare(".") != 0) && (filestring.compare("..") != 0))
			{
				if ((*file == 'd') || (*file == 'D'))
				{
					RemoteFileInfo di(current, filestring.c_str(), size, date.latin1(), date_int);
					dirtable->push_back(di);
				}
				else if ((*file == 'l') || (*file == 'L'))
				{
					qWarning("links to files not supported yet!");
				}
				else
				{
					RemoteFileInfo fi(current, filestring.c_str(), size, date.latin1(), date_int);
					filetable->push_back(fi);
				}
			}
		}
	}
	fclose(dirfile);
	return true;
}

/* recursive method to check content of a directory */

bool FtpThread::Scandir_recurse(kbdirectory* dir)
{
	char currentdir[1024];
	int result;
	list<RemoteFileInfo> dirlist;
	list<RemoteFileInfo> filelist;
	QString dirname;
	
	result = mp_ftp->Pwd(currentdir, 1024);
	
	if (!result) 
	{
		Event(EventHandler::misc_failure);
		return false;
	}
	else Event(EventHandler::misc_success);
		
	result = mp_ftp->Chdir(dir->Name().latin1());
	
	if (!result) 
	{
		Event(EventHandler::misc_failure);
		return true;
	}
	else Event(EventHandler::misc_success);
	
	dirname = QDir::homeDirPath() + 
	"/.kasablanca/" +
	QString::number((int) time(NULL) & 0xffff) + 
	".dir";
	
	result = mp_ftp->Dir(dirname.latin1(), "");
			
	if (!result) 
	{
		Event(EventHandler::misc_failure);
		return false;
	}
	else Event(EventHandler::misc_success);
	
	if(!FormatFilelist(dirname.latin1(), "", &filelist, &dirlist)) return false; 
	
	QFile::remove(dirname);
	
	list<RemoteFileInfo>::iterator fiIterator;
	
	for(fiIterator = filelist.begin(); fiIterator != filelist.end(); fiIterator++)
	{
		dir->AddFile((*fiIterator).fileName());
	}
	
	for(fiIterator = dirlist.begin(); fiIterator != dirlist.end(); fiIterator++)
	{
		kbdirectory* newdir = dir->AddDirectory((*fiIterator).fileName());
		if (!Scandir_recurse(newdir)) return false;
	}
		
	result = mp_ftp->Chdir(currentdir);
	
	if (!result) 
	{
		Event(EventHandler::misc_failure);
		return false;
	}
	else Event(EventHandler::misc_success);
	
	return true;
}

/* recursive method to delete directories */

bool FtpThread::Delete_recurse(QString name)
{
	char currentdir[1024];
	int result;
	list<RemoteFileInfo> dirlist;
	list<RemoteFileInfo> filelist;
	QString dirname;
	
	result = mp_ftp->Pwd(currentdir, 1024);
	
	if (!result) 
	{
		Event(EventHandler::misc_failure);
		return false;
	}
	else Event(EventHandler::misc_success);
		
	result = mp_ftp->Chdir(name.latin1());
	
	if (!result) 
	{
		Event(EventHandler::misc_failure);
		return true;
	}
	else Event(EventHandler::misc_success);
	
	dirname = QDir::homeDirPath() + 
	"/.kasablanca/" +
	QString::number((int) time(NULL) & 0xffff) + 
	".dir";
	
	result = mp_ftp->Dir(dirname.latin1(), "");
			
	if (!result) 
	{
		Event(EventHandler::misc_failure);
		return false;
	}
	else Event(EventHandler::misc_success);
	
	if(!FormatFilelist(dirname.latin1(), "", &filelist, &dirlist)) return false; 
	
	QFile::remove(dirname);
	
	list<RemoteFileInfo>::iterator fiIterator;
	
	for(fiIterator = filelist.begin(); fiIterator != filelist.end(); fiIterator++)
	{
		mp_ftp->Delete((*fiIterator).fileName().latin1());
	}
	
	for(fiIterator = dirlist.begin(); fiIterator != dirlist.end(); fiIterator++)
	{
		if (!Delete_recurse((*fiIterator).fileName())) return false;
	}
		
	result = mp_ftp->Chdir(currentdir);
	
	if (!result) 
	{
		Event(EventHandler::misc_failure);
		return false;
	}
	else Event(EventHandler::misc_success);
	
	result = mp_ftp->Rmdir(name.latin1());
	
	if (!result) 
	{
		Event(EventHandler::misc_failure);
		return false;
	}
	else Event(EventHandler::misc_success);
	
	return true;
}

/* check if we lost the connection */

bool FtpThread::ConnectionLost()
{
	QString response;
	
	response = mp_ftp->LastResponse();
	
	if (response.startsWith("421")) 
	{
		ClearQueue();
		return true;
	}
	else return false;
}
	
/* empty queue */

void FtpThread::ClearQueue()
{
	m_tasklist.clear();
}
	
	
