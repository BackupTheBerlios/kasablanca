//
// C++ Interface: ftpsession
//
// Description: 
//
//
// Author: Magnus Kulke <sikor_sxe@radicalapproach.de>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef FTPSESSION_H
#define FTPSESSION_H

#define KB_THREAD_TIMEOUT 1000

#include <qstring.h>
#include <list>

#include "remotefileinfo.h"

#include <qobject.h>

using namespace std;

class FtpThread;
class EventHandler;
class QMutex;
class QTextEdit;
class QToolButton;
class QListView;
class siteinfo;
class QPopupMenu;
class QLineEdit;

/**
@author Magnus Kulke
*/
class FtpSession : public QObject
{
Q_OBJECT
public:
	FtpSession(QObject *parent = 0, const char *name = 0);
	~FtpSession();
	void SetLogWindow(QTextEdit* logwindow) { mp_logwindow = logwindow; };
	void SetRefreshButton(QToolButton* refreshbutton) { mp_refreshbutton = refreshbutton; };
	void SetConnectButton(QToolButton* connectbutton) { mp_connectbutton = connectbutton; };
	void SetBrowser(QListView* browser) { mp_browser = browser; };
	void SetCwdLine(QLineEdit* cwdline) { mp_cwdline = cwdline; };
	void SetCmdLine(QLineEdit* cmdline) { mp_cmdline = cmdline; };
	void SetBookmarksMenu(QPopupMenu *bookmarksmenu) { mp_bookmarksmenu = bookmarksmenu; };
	bool Connected() { return m_connected; };
	void Disconnect();
	void Connect();
	bool Occupied() { return m_occupied; };
	void Occupy();
	void Free();
	
private:
	FtpThread *mp_ftpthread;
	EventHandler *mp_eventhandler;
	QMutex *mp_mutex;
	siteinfo *mp_siteinfo;
	QTextEdit *mp_logwindow;
	QToolButton *mp_connectbutton, *mp_refreshbutton;
	QLineEdit *mp_cwdline, *mp_cmdline;
	QPopupMenu* mp_bookmarksmenu;
	QListView *mp_browser;
	QString m_log;
	bool m_connected, m_occupied;
	
public slots:
	void SLOT_Log(QString log) { m_log = m_log + log; };
	void SLOT_ConnectMenu(int i);
	void SLOT_Connect(bool success);
	void SLOT_Login(bool success);
	void SLOT_Quit(bool success);
	void SLOT_Pwd(bool success, QString pwd);
	void SLOT_Dir(bool success, list<RemoteFileInfo> dirlist, list<RemoteFileInfo> filelist);
	void SLOT_ConnectButton();
	void SLOT_RefreshButton();
	void SLOT_Finish();
	
private:
	void PrintLog(bool success);
};

#endif
