/***************************************************************************
                          kasablanca.cpp  -  description
                             -------------------
    begin                : Die Aug 19 20:54:14 CEST 2003
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

#include <kstandarddirs.h>
#include <qstatusbar.h>
#include <qtextedit.h>
#include <qlineedit.h>
#include <qcolor.h>
#include <qpixmap.h>
#include <kprocess.h>
#include <kaboutdialog.h>
#include <qmenubar.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <kglobal.h>
#include <kicontheme.h>
#include <kiconloader.h>
#include <qevent.h>
#include <qobject.h>
#include <qdir.h>
#include <qstringlist.h>
#include <qpopupmenu.h>
#include <qfileinfo.h>
#include <qheader.h>
#include <qdatetime.h>
#include <qeventloop.h>
#include <qdom.h>
#include <kmessagebox.h>
#include <klineeditdlg.h>
#include <qaction.h>
#include <kmainwindow.h>
#include <kprocio.h>
#include <vector>
#include <qtoolbutton.h>

#include "customconnectdialog.h"
#include "fileexistsdialog.h"
#include "bookmarkdialog.h"
#include "remotefileinfo.h"
#include "transferdir.h"
#include "transferfile.h"
#include "kbprocess.h"

#include "kasablanca.h"

Kasablanca::Kasablanca(QWidget *parent, const char *name) : KasablancaMainWindow(parent, name)
{
	/* when the qmainwindow is closed the object gets deleted,
	so the destructers of kbprocesses are called. */

	setWFlags(WDestructiveClose);

	menubar->insertItem("Connect To",  &m_bookmarksmenu, 1001, 2);

	if (ParseBookmarks() != 1) KMessageBox::error(0,"could not open kasablanca bookmark xml.");

	//when developing change the following line to the kbftp path
	//and disable the other two lines. */

	//m_proc.addArgument("/home/mkulke/Apps/kasablanca-0.3/debug/src/kbftp/kbftp");

	if (locate("exe", "kbftp") == QString::null) KMessageBox::error(0,"kbftp binary is not in kde binary path.");
	else m_proc.addArgument(locate("exe", "kbftp"));

   TaskView->setSorting(-1);
	BrowserA->setSorting(-1);
	BrowserB->setSorting(-1);

	mp_header_a = BrowserA->header();
	mp_header_b = BrowserB->header();
	
	/* for now the right refresh button is always on
	and the right connect button is always off. */
	
	RefreshButtonB->setEnabled(true);
	ConnectButtonB->setEnabled(false);
	
	TransferButtonA->setIconSet(KGlobal::iconLoader()->loadIconSet("forward",KIcon::Toolbar));
   TransferButtonB->setIconSet(KGlobal::iconLoader()->loadIconSet("back",KIcon::Toolbar));
	RefreshButtonA->setIconSet(KGlobal::iconLoader()->loadIconSet("reload",KIcon::Toolbar));
   RefreshButtonB->setIconSet(KGlobal::iconLoader()->loadIconSet("reload",KIcon::Toolbar));
	ConnectButtonA->setIconSet(KGlobal::iconLoader()->loadIconSet("connect_no",KIcon::Toolbar));
   ConnectButtonB->setIconSet(KGlobal::iconLoader()->loadIconSet("connect_no",KIcon::Toolbar));
	
   m_rclickmenu_a.insertItem("Transfer", 1001);
   m_rclickmenu_a.insertItem("Queue", 1002);
   m_rclickmenu_a.insertSeparator();
   m_rclickmenu_a.insertItem("Delete", 1003);
   m_rclickmenu_a.insertItem("Rename", 1004);
	m_rclickmenu_a.insertSeparator();
	m_rclickmenu_a.insertItem("Mkdir", 1005);

   m_rclickmenu_b.insertItem("Transfer", 1001);
   m_rclickmenu_b.insertItem("Queue", 1002);
   m_rclickmenu_b.insertSeparator();
   m_rclickmenu_b.insertItem("Delete", 1003);
   m_rclickmenu_b.insertItem("Rename", 1004);
	m_rclickmenu_b.insertSeparator();
	m_rclickmenu_b.insertItem("Mkdir", 1005);

   m_rclickmenu_t.insertItem("Start", 1001);
   m_rclickmenu_t.insertSeparator();
   m_rclickmenu_t.insertItem("Skip", 1002);

   UpdateLocalDisplay();

   connect( &m_bookmarksmenu, SIGNAL( activated(int) ), this, SLOT( SLOT_ConnectBookmark(int) ) );

   connect( BrowserA, SIGNAL (doubleClicked(QListViewItem*) ), this, SLOT ( SLOT_ItemClickedA(QListViewItem*) ) );
   connect( BrowserB, SIGNAL (doubleClicked(QListViewItem*) ), this, SLOT ( SLOT_ItemClickedB(QListViewItem*) ) );
   connect( BrowserB, SIGNAL (rightButtonPressed( QListViewItem *, const QPoint &, int )), this,
		SLOT (SLOT_ItemRightClickedB(QListViewItem *, const QPoint &, int )));
   connect( BrowserA, SIGNAL (rightButtonPressed( QListViewItem *, const QPoint &, int )), this,
		SLOT (SLOT_ItemRightClickedA(QListViewItem *, const QPoint &, int )));
   connect( TaskView, SIGNAL (rightButtonPressed( QListViewItem *, const QPoint &, int )), this,
		SLOT (SLOT_ItemRightClickedT(QListViewItem *, const QPoint &, int )));
	connect( mp_header_b, SIGNAL (clicked (int )), this, SLOT (SLOT_HeaderBClicked(int)));
	connect( mp_header_a, SIGNAL (clicked (int )), this, SLOT (SLOT_HeaderAClicked(int)));

   m_rclickmenu_a.connectItem(1001, this, SLOT(SLOT_TransferA()));
   m_rclickmenu_b.connectItem(1001, this, SLOT(SLOT_TransferB()));

	m_rclickmenu_a.connectItem(1002, this, SLOT(SLOT_QueueA()));
   m_rclickmenu_b.connectItem(1002, this, SLOT(SLOT_QueueB()));

	m_rclickmenu_a.connectItem(1003, this, SLOT(SLOT_DeleteA()));
	m_rclickmenu_b.connectItem(1003, this, SLOT(SLOT_DeleteB()));

	m_rclickmenu_a.connectItem(1004, this, SLOT(SLOT_RenameA()));
	m_rclickmenu_b.connectItem(1004, this, SLOT(SLOT_RenameB()));

	m_rclickmenu_a.connectItem(1005, this, SLOT(SLOT_MkdirA()));
	m_rclickmenu_b.connectItem(1005, this, SLOT(SLOT_MkdirB()));

   m_rclickmenu_t.connectItem(1001, this, SLOT(SLOT_StartQueue()));

	m_rclickmenu_t.connectItem(1002, this, SLOT(SkipTasks()));

	m_rclickmenu_a.setItemEnabled(1001, false);
	m_rclickmenu_a.setItemEnabled(1002, false);
	m_rclickmenu_a.setItemEnabled(1003, false);
	m_rclickmenu_a.setItemEnabled(1004, false);
	m_rclickmenu_a.setItemEnabled(1005, false);
	m_rclickmenu_b.setItemEnabled(1001, false);
	m_rclickmenu_b.setItemEnabled(1002, false);
	m_rclickmenu_b.setItemEnabled(1004, false);
	//m_rclickmenu_b.setItemEnabled(1005, false);
	m_rclickmenu_t.setItemEnabled(1001, false);

	m_dcount = 0;
	m_qstate = done;

	connect(&m_proc, SIGNAL(readyReadStdout()), this, SLOT(SLOT_KbftpReadReady()));

   SetGuiStatus(disconnected);
}

