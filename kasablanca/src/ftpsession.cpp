//
// C++ Implementation: ftpsession
//
// Description: 
//
//
// Author: Magnus Kulke <mkulke@magnusmachine>, (C) 2004
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

#include "customconnectdialog.h"
#include "ftpthread.h"
#include "eventhandler.h"
#include "siteinfo.h"
#include "ftpsession.h"
#include "kasablanca.h"

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

	connect(mp_eventhandler, SIGNAL(ftp_login(bool)), SLOT(SLOT_Login(bool)));
	connect(mp_eventhandler, SIGNAL(ftp_log(QString)), SLOT(SLOT_Log(QString)));
	connect(mp_eventhandler, SIGNAL(ftp_connect(bool)), SLOT(SLOT_Connect(bool)));
	connect(mp_eventhandler, SIGNAL(ftp_login(bool)), SLOT(SLOT_Login(bool)));
	connect(mp_eventhandler, SIGNAL(ftp_quit(bool)), SLOT(SLOT_Quit(bool)));
	connect(mp_eventhandler, SIGNAL(ftp_pwd(bool, QString)), SLOT(SLOT_Pwd(bool, QString)));
	connect(mp_eventhandler, SIGNAL(ftp_finished()), SLOT(SLOT_Finish()));
	
}

FtpSession::~FtpSession()
{
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
		m_log = "aborted ftp operation";
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

void FtpSession::SLOT_RefreshButton()
{
	if (Occupied()) 
	{	
		qWarning("WARNING: refresh button pressed while occupied");
		return;
	}
	if (Connected())
	{	
		Occupy();
		mp_ftpthread->Pwd();
		mp_ftpthread->Dir();
		mp_ftpthread->start();
	}
	else qWarning("WARNING: local listing not yet implemented");
}

void FtpSession::SLOT_Connect(bool success)
{
	if (!success) 
	{
		m_log = i18n("connection failed");
		Disconnect();
	}
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

void FtpSession::SLOT_Dir(bool success, list<RemoteFileInfo>, list<RemoteFileInfo>)
{
	PrintLog(success);
}

void FtpSession::SLOT_Pwd(bool success, QString)
{
	PrintLog(success);
}

void FtpSession::PrintLog(bool success)
{
	if (success == true) mp_logwindow->setColor(green);
	else mp_logwindow->setColor(red);	
	mp_logwindow->append(m_log);
	m_log = "";		
}

void FtpSession::Connect()
{
	mp_bookmarksmenu->setEnabled(false);
	mp_connectbutton->setIconSet(KGlobal::iconLoader()->loadIconSet("connect_established",KIcon::Toolbar));
	m_connected = true;
}

void FtpSession::Disconnect()
{
	mp_bookmarksmenu->setEnabled(true);
	mp_connectbutton->setIconSet(KGlobal::iconLoader()->loadIconSet("connect_no",KIcon::Toolbar));
	m_connected = false;
}

void FtpSession::Occupy()
{
	m_occupied = true;
}

void FtpSession::Free()
{
	m_occupied = false;
}
#include "ftpsession.moc"
