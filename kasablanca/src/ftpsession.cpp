//
// C++ Implementation: ftpsession
//
// Description: 
//
//
// Author: Magnus Kulke <sikor_sxe@radicalapproach.de>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <klocale.h>
#include <kglobal.h>
#include <kmessagebox.h>
#include <kiconloader.h>
#include <qtextedit.h>
#include <qtoolbutton.h>
#include <qpopupmenu.h>
#include <qwidget.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qlabel.h>
#include <qpixmap.h>

#include "customconnectdialog.h"
#include "ftpthread.h"
#include "eventhandler.h"
#include "siteinfo.h"
#include "ftpsession.h"
#include "kasablanca.h"
#include "diritem.h"
#include "fileitem.h"
#include "remotefileinfo.h"

FtpSession::FtpSession(QObject *parent, const char *name)
 : QObject(parent, name)
{
	mp_ftpthread = new FtpThread();
	mp_eventhandler = new EventHandler(this, "event handler");
	mp_siteinfo = new siteinfo();

	m_connected = false;
	m_occupied = false;
		
	mp_eventhandler->SetFtpThread(mp_ftpthread); 
	mp_ftpthread->SetEventReceiver(mp_eventhandler);

	m_iconencrypted = KGlobal::iconLoader()->loadIconSet("encrypted",KIcon::Small).pixmap(QIconSet::Small,QIconSet::Normal);
   m_iconunencrypted = KGlobal::iconLoader()->loadIconSet("encrypted",KIcon::Small).pixmap(QIconSet::Small,QIconSet::Disabled);
	
	connect(mp_eventhandler, SIGNAL(ftp_login(bool)), SLOT(SLOT_Login(bool)));
	connect(mp_eventhandler, SIGNAL(ftp_log(QString, bool)), SLOT(SLOT_Log(QString, bool)));
	connect(mp_eventhandler, SIGNAL(ftp_connect(bool)), SLOT(SLOT_Connect(bool)));
	connect(mp_eventhandler, SIGNAL(ftp_login(bool)), SLOT(SLOT_Login(bool)));
	connect(mp_eventhandler, SIGNAL(ftp_quit(bool)), SLOT(SLOT_Quit(bool)));
	connect(mp_eventhandler, SIGNAL(ftp_chdir(bool)), SLOT(SLOT_Chdir(bool)));
	connect(mp_eventhandler, SIGNAL(ftp_pwd(bool, QString)), SLOT(SLOT_Pwd(bool, QString)));
	connect(mp_eventhandler, SIGNAL(ftp_dir(bool, list<RemoteFileInfo>, list<RemoteFileInfo>)), 
		SLOT(SLOT_Dir(bool, list<RemoteFileInfo>, list<RemoteFileInfo>)));
	connect(mp_eventhandler, SIGNAL(ftp_encryptdata(bool, bool)), SLOT(SLOT_EncryptData(bool, bool)));
	connect(mp_eventhandler, SIGNAL(ftp_finished()), SLOT(SLOT_Finish()));
	connect(mp_eventhandler, SIGNAL(ftp_connectionlost()), SLOT(SLOT_ConnectionLost()));
}

FtpSession::~FtpSession()
{
}

void FtpSession::SLOT_Log(QString log, bool out) 
{ 
	if (out) m_loglist.push_back(make_pair(log, true));
	else m_loglist.push_back(make_pair(log, false));
}

void FtpSession::SLOT_ConnectMenu(int i)
{
	if (i == 0) 
	{
		CustomConnectDialog dlg;
    	mp_siteinfo->Clear();
    	dlg.mp_site = mp_siteinfo;
    	if (dlg.exec() == QDialog::Rejected) return; 	
    	else if (!mp_siteinfo->CheckContent()) 
		{
			KMessageBox::error(0,"That site information is not legit.");
			return;
		}
	}
	else 
	{
		mp_siteinfo = &static_cast<Kasablanca*>(parent())->m_bookmarks.at(i - 1);
		qWarning("INFO: connecting to bookmark entry named: %s", mp_siteinfo->GetName());
	}
	Connect();
	Occupy();
	mp_ftpthread->Connect(mp_siteinfo->GetInfo());
	if (mp_siteinfo->GetTls() > 0) mp_ftpthread->Authtls();
	if (mp_siteinfo->GetPasv() > 0) mp_ftpthread->Pasv(true);
	else mp_ftpthread->Pasv(false);
	mp_ftpthread->Login(mp_siteinfo->GetUser(), mp_siteinfo->GetPass());
	mp_ftpthread->start();
}

void FtpSession::SLOT_Finish()
{
	qWarning("INFO: gui freed");
	Free();
}
void FtpSession::SLOT_ConnectButton()
{
	/* this button is also an abort button */

	if (Occupied()) 
	{
		m_loglist.push_back(make_pair(i18n("aborted ftp operation"), false));
		SLOT_Quit(false);
		SLOT_Finish();
		Disconnect();
		mp_ftpthread->terminate();
		mp_ftpthread->wait(KB_THREAD_TIMEOUT);
		mp_ftpthread->ClearQueue();
	}
	
	/* when connected issue disconnect */
	
	else if (Connected())
	{
		Disconnect();
		Occupy();
		mp_ftpthread->Quit();
		mp_ftpthread->start();
	}
	
	/* when disconnected show connect menu */
	
	else if (!Connected()) mp_bookmarksmenu->exec(mp_connectbutton->mapToGlobal(QPoint(0,0)));
}