Kasablanca::~Kasablanca()
{
}

void Kasablanca::SLOT_UpdateB()
{
	UpdateLocalDisplay();
}

void Kasablanca::SLOT_ProcessExited(KProcess *proc)
{
	qWarning("local process exited");	

	delete proc;

	UpdateLocalDisplay();
}

void Kasablanca::SLOT_ConnectCustom()
{
   CustomConnectDialog dlg;

	m_site_a.Clear();
   dlg.mp_site = &m_site_a;

   if ((dlg.exec() == QDialog::Accepted) && (m_site_a.CheckContent()))
   {
		if (!m_proc.start())
		{
			qWarning("could not start");
			return;
		}

		if (m_site_a.GetPasv() == 1) m_proc.writeStdin("setconnmode pasv");
		else m_proc.writeStdin("setconnmode port");

		m_proc.writeStdin("connect " + QString(m_site_a.GetInfo()));
		if (m_site_a.GetTls() > 0) m_proc.writeStdin("negotiateencryption");
		m_proc.writeStdin("login " + QString(m_site_a.GetUser()) + " " + QString(m_site_a.GetPass()));

		UpdateRemote();
	}
}

void Kasablanca::SetGuiStatus(State s)
{
	switch (s)
	{
   	case connected:
			m_status = connected;

			BrowserA->setEnabled(true);
			LogWindow->setColor(white);
			TransferButtonA->setEnabled(false);
			TransferButtonB->setEnabled(false);	
			ConnectButtonA->setEnabled(true);
			ConnectButtonA->setIconSet(KGlobal::iconLoader()->loadIconSet("connect_established",KIcon::Toolbar));
			RefreshButtonA->setEnabled(true);
			CommandLineA->setEnabled(true);
			CwdLineA->setEnabled(true);
			m_bookmarksmenu.setEnabled(false);
			ActionConnectCustom->setEnabled(false);
			m_rclickmenu_t.setItemEnabled(1001, true);
			m_rclickmenu_t.setItemEnabled(1002, true);
			m_rclickmenu_a.setItemEnabled(1003, true);
			m_rclickmenu_a.setItemEnabled(1005, true);
			break;
		case disconnected:
			m_status = disconnected;

			while (QListViewItem* tmpviewitem = BrowserA->firstChild()) delete tmpviewitem;
			while (QListViewItem* tmpviewitem = TaskView->firstChild()) delete tmpviewitem;
			BrowserA->setEnabled(false);
			LogWindow->setColor(white);
			TransferButtonA->setEnabled(false);
			TransferButtonB->setEnabled(false);
			ConnectButtonA->setEnabled(false);
			ConnectButtonA->setIconSet(KGlobal::iconLoader()->loadIconSet("connect_no",KIcon::Toolbar));
			RefreshButtonA->setEnabled(false);
			CommandLineA->setEnabled(false);
			CwdLineA->setEnabled(false);
			m_bookmarksmenu.setEnabled(true);
			ActionConnectCustom->setEnabled(true);
			m_rclickmenu_t.setItemEnabled(1001, false);
			m_rclickmenu_t.setItemEnabled(1002, false);
			m_rclickmenu_a.setItemEnabled(1003, false);
			m_rclickmenu_a.setItemEnabled(1005, false);
			break;

		case occupied:
			m_status = occupied;

			BrowserA->setEnabled(false);
			LogWindow->setColor(white);
			TransferButtonA->setEnabled(false);
			TransferButtonB->setEnabled(false);
			ConnectButtonA->setEnabled(true);
			ConnectButtonA->setIconSet(KGlobal::iconLoader()->loadIconSet("connect_established",KIcon::Toolbar));
			RefreshButtonA->setEnabled(false);
			CommandLineA->setEnabled(false);
			CwdLineA->setEnabled(false);
			m_bookmarksmenu.setEnabled(false);
			ActionConnectCustom->setEnabled(false);
			m_rclickmenu_t.setItemEnabled(1001, false);
			m_rclickmenu_t.setItemEnabled(1002, true);
			m_rclickmenu_a.setItemEnabled(1003, false);
			m_rclickmenu_a.setItemEnabled(1005, false);
			break;
    }
}

