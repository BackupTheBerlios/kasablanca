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
#include <kprocess.h>
#include <kiconloader.h>
#include <kinputdialog.h>
#include <qtextedit.h>
#include <qtoolbutton.h>
#include <qpopupmenu.h>
#include <qwidget.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qheader.h>

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
	m_sortascending = true;
		
	mp_eventhandler->SetFtpThread(mp_ftpthread); 
	mp_ftpthread->SetEventReceiver(mp_eventhandler);

	m_iconencrypted = KGlobal::iconLoader()->loadIconSet("encrypted",KIcon::Small).pixmap(QIconSet::Small,QIconSet::Normal);
   m_iconunencrypted = KGlobal::iconLoader()->loadIconSet("encrypted",KIcon::Small).pixmap(QIconSet::Small,QIconSet::Disabled);
	
	connect(mp_eventhandler, SIGNAL(ftp_log(QString, bool)), SLOT(SLOT_Log(QString, bool)));
	connect(mp_eventhandler, SIGNAL(ftp_connect(bool)), SLOT(SLOT_Connect(bool)));
	connect(mp_eventhandler, SIGNAL(ftp_login(bool)), SLOT(SLOT_Login(bool)));
	connect(mp_eventhandler, SIGNAL(ftp_xfered(int, bool)), SLOT(SLOT_Xfered(int, bool)));
	connect(mp_eventhandler, SIGNAL(ftp_quit(bool)), SLOT(SLOT_Misc(bool)));
	connect(mp_eventhandler, SIGNAL(ftp_chdir(bool)), SLOT(SLOT_Misc(bool)));
	connect(mp_eventhandler, SIGNAL(ftp_raw(bool)), SLOT(SLOT_Misc(bool)));
	connect(mp_eventhandler, SIGNAL(ftp_misc(bool)), SLOT(SLOT_Misc(bool)));
	connect(mp_eventhandler, SIGNAL(ftp_mkdir(bool)), SLOT(SLOT_Misc(bool)));
	connect(mp_eventhandler, SIGNAL(ftp_rename(bool)), SLOT(SLOT_Misc(bool)));
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

void FtpSession::SLOT_Xfered(int xfered, bool encrypted)
{
	if (encrypted) mp_encryptionicon->setPixmap(m_iconencrypted);
	else mp_encryptionicon->setPixmap(m_iconunencrypted);	
	qWarning("INFO: %d bytes transfered", xfered);
}

void FtpSession::SLOT_HeaderClicked(int section)
{
	m_sortascending = m_sortascending ^ true;

	QListViewItem* x = mp_browser->firstChild();
	mp_browser->takeItem(x);

	mp_browser->setSorting(section, m_sortascending);
	mp_browser->sort();
	mp_browser->setSorting(-1);

	mp_browser->insertItem(x);
}