void FtpSession::SLOT_CwdLine()
{
	if (Occupied()) 
	{	
		qWarning("ERROR: refresh button pressed while occupied");
		return;
	}
	if (Connected())
	{	
		Occupy();
		mp_ftpthread->Chdir(mp_cwdline->text());
		RefreshBrowser();
		mp_ftpthread->start();
	}
	else qWarning("WARNING: local browsing not yet implemented"); 		
}

void FtpSession::SLOT_RefreshButton()
{
	if (Occupied()) 
	{	
		qWarning("ERROR: refresh button pressed while occupied");
		return;
	}
	if (Connected())
	{	
		Occupy();
		RefreshBrowser();
		mp_ftpthread->start();
	}
	else 
	{
		while (QListViewItem* tmpviewitem = mp_browser->firstChild()) delete tmpviewitem;
		qWarning("WARNING: local browsing not yet implemented");
	}
}

void FtpSession::SLOT_Connect(bool success)
{
	if (!success) 
	{
		m_loglist.push_back(make_pair(i18n("connection failed"), false));
		Disconnect();
	}
	PrintLog(success);	
}

void FtpSession::SLOT_EncryptData(bool success, bool)
{
	PrintLog(success);	
}
	
void FtpSession::SLOT_Chdir(bool success)
{
	PrintLog(success);
}	

void FtpSession::SLOT_Login(bool success)
{
	PrintLog(success);	
	if (!success) Disconnect();
}

void FtpSession::SLOT_Quit(bool success)
{
	PrintLog(success);	
}

void FtpSession::SLOT_ConnectionLost()
{
	PrintLog(false);	
	Disconnect();
}

void FtpSession::SLOT_Dir(bool success, list<RemoteFileInfo> dirlist, list<RemoteFileInfo> filelist)
{
	PrintLog(success);
	if (success)
	{	
		while (QListViewItem* tmpviewitem = mp_browser->firstChild()) delete tmpviewitem;
		QListViewItem* dirup = new QListViewItem(mp_browser, "..");
		dirup->setPixmap(0, KGlobal::iconLoader()->loadIcon("folder",KIcon::Small));
		dirup->setSelectable(false);	
		list<RemoteFileInfo>::iterator i;
		for (i = dirlist.begin(); i != dirlist.end(); i++) new diritem(&*i, mp_browser, mp_browser->lastItem());	
		for (i = filelist.begin(); i != filelist.end(); i++) new fileitem(&*i, mp_browser, mp_browser->lastItem());
	}
}

void FtpSession::SLOT_Pwd(bool success, QString pwd)
{
	PrintLog(success);
	if (success) 
	{
		m_remoteworkingdir = pwd;
		mp_cwdline->setText(pwd);
	}
}

void FtpSession::PrintLog(bool success)
{
	list<logentries>::iterator i;
	for (i = m_loglist.begin(); i != m_loglist.end(); i++)
	{
		if ((*i).second == true)
		{
			if (success) mp_logwindow->setColor(green);
			else mp_logwindow->setColor(red);
			mp_logwindow->append((*i).first);
		}
		else
		{
			mp_logwindow->setColor(yellow);
			mp_logwindow->append((*i).first);
		}	
	}	
	m_loglist.clear();
}

void FtpSession::Connect()
{
	mp_bookmarksmenu->setEnabled(false);
	mp_connectbutton->setIconSet(KGlobal::iconLoader()->loadIconSet("connect_established",KIcon::Toolbar));
	mp_statusline->setText(i18n("Occupied"));
	m_connected = true;
}

void FtpSession::Disconnect()
{
	mp_bookmarksmenu->setEnabled(true);
	mp_connectbutton->setIconSet(KGlobal::iconLoader()->loadIconSet("connect_no",KIcon::Toolbar));
	mp_statusline->setText(i18n("Disconnected"));
	m_connected = false;
}

void FtpSession::Occupy()
{
	mp_browser->setEnabled(false);
	mp_cmdline->setEnabled(false);
	mp_cwdline->setEnabled(false);
	mp_refreshbutton->setEnabled(false);
	mp_statusline->setText(i18n("Occupied"));
	m_occupied = true;
}

void FtpSession::Free()
{
	mp_browser->setEnabled(true);
	mp_cmdline->setEnabled(true);
	mp_cwdline->setEnabled(true);
	mp_refreshbutton->setEnabled(true);
	if (m_connected) mp_statusline->setText(i18n("Connected"));
	else mp_statusline->setText(i18n("Disconnected"));	
	if ((mp_siteinfo->GetTls() > 0) && (m_connected)) mp_encryptionicon->setPixmap(m_iconencrypted);
	else mp_encryptionicon->setPixmap(m_iconunencrypted);	
	m_occupied = false;
}

void FtpSession::RefreshBrowser()
{
	mp_ftpthread->Pwd();
	if (mp_siteinfo->GetTls() > 1) mp_ftpthread->EncryptData(true);
	mp_ftpthread->Dir();
}

#include "ftpsession.moc"