void Kasablanca::SLOT_EditBookmarks()
{
	BookmarkDialog dlg;
	if (dlg.exec() == QDialog::Accepted)
	{
		ParseBookmarks();
	}
}

void Kasablanca::SLOT_ConnectBookmark(int n)
{
	m_site_a.Clear();

	m_site_a.SetInfo(m_bookmarks.at(n).GetInfo());
	m_site_a.SetName(m_bookmarks.at(n).GetName());
	m_site_a.SetPass(m_bookmarks.at(n).GetPass());
	m_site_a.SetPasv(m_bookmarks.at(n).GetPasv());
	m_site_a.SetTls(m_bookmarks.at(n).GetTls());
	m_site_a.SetUser(m_bookmarks.at(n).GetUser());

	if (!m_proc.start())
	{
		qWarning("could not start");
		return;
	}

	if (m_site_a.GetPasv() == 1) m_proc.writeStdin("setconnmode pasv");
	else m_proc.writeStdin("setconnmode port");

	m_proc.writeStdin("connect " + QString(m_site_a.GetInfo()));
	if (m_site_a.GetTls() > 0) m_proc.writeStdin("negotiateencryption");
	m_proc.writeStdin("login " + QString(m_site_a.GetUser()) + " " + QString(m_site_a.GetPass()));

	UpdateRemote();
}

void Kasablanca::SLOT_EnterCommandA()
{
	m_proc.writeStdin("raw " + CommandLineA->text());
	CommandLineA->setText("");
}

void Kasablanca::SLOT_EnterCwdA()
{
	m_proc.writeStdin("chdir " + CwdLineA->text());;
	UpdateRemote();
}

void Kasablanca::SLOT_ConnectA()
{
	if (m_status == occupied)
	{
		m_proc.tryTerminate();
		m_proc.kill();
		SetGuiStatus(disconnected);
		LogWindow->setColor(red);
		LogWindow->append("killed ftp connection");
	}
	else m_proc.writeStdin("quit");
}

void Kasablanca::SLOT_ConnectB()
{
	return;
}

int Kasablanca::ParseBookmarks()
{
	QFileInfo fi(QDir::homeDirPath() + "/.kasablanca/bookmarks.xml");
	if (fi.exists() == false)
	{
		if (QDir::home().mkdir(".kasablanca") == false) return -1;
		QDomDocument doc( "KasablancaBookmarks" );
		QDomElement root = doc.createElement( "kasablanca" );
		doc.appendChild( root );
		QFile fileout( QDir::homeDirPath() + "/.kasablanca/bookmarks.xml" );
  		if( !fileout.open( IO_WriteOnly ) ) return -1;

  		QTextStream ts( &fileout );
  		ts << doc.toString();
  		fileout.close();
	}

	QDomDocument doc( "KasablancaBookmarks" );
	QFile filein( QDir::homeDirPath() + "/.kasablanca/bookmarks.xml" );
	if( !filein.open( IO_ReadOnly ) ) return -1;
	if( !doc.setContent( &filein ) )
	{
  		filein.close();
  		return -2;
	}
	filein.close();

	m_bookmarks.clear();

	QDomElement root = doc.documentElement();
	if( root.tagName() != "kasablanca" ) return -3;

	QDomNode curNode = root.firstChild();
	while( !curNode.isNull() )
	{
		QDomElement siteElement = curNode.toElement();
		if( !siteElement.isNull() )
		{
			if( siteElement.tagName() == "site" )
			{
				siteinfo entry;

				entry.SetPasv(1);
            entry.SetTls(0);
            entry.SetName(siteElement.attribute( "name", "" ).latin1());

				QDomNode siteNode = siteElement.firstChild();
				while( !siteNode.isNull() )
				{
					QDomElement curElement = siteNode.toElement();
					if( !curElement.isNull() )
					{
						if( curElement.tagName() == "user" ) entry.SetUser(curElement.text().latin1());
						if( curElement.tagName() == "pass" ) entry.SetPass(curElement.text().latin1());
						if( curElement.tagName() == "info" ) entry.SetInfo(curElement.text().latin1());
						if( curElement.tagName() == "pasv" ) entry.SetPasv(curElement.text().toInt());
						if( curElement.tagName() == "tls" ) entry.SetTls(curElement.text().toInt());
					}
					siteNode = siteNode.nextSibling();
				}
				if (entry.CheckContent()) m_bookmarks.push_back(entry);
			}
		}
		curNode = curNode.nextSibling();
	}

	m_bookmarksmenu.clear();
	for (int i = 0; i < static_cast<int>(m_bookmarks.size()); i++)
   {
   	m_bookmarksmenu.insertItem(m_bookmarks.at(i).GetName(),i);
   }

   return 1;
}