void FtpSession::SLOT_ActionMenu(int i)
{
	if (Occupied()) 
	{	
		qWarning("ERROR: triggered action while occupied");
		return;
	}
	if (i == Kasablanca::Mkdir)
	{
		bool b;
		QString name = KInputDialog::getText(i18n("Enter directory name"), i18n("Enter directory name:"), "", &b);
		if (!b) return;
		if (Connected())
		{
			Occupy();
			mp_ftpthread->Mkdir(name);
			RefreshBrowser();
			mp_ftpthread->start();
		}
		else 
		{
			m_localworkingdir.mkdir(name);
			UpdateLocal();
		}
	}
	else if (i == Kasablanca::Delete)
	{	
		if (Connected())
		{
			Occupy();
			QListViewItemIterator it(mp_browser);
			while (it.current())
			{
				if (it.current()->isSelected())
				{
					kbitem* item = static_cast<kbitem*>(it.current());
					int warning = KMessageBox::warningContinueCancel(0, i18n("Delete this item?"), item->m_file);
					if (warning == KMessageBox::Continue)
					{
						if (item->rtti() == kbitem::dir) mp_ftpthread->Rmdir(item->m_file);
						else if (item->rtti() == kbitem::file) mp_ftpthread->Rm(item->m_file);
					}
				}
				it++;
			}
			RefreshBrowser();
			mp_ftpthread->start();
		}
		else 
		{
			QListViewItemIterator it(mp_browser);
			while (it.current())
			{
				if (it.current()->isSelected())
				{
					kbitem* item = static_cast<kbitem*>(it.current());
					int warning = KMessageBox::warningContinueCancel(0, i18n("Delete this item?"), item->m_file);
					if (warning == KMessageBox::Continue)
					{
						if (item->rtti() == kbitem::dir) RmdirLocal(item->m_file);
						else if (item->rtti() == kbitem::file) m_localworkingdir.remove(item->m_file);
					}
				}
				it++;
			}
			UpdateLocal();
		}
	}
	else if (i == Kasablanca::Rename)
	{
		if (Connected())
		{
			Occupy();
			QListViewItemIterator it(mp_browser);
			while (it.current())
			{
				if (it.current()->isSelected()) 
				{	
					bool b;
					kbitem* item = static_cast<kbitem*>(it.current());
					QString name = KInputDialog::getText(i18n("Enter new name"), i18n("Enter new name:"), item->m_file, &b);
					if (b) mp_ftpthread->Rename(item->m_file, name);
				}
				it++;
			}
			RefreshBrowser();
			mp_ftpthread->start();
		}
		else 
		{
			QListViewItemIterator it(mp_browser);
			while (it.current())
			{
				if (it.current()->isSelected()) 
				{	
					bool b;
					kbitem* item = static_cast<kbitem*>(it.current());
					QString name = KInputDialog::getText(i18n("Enter new name"), i18n("Enter new name:"), item->m_file, &b);
					if (b) m_localworkingdir.rename(item->text(0), name);
				}
				it++;
			}
			UpdateLocal();
		}
	}
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
	RefreshBrowser();
	mp_ftpthread->start();
}

void FtpSession::SLOT_Finish()
{
	qWarning("INFO: gui freed");
	Free();
}

void FtpSession::SLOT_ItemClicked(QListViewItem * item)
{
	if (Occupied()) 
	{	
		qWarning("ERROR: item clicked while occupied");
		return;
	}
	if (item->rtti() == kbitem::file) return;
	else if (Connected())
	{
		Occupy();
		if (item->text(0) == "..") mp_ftpthread->Cdup();
		else mp_ftpthread->Chdir(item->text(0));
		RefreshBrowser();
		mp_ftpthread->start();
	}
	else UpdateLocal(item->text(0));
}

void FtpSession::SLOT_ItemRClicked(QListViewItem *, const QPoint & point, int)
{
	mp_rclickmenu->exec(point);
}

void FtpSession::SLOT_CmdLine()
{
	if (Occupied()) 
	{	
		qWarning("ERROR: entered command while occupied");
		return;
	}
	else if (Connected())
	{
		Occupy();
		mp_ftpthread->Raw(mp_cmdline->text());
		mp_cmdline->setText("");
		mp_ftpthread->start();
	}
	else
	{
		KProcess* p = new KProcess();
 		p->setWorkingDirectory(m_localworkingdir.absPath());
		*p << QStringList::split(" ", mp_cmdline->text());
		connect(p, SIGNAL(processExited(KProcess*)), SLOT(SLOT_LocalProcessExited(KProcess*)));
		if (p->start() == TRUE) mp_cmdline->setText("");
	}
}

