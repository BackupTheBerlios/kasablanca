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
class QLabel;
class QPixmap;
class QPoint;

typedef pair<QString, bool> logentries;

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
	void SetStatusLine(QLabel* statusline) { mp_statusline = statusline; };
	void SetEncryptionIcon(QLabel* encryptionicon) { mp_encryptionicon = encryptionicon; };
	void SetBookmarksMenu(QPopupMenu *bookmarksmenu) { mp_bookmarksmenu = bookmarksmenu; };
	void SetRclickMenu(QPopupMenu *rclickmenu) { mp_rclickmenu = rclickmenu; };
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
	QPopupMenu* mp_bookmarksmenu, *mp_rclickmenu;
	QListView *mp_browser;
	QLabel *mp_statusline, *mp_encryptionicon;
	QString m_remoteworkingdir;
	bool m_connected, m_occupied;
	list<logentries> m_loglist;
	QPixmap m_iconencrypted, m_iconunencrypted;
	
public slots:
	void SLOT_Log(QString log, bool out);
	void SLOT_ActionMenu(int i);
	void SLOT_ConnectMenu(int i);
	void SLOT_ItemClicked(QListViewItem*);
	void SLOT_ItemRClicked(QListViewItem * item, const QPoint & point, int col);
	void SLOT_Connect(bool success);
	void SLOT_Login(bool success);
	void SLOT_Quit(bool success);
	void SLOT_Pwd(bool success, QString pwd);
	void SLOT_Chdir(bool success);
	void SLOT_Misc(bool success);
	void SLOT_EncryptData(bool success, bool enabled);
	void SLOT_Dir(bool success, list<RemoteFileInfo> dirlist, list<RemoteFileInfo> filelist);
	void SLOT_ConnectButton();
	void SLOT_RefreshButton();
	void SLOT_CwdLine();
	void SLOT_CmdLine();
	void SLOT_Finish();
	void SLOT_ConnectionLost();
	
private:
	void PrintLog(bool success);
	void RefreshBrowser();
		
};

#endif