void Kasablanca::SLOT_ItemClickedB(QListViewItem * item)
{
	if (QString::compare(item->text(0),"..") == 0)
	{
      //PrintLog("change local dir to .. ", 0);
		if (m_currentlocaldir.cdUp() == TRUE) UpdateLocalDisplay();
   }
   else if (item->rtti() == 1001)
   {
   	//PrintLog(QString("change local dir to ") + item->text(0));
      if (m_currentlocaldir.cd(item->text(0)) == TRUE) UpdateLocalDisplay();
	}
}

void Kasablanca::SLOT_ItemClickedA(QListViewItem * item)
{
	if (item->text(0) == "..") m_proc.writeStdin("cdup");
	else m_proc.writeStdin("chdir " + item->text(0));
	UpdateRemote();
}

void Kasablanca::SLOT_EnterCommandB()
{
   KProcess* p = new KProcess();

	connect( p, SIGNAL (processExited (KProcess*)), this, SLOT (SLOT_ProcessExited(KProcess*)));

 	p->setWorkingDirectory(m_currentlocaldir.absPath());

	*p << QStringList::split(" ", CommandLineB->text());

	if (p->start() == TRUE) CommandLineB->setText("");
}

void Kasablanca::UpdateLocalDisplay()
{
	const QFileInfoList * filelist;
	const QFileInfoList * dirlist;
	KIconLoader * il;

	il = KGlobal::iconLoader();

	BrowserB->sortColumn();

	m_currentlocaldir.setSorting(QDir::Name);

	m_currentlocaldir.setFilter(QDir::Dirs | QDir::Hidden);
	dirlist = m_currentlocaldir.entryInfoList();

	while (QListViewItem* tmpviewitem = BrowserB->firstChild()) delete tmpviewitem;

	QListViewItem* dirup = new QListViewItem(BrowserB, "..");
	dirup->setPixmap(0, il->loadIcon("folder",KIcon::Small));
	dirup->setSelectable(false);

	QFileInfoListIterator dit( *dirlist );
	QFileInfo * dirinfo;
	while( (dirinfo=dit.current()) != 0 )
	{
		++dit;
		if ( (dirinfo->fileName() != QString(".")) && (dirinfo->fileName() != QString("..")) )
		{
			diritem * di = new diritem(BrowserB, BrowserB->lastItem(), dirinfo->fileName(),
			m_currentlocaldir.absPath(), dirinfo->lastModified().toString("MMM dd yyyy"), dirinfo->size(),
				dirinfo->lastModified().date().year() * 10000
				+ dirinfo->lastModified().date().month() * 100
				+ dirinfo->lastModified().date().day());
			di->setPixmap(0, il->loadIcon("folder",KIcon::Small));
		}
	}

	m_currentlocaldir.setFilter(QDir::Files | QDir::Hidden);
	filelist = m_currentlocaldir.entryInfoList();

	QFileInfoListIterator fit( *filelist );
	QFileInfo * fileinfo;
	while( (fileinfo=fit.current()) != 0 )
	{
		++fit;
		fileitem * fi = new fileitem(BrowserB, BrowserB->lastItem(), fileinfo->fileName(),
			m_currentlocaldir.absPath(), fileinfo->lastModified().toString("MMM dd yyyy"), fileinfo->size(),
			fileinfo->lastModified().date().year() * 10000
			+ fileinfo->lastModified().date().month() * 100
			+ fileinfo->lastModified().date().day());
		fi->setPixmap(0, il->loadIcon("files",KIcon::Small));
	}

	CwdLineB->setText(m_currentlocaldir.absPath());

	TransferButtonB->setEnabled(false);
}

void Kasablanca::SLOT_ItemRightClickedB(QListViewItem * item, const QPoint & point, int col )
{
    m_rclickmenu_b.exec(point);
}

void Kasablanca::SLOT_ItemRightClickedA(QListViewItem * item, const QPoint & point, int col )
{
    m_rclickmenu_a.exec(point);
}

void Kasablanca::SLOT_ItemRightClickedT(QListViewItem * item, const QPoint & point, int col )
{
    m_rclickmenu_t.exec(point);
}

void Kasablanca::SkipTasks()
{
    QListViewItemIterator it(TaskView);
    while ( it.current() )
    {
        if (it.current()->isSelected()) delete it.current();
        else ++it;
    }
}

void Kasablanca::SLOT_Quit()
{
	if (m_status != disconnected)
	{
		m_proc.writeStdin("quit");
	}
	else qApp->quit();
}

void Kasablanca::SLOT_DeleteB()
{
	QListViewItemIterator it(BrowserB);
	while (it.current())
	{
		if (it.current()->isSelected())
		{
			QListViewItem* item = it.current();

			if (item->rtti() == 1001)
			{
				//qWarning("blub: %s", static_cast<diritem*>(item)->m_file.latin1());
				m_currentlocaldir.rmdir(static_cast<diritem*>(item)->m_file);
			}
			else if (item->rtti() == 1002)
			{
				QFile::remove(m_currentlocaldir.absPath() + "/" + static_cast<fileitem*>(item)->m_file);
			}
		}
		it++;
	}
	UpdateLocalDisplay();
}

void Kasablanca::SLOT_DeleteA()
{
	QListViewItemIterator it(BrowserA);
	while (it.current())
	{
		if (it.current()->isSelected())
		{
			QListViewItem* item = it.current();

			if (item->rtti() == 1001)
			{
				m_proc.writeStdin("rmdir " + static_cast<diritem*>(item)->m_file);
			}
			else if (item->rtti() == 1002)
			{
				m_proc.writeStdin("delete " + static_cast<fileitem*>(item)->m_file);
			}
		}
		it++;
	}
	UpdateRemote();
}