void FtpSession::SLOT_ConnectButton()
{
	/* this button is also an abort button */

	if (Occupied()) 
	{
		mp_logwindow->setColor(yellow);
		mp_logwindow->append(i18n("Aborted ftp operation"));
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
		qWarning("ERROR: entered cwd while occupied");
		return;
	}
	if (Connected())
	{	
		Occupy();
		mp_ftpthread->Chdir(mp_cwdline->text());
		RefreshBrowser();
		mp_ftpthread->start();
	}
	else UpdateLocal(mp_cwdline->text());		
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
	else UpdateLocal();
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
	
void FtpSession::SLOT_Misc(bool success)
{
	PrintLog(success);
}

void FtpSession::SLOT_Login(bool success)
{
	PrintLog(success);	
	if (!success) Disconnect();
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
		static_cast<Kasablanca*>(parent())->SLOT_SelectionChanged();
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
	UpdateLocal();
	mp_bookmarksmenu->setEnabled(true);
	mp_connectbutton->setIconSet(KGlobal::iconLoader()->loadIconSet("connect_no",KIcon::Toolbar));
	mp_statusline->setText(i18n("Disconnected"));
	m_connected = false;
}

void FtpSession::Occupy()
{
	mp_rclickmenu->setEnabled(false);
	mp_browser->setEnabled(false);
	mp_cmdline->setEnabled(false);
	mp_cwdline->setEnabled(false);
	mp_refreshbutton->setEnabled(false);
	mp_statusline->setText(i18n("Occupied"));
	m_occupied = true;
}

void FtpSession::Free()
{
	mp_rclickmenu->setEnabled(true);
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

void FtpSession::UpdateLocal(QString cwd)
{
	const QFileInfoList *filelist, *dirlist;

	if (cwd != "") m_localworkingdir.cd(cwd);
		
	mp_browser->sortColumn();
	m_localworkingdir.setSorting(QDir::Name);
	
	while (QListViewItem* tmpviewitem = mp_browser->firstChild()) delete tmpviewitem;

	QListViewItem* dirup = new QListViewItem(mp_browser, "..");
	dirup->setPixmap(0, KGlobal::iconLoader()->loadIcon("folder",KIcon::Small));
	dirup->setSelectable(false);

	m_localworkingdir.setFilter(QDir::Dirs | QDir::Hidden);
	dirlist = m_localworkingdir.entryInfoList();
	
	QFileInfoListIterator dit( *dirlist );
	QFileInfo * dirinfo;
	while( (dirinfo=dit.current()) != 0 )
	{
		++dit;
		if ( (dirinfo->fileName() != QString(".")) && (dirinfo->fileName() != QString("..")) )
		{
			diritem * di = new diritem(mp_browser, mp_browser->lastItem(), dirinfo->fileName(),
			m_localworkingdir.absPath(), dirinfo->lastModified().toString("MMM dd yyyy"), dirinfo->size(),
				dirinfo->lastModified().date().year() * 10000
				+ dirinfo->lastModified().date().month() * 100
				+ dirinfo->lastModified().date().day());
			di->setPixmap(0, KGlobal::iconLoader()->loadIcon("folder",KIcon::Small));
		}
	}

	m_localworkingdir.setFilter(QDir::Files | QDir::Hidden);
	filelist = m_localworkingdir.entryInfoList();

	QFileInfoListIterator fit( *filelist );
	QFileInfo * fileinfo;
	while( (fileinfo=fit.current()) != 0 )
	{
		++fit;
		fileitem * fi = new fileitem(mp_browser, mp_browser->lastItem(), fileinfo->fileName(),
			m_localworkingdir.absPath(), fileinfo->lastModified().toString("MMM dd yyyy"), fileinfo->size(),
			fileinfo->lastModified().date().year() * 10000
			+ fileinfo->lastModified().date().month() * 100
			+ fileinfo->lastModified().date().day());
		fi->setPixmap(0, KGlobal::iconLoader()->loadIcon("files",KIcon::Small));
	}
	
	mp_cwdline->setText(m_localworkingdir.absPath());
}

void FtpSession::RmdirLocal(QString dir)
{
	QStringList filelist, dirlist;
	QString olddir;
	
	olddir = m_localworkingdir.path();
	m_localworkingdir.cd(dir);
	
	filelist = m_localworkingdir.entryList("*", QDir::Files | QDir::Hidden); 
	for (QStringList::Iterator it = filelist.begin(); it != filelist.end(); ++it) m_localworkingdir.remove(*it);
	
	dirlist = m_localworkingdir.entryList("*", QDir::Dirs | QDir::Hidden);
	for (QStringList::Iterator it = dirlist.begin(); it != dirlist.end(); ++it) 
	{
		if ((*it != ".") && (*it != "..")) RmdirLocal(*it);
	}
	
	m_localworkingdir.cd(olddir);
	m_localworkingdir.rmdir(dir);
}
#include "ftpsession.moc"