void Kasablanca::InsertMarkedItems(TransferMethod m, QListViewItem* begin)
{
	QListViewItem* lastitem = begin;

	/* insert all marked items at the begin of the queue,
	for every selected either a file or dir item is created. */

	QListViewItemIterator it(BrowserA);
	while ((it.current()) && (m == download))
	{
		if (it.current()->isSelected())
		{
			QListViewItem* item = it.current();

			if (item->rtti() == 1001)
			{
				QFileInfo filocal(m_currentlocaldir.absPath(), static_cast<diritem*>(item)->m_file);
				RemoteFileInfo firemote(static_cast<diritem*>(item)->m_path,
					static_cast<diritem*>(item)->m_file,
					static_cast<diritem*>(item)->m_size,
					static_cast<diritem*>(item)->m_date,
					static_cast<diritem*>(item)->m_date_int);
				lastitem = new transferdir(TaskView, lastitem, filocal, firemote, transferitem::download);
			}
			else if (item->rtti() == 1002)
			{
				QFileInfo filocal(m_currentlocaldir.absPath(), static_cast<fileitem*>(item)->m_file);
				RemoteFileInfo firemote(static_cast<fileitem*>(item)->m_path,
					static_cast<fileitem*>(item)->m_file,
					static_cast<fileitem*>(item)->m_size,
					static_cast<fileitem*>(item)->m_date,
					static_cast<fileitem*>(item)->m_date_int);
				lastitem = new transferfile(TaskView, lastitem, filocal, firemote, transferitem::download);
			}
			it.current()->setSelected(false);
		}
		++it;
	}

	QListViewItemIterator jt(BrowserB);
	while ((jt.current()) && (m == upload))
	{
		if (jt.current()->isSelected())
		{
			QListViewItem* item = jt.current();

			if (item->rtti() == 1001)
			{
				QFileInfo filocal(m_currentlocaldir.absPath(), static_cast<diritem*>(item)->m_file);
				RemoteFileInfo firemote(m_currentremotedir_a,
					static_cast<diritem*>(item)->m_file,
					static_cast<diritem*>(item)->m_size,
					static_cast<diritem*>(item)->m_date,
					static_cast<diritem*>(item)->m_date_int);
				lastitem = new transferdir(TaskView, lastitem, filocal, firemote, transferitem::upload);
			}
			else if (item->rtti() == 1002)
			{
				QFileInfo filocal(m_currentlocaldir.absPath(), static_cast<fileitem*>(item)->m_file);
				RemoteFileInfo firemote(m_currentremotedir_a,
					static_cast<diritem*>(item)->m_file,
					static_cast<diritem*>(item)->m_size,
					static_cast<diritem*>(item)->m_date,
					static_cast<diritem*>(item)->m_date_int);
				lastitem = new transferfile(TaskView, lastitem, filocal, firemote, transferitem::upload);
			}
			jt.current()->setSelected(false);
		}
		++jt;
	}
}

void Kasablanca::Xfer()
{
	/* if there's no item in the queue view left, the method ends. */

	if (TaskView->childCount() == 0)
	{
		UpdateRemote();
		UpdateLocalDisplay();
		return;
	}
	else 
	{
		TaskView->firstChild()->setSelected(false);
		TaskView->firstChild()->setSelectable(false);
	}
		
	qWarning("item: %s", TaskView->firstChild()->text(0).latin1());

	/* process first queue item */

	if (TaskView->firstChild()->rtti() == transferitem::dir)
	{
		qWarning("...is a dir");
		transferdir* dir = static_cast<transferdir*>(TaskView->firstChild());

		/* check if we're in the correct remote directory yet.
		if not issue a change to the transfer directory */

		if (dir->m_firemote.dirPath() != m_currentremotedir_a)
		{
			qWarning("in the wrong path");
			m_qstate = changedir;

			m_proc.writeStdin("chdir " + dir->m_firemote.dirPath());
			UpdateRemote();
		}
		else
		{
			/* change to the local transfer directory */

			m_currentlocaldir.cd(dir->m_filocal.dirPath());
			UpdateLocalDisplay();

			/*-------------upload/download---------------*/

			if (dir->type() == transferitem::upload)
			{
				/* the scanlocal flag is set */

				m_qstate = scanlocal;

				/* create dir on remote */

				m_proc.writeStdin("mkdir " + dir->m_firemote.fileName());

				/* change to remote dir */

				m_proc.writeStdin("chdir " + dir->m_firemote.fileName());

				/* enter dir on local */

				m_currentlocaldir.cd(dir->m_filocal.fileName());
				UpdateLocalDisplay();
			}
			else if (dir->type() == transferitem::download)
			{
				/* the scan dir flag is set */

				m_qstate = scanremote;

				/* cwd to new dir on remote.  */

				m_proc.writeStdin("chdir " + dir->m_firemote.fileName());

				/* create dir on local */

				m_currentlocaldir.mkdir(dir->m_filocal.fileName());

				/* enter dir on local */

				m_currentlocaldir.cd(dir->m_filocal.fileName());
				UpdateLocalDisplay();
			}

			/* the remote directory content gets updated for
			further processing */

			UpdateRemote();
		}
	}
	else if (TaskView->firstChild()->rtti() == transferitem::file)
	{
		qWarning("...is a file");
		transferfile* file = static_cast<transferfile*>(TaskView->firstChild());

		/* change to correct local directory */

		m_currentlocaldir.cd(file->m_filocal.dirPath());
		UpdateLocalDisplay();

		/* check if we're in the correct remote directory yet.
		if not issue a change to the transfer directory */

		if (file->m_firemote.dirPath() != CwdLineA->text())
		{
			qWarning("in the wrong path");
			m_qstate = changedir;
			m_proc.writeStdin("chdir " + file->m_firemote.dirPath());
			UpdateRemote();
		}
		else
		{
			/*-------------upload/download---------------*/

			QString remotename = file->m_firemote.fileName();
			QString localname = file->m_filocal.fileName();
			FileExistsDialog dialog;
			bool resume = false;
			bool skip = false;
			bool filepresent = false;

			if (file->type() == transferitem::upload)
			{
				uint offset = file->m_filocal.size();

				// search the whole browserA list for the same filename

				QListViewItemIterator it(BrowserA);
				while ( it.current() )
				{
					if (file->m_firemote.fileName().lower() == it.current()->text(0).lower())
					{
						filepresent = true;
						if (it.current()->rtti() == 1002)
						{
							fileitem* firemote = static_cast<fileitem*>(it.current());
							if (offset < firemote->m_size)
            				{
                				dialog.ResumeButton->setEnabled(true);
            				}
						}
						if (it.current()->rtti() == 1002) dialog.ResumeButton->setEnabled(false);
					}
					++it;
				}

				if (filepresent)
				{
					dialog.setCaption(file->m_filocal.fileName() + " exists yet...");

					switch (dialog.exec())
					{
						case 0:
							skip = false;
							break;
						case 1:
							resume = true;
							break;
						case 3:
							skip = true;
							break;
						case 4:
							bool b = false;
							while ((remotename.lower() == file->m_firemote.fileName().lower()) || (!b))
							{
								remotename = KLineEditDlg::getText("Enter New Name:", remotename + "_alt", &b, this);
							}
							break;
					}
				}

				if (skip)
				{

				}
				else
				{
					/* if tls level is 2 then disable data encryption, on level 3 enable it */

					if (m_site_a.GetTls() == 2) m_proc.writeStdin("setdataencryption off");
					else if (m_site_a.GetTls() == 3) m_proc.writeStdin("setdataencryption on");

					/* resume the file if resume is set */

					if (resume) m_proc.writeStdin("putresume " + file->m_filocal.filePath() + " "
						+ remotename + " " + QString::number(offset));
					else m_proc.writeStdin("put " + file->m_filocal.filePath() + " "
						+ remotename);
				}
			}
			else if (file->type() == transferitem::download)
			{
				uint offset = file->m_filocal.size();

				/* check if file is yet present */

				if (file->m_filocal.isFile())
				{
					if (offset < file->m_firemote.size())
					{
						dialog.ResumeButton->setEnabled(true);
					}
				}
				if (file->m_filocal.isDir())
				{
					dialog.OverwriteButton->setEnabled(false);
				}


				if ((file->m_filocal.isFile() == true) or (file->m_filocal.isDir() == true))
				{
					dialog.setCaption(file->m_filocal.fileName() + " exists yet...");
					switch (dialog.exec())
					{
					case 0:
						skip = false;
						break;
					case 1:
						resume = true;
						break;
					case 3:
						skip = true;
						break;
					case 4:
						bool b = false;
						while ((localname.lower() == file->m_firemote.fileName().lower()) || (!b))
						{
							localname = KLineEditDlg::getText("Enter New Name:", localname + "_alt", &b, this);
						}
						break;
					}
				}

				/* if skip is set the first child in the taskview gets deleted
				and Xfer is called recursive */

				if (skip)
				{
					delete TaskView->firstChild();
					Xfer();
				}
				else
				{
					/* if tls level is 2 then disable data encryption, on level 3 enable it */

					if (m_site_a.GetTls() == 2) m_proc.writeStdin("setdataencryption off");
					else if (m_site_a.GetTls() == 3) m_proc.writeStdin("setdataencryption on");

					/* resume the file if resume is set */

					if (resume) m_proc.writeStdin("getresume " + file->m_filocal.dirPath() + "/" + localname + " "
						+ remotename + " " + QString::number(offset));
					else m_proc.writeStdin("get " + file->m_filocal.dirPath() + "/" + localname + " "
						+ remotename);
				}
			}
		}
	}
}

void Kasablanca::SLOT_SelectionChanged()
{
	int counter_a = 0;
	int counter_b = 0;
	bool flag;

	QListViewItemIterator ita(BrowserA);
 	while ( ita.current() )
 	{
   	if (ita.current()->isSelected()) counter_a++;
   	++ita;
 	}

	QListViewItemIterator itb(BrowserB);
 	while ( itb.current() )
 	{
   	if (itb.current()->isSelected()) counter_b++;
   	++itb;
 	}

	flag = ((counter_a != 0) && (m_status == connected));

	m_rclickmenu_a.setItemEnabled(1001, flag);
	m_rclickmenu_a.setItemEnabled(1002, flag);
	m_rclickmenu_a.setItemEnabled(1004, (counter_a == 1));
	TransferButtonA->setEnabled(flag);
	
	flag = ((counter_b != 0) && (m_status == connected));

	m_rclickmenu_b.setItemEnabled(1001, flag);
	m_rclickmenu_b.setItemEnabled(1002, flag);
	m_rclickmenu_b.setItemEnabled(1004, (counter_b == 1));
	TransferButtonB->setEnabled(flag);
}

/** No descriptions */
void Kasablanca::SLOT_UpdateA()
{
	UpdateRemote();
}

void Kasablanca::SLOT_TransferA()
{
	InsertMarkedItems(download);
	Xfer();
}

void Kasablanca::SLOT_TransferB()
{
	InsertMarkedItems(upload);
	Xfer();
}

void Kasablanca::SLOT_QueueA() { InsertMarkedItems(download, TaskView->lastItem()); }

void Kasablanca::SLOT_QueueB() { InsertMarkedItems(upload, TaskView->lastItem()); }

void Kasablanca::SLOT_StartQueue(){ Xfer();}

void Kasablanca::SLOT_EnterCwdB()
{
    m_currentlocaldir.cd(CwdLineB->text());
    UpdateLocalDisplay();
}

/*!
    \fn Kasablanca::UpdateRemote()
 */
void Kasablanca::UpdateRemote()
{
	m_proc.writeStdin("pwd");
	if (m_site_a.GetTls() >= 2) m_proc.writeStdin("setdataencryption on");
	m_proc.writeStdin("dir " + QDir::homeDirPath() + "/.kasablanca/" + m_tempdirname + "." + QString::number(m_dcount++));
	qWarning("create dirfile");
}

void Kasablanca::SLOT_HeaderBClicked(int section)
{
	m_sortascending_b = m_sortascending_b ^ true;

	QListViewItem* x = BrowserB->firstChild();
	BrowserB->takeItem(x);

	BrowserB->setSorting(section, m_sortascending_b);
	BrowserB->sort();
	BrowserB->setSorting(-1);

	BrowserB->insertItem(x);

}

void Kasablanca::SLOT_HeaderAClicked(int section)
{
	m_sortascending_a = m_sortascending_a ^ true;

	QListViewItem* x = BrowserA->firstChild();
	BrowserA->takeItem(x);

	BrowserA->setSorting(section, m_sortascending_a);
	BrowserA->sort();
	BrowserA->setSorting(-1);

	BrowserA->insertItem(x);

}

void Kasablanca::SLOT_MkdirA()
{
	bool b;
	QString name;

	name = KLineEditDlg::getText("Enter Directory Name:", "", &b, this);
	if (!b) return;
	m_proc.writeStdin("mkdir " + name);
	UpdateRemote();
}

void Kasablanca::SLOT_MkdirB()
{
	bool b;
	QString name;

	name = KLineEditDlg::getText("Enter Directory Name:", "", &b, this);

	if (!b) return;

 	m_currentlocaldir.mkdir(name);
   UpdateLocalDisplay();
}

void Kasablanca::UpdateRemoteDisplay(QString dirfile)
{
	vector <RemoteFileInfo> fitable;
	vector <RemoteFileInfo> ditable;

	KIconLoader * il;

	il = KGlobal::iconLoader();

	BrowserA->sortColumn();

	FormatFilelist(dirfile.latin1(), m_currentremotedir_a.latin1(), &fitable, &ditable);

	// delete temporary dirfile.

	QFile::remove(QDir::homeDirPath() + "/.kasablanca/" + m_tempdirname);
	QFile::remove(dirfile);

	while (QListViewItem* tmpviewitem = BrowserA->firstChild()) delete tmpviewitem;

	QListViewItem* dirup = new QListViewItem(BrowserA, "..");
	dirup->setPixmap(0, il->loadIcon("folder",KIcon::Small));
	dirup->setSelectable(false);

	for (int i = 0; i < static_cast<int>(ditable.size()); i++)
	{
		diritem * di = new diritem(BrowserA,
			BrowserA->lastItem(),
			ditable.at(i).fileName(),
			ditable.at(i).dirPath(),
			ditable.at(i).date(),
			ditable.at(i).size(),ditable.at(i).date_int());
		di->setPixmap(0, il->loadIcon("folder",KIcon::Small));
	}

	for (int i = 0; i < static_cast<int>(fitable.size()); i++)
	{
		fileitem * fi = new fileitem(BrowserA,
			BrowserA->lastItem(),
			fitable.at(i).fileName(),
			fitable.at(i).dirPath(),
			fitable.at(i).date(),
			fitable.at(i).size(),
			fitable.at(i).date_int());
		fi->setPixmap(0, il->loadIcon("files",KIcon::Small));
	}

	// update the remote cwd line

	CwdLineA->setText(m_currentremotedir_a);
}

int Kasablanca::FormatFilelist(const char *filename, QString current, vector<RemoteFileInfo> *filetable, vector<RemoteFileInfo> *dirtable)
{
	int i, blocks, space;
	unsigned int loc, fileloc, datebegin, sizebegin;
	string buffer, filestring;

	RemoteFileInfo fi;
	QFileInfo dp;

	FILE* dirfile;

	char file[1024];

	char month[][5] = {
		"... ", "Jan ", "Feb ", "Mar ", "Apr ", "May ", "Jun ", "Jul ", "Aug ",
		"Sep ", "Oct ", "Nov ", "Dec "
	};

	int month_int;

	dirfile = fopen(filename, "r");
	if (dirfile == NULL)
	{
		qWarning("failed open dirfile");
		return 0;
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
	return 1;
}

void Kasablanca::SLOT_KbftpReadReady()
{
	QString s;

	while (m_proc.canReadLineStdout())
	{
		s = m_proc.readLineStdout();
		if (s.left(3) == "kb.")
		{
			if (s.left(10) == "kb.success")
			{
				qWarning("kb.success");
				SetGuiStatus(connected);
				LogWindow->setColor(green);
				LogWindow->append(m_log);
				m_log = "";
			}
			else if (s.left(9) == "kb.issue.")
			{
				qWarning("%s", s.latin1());

				if (s == "kb.issue.get")
				{
					m_xferallsize = static_cast<transferitem*>(TaskView->firstChild())->m_firemote.size();
					m_xferresumesize = 0;
					m_timer.start();
				}
				else if (s == "kb.issue.getresume")
				{
					m_xferallsize = static_cast<transferitem*>(TaskView->firstChild())->m_firemote.size();
					m_xferresumesize = static_cast<transferitem*>(TaskView->firstChild())->m_filocal.size();
					m_timer.start();
				}
				else if (s == "kb.issue.put")
				{
					m_xferallsize = static_cast<transferitem*>(TaskView->firstChild())->m_filocal.size();
					m_xferresumesize = 0;
					m_timer.start();
				}
				else if (s == "kb.issue.putresume")
				{
					m_xferallsize = static_cast<transferitem*>(TaskView->firstChild())->m_filocal.size();
					m_xferresumesize = static_cast<transferitem*>(TaskView->firstChild())->m_firemote.size();
					m_timer.start();
				}

				SetGuiStatus(occupied);
				LogWindow->setColor(white);
				LogWindow->append(s.remove(0, 9));
			}
			else if (s.left(10) == "kb.failure")
			{
				qWarning("kb.failure");
				LogWindow->setColor(red);
				LogWindow->append(m_log);
				m_log = "";
				if (s.left(16) == "kb.failure.fatal")
				{
					SetGuiStatus(disconnected);
				}
				else
				{
					SetGuiStatus(connected);
				}
			}
  	 		else if (s.left(7) == "kb.pwd.")
			{
				// when the pwd command was sucessful, the current remote dir is
				// set to the value after kb.pwd.

				m_currentremotedir_a = s.remove(0, 7);
			}
			else if (s.left(6) == "kb.dir")
			{
				// when the ls command was succesful, the remote view is updated.

				if (s.left(12) == "kb.dir.done.")
				{
					qWarning("dir sucessfully retrieved");
					UpdateRemoteDisplay(s.remove(0,12));

					/* when the queue flag is on all items in dir are
					selected and Xfer() is called */

					if (m_qstate > done)
					{
						if (m_qstate > changedir)
						{
							qWarning("delete item: %s", TaskView->firstChild()->text(0).latin1());
							delete TaskView->firstChild();

							if (m_qstate > proceed)
							{
								if (m_qstate == scanremote)
								{
									QListViewItemIterator it(BrowserA);
									while ( it.current() )
									{
										it.current()->setSelected(true);
										++it;
									}
									InsertMarkedItems(download);
								}
								else if (m_qstate == scanlocal)
								{
									QListViewItemIterator it(BrowserB);
									while ( it.current() )
									{
										it.current()->setSelected(true);
										++it;
									}
									InsertMarkedItems(upload);
								}
							}
						}
						m_qstate = done;
						Xfer();
					}
				}
				else
				{
					statusBar()->message(s.remove(0, 7) + "kb received");
				}
			}
			else if (s.left(7) == "kb.xfer")
			{
				if (s.left(12) == "kb.xfer.done")
				{
					/* delete current queue item */

					delete TaskView->firstChild();

					Xfer();
				}
				else
				{
					int xfered = s.remove(0, 8).toInt();

					TaskView->firstChild()->setText(1, QString::number(xfered >> 10)
						+ " of " + QString::number(m_xferallsize >> 10) + " kb transfered"
						+ "(" + QString::number(xfered / m_timer.elapsed()) + " kb/s) "
						+ "(" + QString::number(((xfered + m_xferresumesize)* 100 ) / m_xferallsize) + "%) ");

					//	kbxferinfo* x = static_cast<kbxferinfo*>(arg);
					//	int y = x->resumesize + xfered;

					//	x->item->setText(1,
					//	QString::number(y >> 10) + " of " + QString::number(x->size >> 10) + " kb transfered "
					//+ "(" + QString::number(xfered / x->time.elapsed()) + " kb/s) "
					//+ "(" + QString::number((y * 100 ) / x->size) + "%) ");

					//qApp->eventLoop()->processEvents(QEventLoop::AllEvents);



					//statusBar()->message(s.remove(0, 8) + "kb received");
				}
			}
			else if (s == "kb.quit")
			{
				SetGuiStatus(disconnected);
			}
		}
		else m_log.append(s + '\n');
	}
}

void Kasablanca::SLOT_About()
{
	QPixmap p(locate("appdata", "about.png"));
	KAboutDialog d;
	d.setLogo(p);
	d.setVersion(m_version);
	d.exec();
}

void Kasablanca::SLOT_RenameA()
{
	QListViewItem* item = 0;
	bool b;
	QString name;

	QListViewItemIterator it(BrowserA);
	while ( it.current() )
	{
		if (it.current()->isSelected())
		{
			item = it.current();
			break;
		}
		++it;
	}

	if (!item) return;

	name = KLineEditDlg::getText("Enter New Name:", item->text(0), &b, this);

	if (!b) return;

	m_proc.writeStdin("rename " + item->text(0) + " " + name);
	UpdateRemote();
}

void Kasablanca::SLOT_RenameB()
{
	QListViewItem* item = 0;
	bool b;
	QString name;

	QListViewItemIterator it(BrowserB);
	while ( it.current() )
	{
		if (it.current()->isSelected())
		{
			item = it.current();
			break;
		}
		++it;
	}

	if (!item) return;

	name = KLineEditDlg::getText("Enter New Name:", item->text(0), &b, this);

	if (!b) return;

	m_currentlocaldir.rename(item->text(0), name);

	UpdateLocalDisplay();

}
