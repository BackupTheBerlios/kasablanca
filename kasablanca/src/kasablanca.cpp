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
#include <qcursor.h>
#include <kapp.h>
#include <kinputdialog.h>

#include "diritem.h"
#include "fileitem.h"
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
		
	QPopupMenu* m = new QPopupMenu();
	
	/* both bookmarkmenus are inserted into a Connect entry */
	
	menubar->insertItem("Connect",  m, 1001, 2);
	m->insertItem("Left Window",  &m_bookmarksmenu_a, 1001);
	m->insertItem("Right Window", &m_bookmarksmenu_b, 1002);
	
	/* if parsebookmarks returns false, an error box appears */
	
	if (ParseBookmarks() != 1) KMessageBox::error(0,"could not open kasablanca bookmark xml.");

	//when developing you might want to change the following line to the kbftp path
	//and disable the other two lines. */

	//m_proc_a.addArgument("kbftp/kbftp");
	//m_proc_b.addArgument("kbftp/kbftp");
	
	if (locate("exe", "kbftp") == QString::null) KMessageBox::error(0,"kbftp binary is not in kde binary path.");
	else 
	{
		m_proc_a.addArgument(locate("exe", "kbftp"));
		m_proc_b.addArgument(locate("exe", "kbftp"));
	}
	
   TaskView->setSorting(-1);
	BrowserA->setSorting(-1);
	BrowserB->setSorting(-1);

	mp_header_a = BrowserA->header();
	mp_header_b = BrowserB->header();
	
	TransferButtonA->setIconSet(KGlobal::iconLoader()->loadIconSet("forward",KIcon::Toolbar));
   TransferButtonB->setIconSet(KGlobal::iconLoader()->loadIconSet("back",KIcon::Toolbar));
	RefreshButtonA->setIconSet(KGlobal::iconLoader()->loadIconSet("reload",KIcon::Toolbar));
   RefreshButtonB->setIconSet(KGlobal::iconLoader()->loadIconSet("reload",KIcon::Toolbar));
	ConnectButtonA->setIconSet(KGlobal::iconLoader()->loadIconSet("connect_no",KIcon::Toolbar));
   ConnectButtonB->setIconSet(KGlobal::iconLoader()->loadIconSet("connect_no",KIcon::Toolbar));
	
   m_rclickmenu_a.insertItem("Transfer", Transfer);
   m_rclickmenu_a.insertItem("Queue", Queue);
   m_rclickmenu_a.insertSeparator();
   m_rclickmenu_a.insertItem("Delete", Delete);
   m_rclickmenu_a.insertItem("Rename", Rename);
	m_rclickmenu_a.insertSeparator();
	m_rclickmenu_a.insertItem("Mkdir", Mkdir);

   m_rclickmenu_b.insertItem("Transfer", Transfer);
   m_rclickmenu_b.insertItem("Queue", Queue);
   m_rclickmenu_b.insertSeparator();
   m_rclickmenu_b.insertItem("Delete", Delete);
   m_rclickmenu_b.insertItem("Rename", Rename);
	m_rclickmenu_b.insertSeparator();
	m_rclickmenu_b.insertItem("Mkdir", Mkdir);

   m_rclickmenu_t.insertItem("Start", Start);
   m_rclickmenu_t.insertSeparator();
   m_rclickmenu_t.insertItem("Skip", Skip);

   connect( &m_bookmarksmenu_a, SIGNAL( activated(int) ), this, SLOT( SLOT_ConnectA(int) ) );
	connect( &m_bookmarksmenu_b, SIGNAL( activated(int) ), this, SLOT( SLOT_ConnectB(int) ) );
	
   connect( BrowserA, SIGNAL (doubleClicked(QListViewItem*) ), this, SLOT ( SLOT_ItemClickedA(QListViewItem*) ) );
   connect( BrowserB, SIGNAL (doubleClicked(QListViewItem*) ), this, SLOT ( SLOT_ItemClickedB(QListViewItem*) ) );
  
   connect( BrowserA, SIGNAL (rightButtonPressed( QListViewItem *, const QPoint &, int )), this,
		SLOT (SLOT_ItemRightClickedA(QListViewItem *, const QPoint &, int )));
  	connect( BrowserB, SIGNAL (rightButtonPressed( QListViewItem *, const QPoint &, int )), this,
		SLOT (SLOT_ItemRightClickedB(QListViewItem *, const QPoint &, int )));
	
	connect( TaskView, SIGNAL (rightButtonPressed( QListViewItem *, const QPoint &, int )), this,
		SLOT (SLOT_ItemRightClickedT(QListViewItem *, const QPoint &, int )));
	
	connect( mp_header_b, SIGNAL (clicked (int )), this, SLOT (SLOT_HeaderBClicked(int)));
	connect( mp_header_a, SIGNAL (clicked (int )), this, SLOT (SLOT_HeaderAClicked(int)));

   m_rclickmenu_a.connectItem(Transfer, this, SLOT(SLOT_TransferA()));
   m_rclickmenu_b.connectItem(Transfer, this, SLOT(SLOT_TransferB()));

	m_rclickmenu_a.connectItem(Queue, this, SLOT(SLOT_QueueA()));
   m_rclickmenu_b.connectItem(Queue, this, SLOT(SLOT_QueueB()));

	m_rclickmenu_a.connectItem(Delete, this, SLOT(SLOT_DeleteA()));
	m_rclickmenu_b.connectItem(Delete, this, SLOT(SLOT_DeleteB()));

	m_rclickmenu_a.connectItem(Rename, this, SLOT(SLOT_RenameA()));
	m_rclickmenu_b.connectItem(Rename, this, SLOT(SLOT_RenameB()));

	m_rclickmenu_a.connectItem(Mkdir, this, SLOT(SLOT_MkdirA()));
	m_rclickmenu_b.connectItem(Mkdir, this, SLOT(SLOT_MkdirB()));

   m_rclickmenu_t.connectItem(Start, this, SLOT(SLOT_StartQueue()));

	m_rclickmenu_t.connectItem(Skip, this, SLOT(SLOT_SkipTasks()));

	m_rclickmenu_a.setItemEnabled(Transfer, false);
	m_rclickmenu_a.setItemEnabled(Queue, false);
	m_rclickmenu_a.setItemEnabled(Delete, false);
	m_rclickmenu_a.setItemEnabled(Rename, false);
	m_rclickmenu_a.setItemEnabled(Mkdir, false);
	
	m_rclickmenu_b.setItemEnabled(Transfer, false);
	m_rclickmenu_b.setItemEnabled(Queue, false);
	m_rclickmenu_b.setItemEnabled(Delete, false);
	m_rclickmenu_b.setItemEnabled(Rename, false);
	m_rclickmenu_b.setItemEnabled(Mkdir, false);
	
	m_rclickmenu_t.setItemEnabled(Start, false);
	m_rclickmenu_t.setItemEnabled(Skip, false);
	
	m_dcount = 0;
	m_qstate = done;
	
	connect(&m_proc_a, SIGNAL(readyToRead(kbprocess*)), this, SLOT(SLOT_KbftpReadReady(kbprocess*)));
	connect(&m_proc_b, SIGNAL(readyToRead(kbprocess*)), this, SLOT(SLOT_KbftpReadReady(kbprocess*)));
	
	UpdateLocalDisplay(A);
	UpdateLocalDisplay(B);
	
	SetGuiStatus(disconnected, A);
	SetGuiStatus(disconnected, B);
}

Kasablanca::~Kasablanca()
{
}

void Kasablanca::SLOT_ProcessExited(KProcess *proc)
{
	qWarning("local process exited");	

	delete proc;
}

void Kasablanca::ConnectCustom(Browser b)
{
	CustomConnectDialog dlg;
	siteinfo* site;
	kbprocess* proc;
	
	if (b == A)
	{
		site = &m_site_a;
		proc = &m_proc_a;
	}
	else
	{
		site = &m_site_b;
		proc = &m_proc_b;
	}
	
	site->Clear();
   dlg.mp_site = site;

	if (dlg.exec() == QDialog::Rejected) return; 	
   else if (site->CheckContent())
   {
		if (!proc->start())
		{
			qWarning("could not start");
			return;
		}

		if (site->GetPasv() == 1) proc->writeStdin("setconnmode pasv");
		else proc->writeStdin("setconnmode port");

		proc->writeStdin("connect " + QString(site->GetInfo()));
		if (site->GetTls() > 0) proc->writeStdin("negotiateencryption");
		proc->writeStdin("login " + QString(site->GetUser()) + " " + QString(site->GetPass()));

		UpdateRemote(b);
	}
	else KMessageBox::error(0,"that site information is not legit.");
}

void Kasablanca::SLOT_RefreshBrowserA()
{
	if (m_status_a == disconnected)
	{
		qWarning("update local a");
		UpdateLocalDisplay(A);
	}
	else if (m_status_a == connected)
	{
		qWarning("update connected a");
		UpdateRemote(A);
	}
}

void Kasablanca::SLOT_RefreshBrowserB()
{
	if (m_status_b == disconnected)
	{
		qWarning("update local b");
		UpdateLocalDisplay(B);
	}
	else if (m_status_b == connected)
	{
		qWarning("update connected b");
		UpdateRemote(B);
	}
}

void Kasablanca::SetGuiStatus(State s, Browser b)
{
	QListView* browser;
	QToolButton* transferbutton, *connectbutton, *connectbutton_other, *refreshbutton;
	QLineEdit* commandline, *cwdline;
	QPopupMenu* bookmarksmenu, *rclickmenu;

	if (b == A)
	{
		m_status_a = s;
		browser = BrowserA;
		transferbutton = TransferButtonA;
		connectbutton = ConnectButtonA;
		connectbutton_other = ConnectButtonB;
		refreshbutton = RefreshButtonA;
		commandline = CommandLineA;
		cwdline = CwdLineA;
		bookmarksmenu = &m_bookmarksmenu_a;
		rclickmenu = &m_rclickmenu_a;
	}
	else
	{
		m_status_b = s;
		browser = BrowserB;
		transferbutton = TransferButtonB;
		connectbutton = ConnectButtonB;
		connectbutton_other = ConnectButtonA;
		refreshbutton = RefreshButtonB;
		commandline = CommandLineB;
		cwdline = CwdLineB;
		bookmarksmenu = &m_bookmarksmenu_b;
		rclickmenu = &m_rclickmenu_b;
	}
	
	switch (s)
	{
   	case connected:

			browser->setEnabled(true);
			LogWindow->setColor(white);
			transferbutton->setEnabled(false);
			connectbutton->setEnabled(true);
			connectbutton_other->setEnabled(true);
			connectbutton->setIconSet(KGlobal::iconLoader()->loadIconSet("connect_established",KIcon::Toolbar));
			refreshbutton->setEnabled(true);
			commandline->setEnabled(true);
			cwdline->setEnabled(true);
			bookmarksmenu->setEnabled(false);	
			rclickmenu->setEnabled(true);
			m_rclickmenu_t.setItemEnabled(Start, true);
			m_rclickmenu_t.setItemEnabled(Skip, true);
			rclickmenu->setItemEnabled(Mkdir, true);	
			break;
			
		case disconnected:

			browser->setEnabled(true);
			LogWindow->setColor(white);
			transferbutton->setEnabled(false);
			connectbutton->setEnabled(true);
			connectbutton_other->setEnabled(true);
			connectbutton->setIconSet(KGlobal::iconLoader()->loadIconSet("connect_no",KIcon::Toolbar));
			refreshbutton->setEnabled(true);
			commandline->setEnabled(true);
			cwdline->setEnabled(true);
			bookmarksmenu->setEnabled(true);
			rclickmenu->setEnabled(true);
			m_rclickmenu_t.setItemEnabled(Start, false);
			m_rclickmenu_t.setItemEnabled(Skip, false);
			rclickmenu->setItemEnabled(Mkdir, true);
			break;

		case occupied:
		
			browser->setEnabled(false);
			LogWindow->setColor(white);
			transferbutton->setEnabled(false);
			connectbutton->setEnabled(true);
			connectbutton_other->setEnabled(false);
			connectbutton->setIconSet(KGlobal::iconLoader()->loadIconSet("connect_established",KIcon::Toolbar));
			refreshbutton->setEnabled(false);
			commandline->setEnabled(false);
			cwdline->setEnabled(false);
			bookmarksmenu->setEnabled(false);
			m_rclickmenu_t.setItemEnabled(Start, false);
			m_rclickmenu_t.setItemEnabled(Skip, true);
			rclickmenu->setEnabled(false);
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

void Kasablanca::ConnectBookmark(int n, Browser b)
{
	siteinfo* site;
	kbprocess* proc;

	if (b == A)
	{
		site = &m_site_a;
		proc = &m_proc_a;
	}
	else
	{
		site = &m_site_b;
		proc = &m_proc_b;
	}
		
	site->Clear();

	site->SetInfo(m_bookmarks.at(n).GetInfo());
	site->SetName(m_bookmarks.at(n).GetName());
	site->SetPass(m_bookmarks.at(n).GetPass());
	site->SetPasv(m_bookmarks.at(n).GetPasv());
	site->SetTls(m_bookmarks.at(n).GetTls());
	site->SetUser(m_bookmarks.at(n).GetUser());

	if (!proc->start())
	{
		qWarning("could not start");
		return;
	}

	if (site->GetPasv() == 1) proc->writeStdin("setconnmode pasv");
	else proc->writeStdin("setconnmode port");

	proc->writeStdin("connect " + QString(site->GetInfo()));
	if (site->GetTls() > 0) proc->writeStdin("negotiateencryption");
	proc->writeStdin("login " + QString(site->GetUser()) + " " + QString(site->GetPass()));
	
	UpdateRemote(b);
}

void Kasablanca::SLOT_EnterCommandA()
{
	if (m_status_a == connected)
	{
		m_proc_a.writeStdin("raw " + CommandLineA->text());
		CommandLineA->setText("");
	}
	else if (m_status_a == disconnected)
	{
		KProcess* p = new KProcess();
		connect( p, SIGNAL(processExited (KProcess*)), this, SLOT(SLOT_ProcessExited(KProcess*)));
 		p->setWorkingDirectory(m_currentlocaldir_a.absPath());
		*p << QStringList::split(" ", CommandLineA->text());
		if (p->start() == TRUE) CommandLineA->setText("");
	}
}

void Kasablanca::SLOT_EnterCwdA()
{
	if (m_status_a == connected)
	{
		m_proc_a.writeStdin("chdir " + CwdLineA->text());;
		UpdateRemote(A);
	}
	else if (m_status_a == disconnected)
	{
		m_currentlocaldir_a.cd(CwdLineA->text());
    	UpdateLocalDisplay(A);
	}
}

void Kasablanca::SLOT_ConnectButtonA()
{
	/* on connection changes the task view has to be cleared */

	while (QListViewItem* tmpviewitem = TaskView->firstChild()) delete tmpviewitem;

	if (m_status_a == disconnected)
	{
		m_bookmarksmenu_a.exec(QCursor::pos());
	}
	else if (m_status_a == occupied)
	{
		m_proc_a.tryTerminate();
		m_proc_a.kill();
		SetGuiStatus(disconnected, A);
		LogWindow->setColor(red);
		LogWindow->append("killed ftp connection");
	}
	else m_proc_a.writeStdin("quit");
}

void Kasablanca::SLOT_ConnectA(int i)
{	
	/* if the identifier is 0 call the Custom Connect Dialog,
	otherwise connect to a bookmark. */

	if (i == 0) ConnectCustom(A);
	else ConnectBookmark(i-1, A);
}

void Kasablanca::SLOT_ConnectButtonB()
{
	/* on connection changes the task view has to be cleared */

	while (QListViewItem* tmpviewitem = TaskView->firstChild()) delete tmpviewitem;

	if (m_status_b == disconnected)
	{
		m_bookmarksmenu_b.exec(QCursor::pos());
	}
	else if (m_status_b == occupied)
	{
		m_proc_b.tryTerminate();
		m_proc_b.kill();
		SetGuiStatus(disconnected, B);
		LogWindow->setColor(red);
		LogWindow->append("killed ftp connection");
	}
	else m_proc_b.writeStdin("quit");
}

void Kasablanca::SLOT_ConnectB(int i)
{
	qWarning("slot: SLOT_ConnectB(%d)", i);
	/* if the identifier is 0 call the Custom Connect Dialog,
	otherwise connect to a bookmark. */

	if (i == 0) ConnectCustom(B);
	else ConnectBookmark(i-1, B);
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

	m_bookmarksmenu_a.clear();
	m_bookmarksmenu_b.clear();
	
	m_bookmarksmenu_a.insertItem("Custom",0);
	m_bookmarksmenu_b.insertItem("Custom",0);
	
	m_bookmarksmenu_a.insertSeparator();
	m_bookmarksmenu_b.insertSeparator();
	
	for (int i = 0; i < static_cast<int>(m_bookmarks.size()); i++)
   {
		m_bookmarksmenu_a.insertItem(m_bookmarks.at(i).GetName(),i + 1);
   	m_bookmarksmenu_b.insertItem(m_bookmarks.at(i).GetName(),i + 1);
   }

   return 1;
}

void Kasablanca::SLOT_ItemClickedB(QListViewItem * item)
{
	if (m_status_b == connected)
	{
		if (item->text(0) == "..") m_proc_b.writeStdin("cdup");
		else m_proc_b.writeStdin("chdir " + item->text(0));
		UpdateRemote(B);
	}
	else if (m_status_b == disconnected)
	{
		if (item->text(0) == "..")
		{
			if (m_currentlocaldir_b.cdUp() == TRUE) UpdateLocalDisplay(B);
   	}
   	else if (item->rtti() == 1001)
   	{
      	if (m_currentlocaldir_b.cd(item->text(0)) == TRUE) UpdateLocalDisplay(B);
		}
	}		
}

void Kasablanca::SLOT_ItemClickedA(QListViewItem * item)
{
	/* when the status is connected, changedir events for remote
	are processed, if the status is disconnected the same happens
	for local. */

	if (m_status_a == connected)
	{
		if (item->text(0) == "..") m_proc_a.writeStdin("cdup");
		else m_proc_a.writeStdin("chdir " + item->text(0));
		UpdateRemote(A);
	}
	else if (m_status_a == disconnected)
	{
		if (item->text(0) == "..")
		{
			if (m_currentlocaldir_a.cdUp() == TRUE) UpdateLocalDisplay(A);
   	}
   	else if (item->rtti() == 1001)
   	{
      	if (m_currentlocaldir_a.cd(item->text(0)) == TRUE) UpdateLocalDisplay(A);
		}
	}		
}

void Kasablanca::SLOT_EnterCommandB()
{
	if (m_status_b == connected)
	{
		m_proc_b.writeStdin("raw " + CommandLineB->text());
		CommandLineB->setText("");
	}
	else if (m_status_b == disconnected)
	{
		KProcess* p = new KProcess();
		connect( p, SIGNAL(processExited (KProcess*)), this, SLOT(SLOT_ProcessExited(KProcess*)));
 		p->setWorkingDirectory(m_currentlocaldir_b.absPath());
		*p << QStringList::split(" ", CommandLineB->text());
		if (p->start() == TRUE) CommandLineB->setText("");
	}
}

void Kasablanca::UpdateLocalDisplay(Browser x)
{
	QListView* browser;
	const QFileInfoList * filelist;
	const QFileInfoList * dirlist;
	QDir* localdir;
	QToolButton* transferbutton;
	QLineEdit* cwdline;
	
	if (x == A) 
	{
		browser = BrowserA;
		localdir = &m_currentlocaldir_a;
		transferbutton = TransferButtonA;
		cwdline = CwdLineA;
	}
	else 
	{
		browser = BrowserB;
		localdir = &m_currentlocaldir_b;
		transferbutton = TransferButtonB;
		cwdline = CwdLineB;
	}

	browser->sortColumn();

	localdir->setSorting(QDir::Name);

	localdir->setFilter(QDir::Dirs | QDir::Hidden);
	dirlist = localdir->entryInfoList();

	while (QListViewItem* tmpviewitem = browser->firstChild()) delete tmpviewitem;

	QListViewItem* dirup = new QListViewItem(browser, "..");
	dirup->setPixmap(0, KGlobal::iconLoader()->loadIcon("folder",KIcon::Small));
	dirup->setSelectable(false);

	QFileInfoListIterator dit( *dirlist );
	QFileInfo * dirinfo;
	while( (dirinfo=dit.current()) != 0 )
	{
		++dit;
		if ( (dirinfo->fileName() != QString(".")) && (dirinfo->fileName() != QString("..")) )
		{
			diritem * di = new diritem(browser, browser->lastItem(), dirinfo->fileName(),
			localdir->absPath(), dirinfo->lastModified().toString("MMM dd yyyy"), dirinfo->size(),
				dirinfo->lastModified().date().year() * 10000
				+ dirinfo->lastModified().date().month() * 100
				+ dirinfo->lastModified().date().day());
			di->setPixmap(0, KGlobal::iconLoader()->loadIcon("folder",KIcon::Small));
		}
	}

	localdir->setFilter(QDir::Files | QDir::Hidden);
	filelist = localdir->entryInfoList();

	QFileInfoListIterator fit( *filelist );
	QFileInfo * fileinfo;
	while( (fileinfo=fit.current()) != 0 )
	{
		++fit;
		fileitem * fi = new fileitem(browser, browser->lastItem(), fileinfo->fileName(),
			localdir->absPath(), fileinfo->lastModified().toString("MMM dd yyyy"), fileinfo->size(),
			fileinfo->lastModified().date().year() * 10000
			+ fileinfo->lastModified().date().month() * 100
			+ fileinfo->lastModified().date().day());
		fi->setPixmap(0, KGlobal::iconLoader()->loadIcon("files",KIcon::Small));
	}

	cwdline->setText(localdir->absPath());
	transferbutton->setEnabled(false);
}

void Kasablanca::SLOT_ItemRightClickedB(QListViewItem *, const QPoint & point, int)
{
	qWarning("slot: SLOT_ItemRightClickedB()");
	m_rclickmenu_b.exec(point);
}

void Kasablanca::SLOT_ItemRightClickedA(QListViewItem *, const QPoint & point, int)
{
	qWarning("slot: SLOT_ItemRightClickedA()");
	m_rclickmenu_a.exec(point);
}

void Kasablanca::SLOT_ItemRightClickedT(QListViewItem *, const QPoint & point, int)
{
	qWarning("slot: SLOT_ItemRightClickedT()");
	m_rclickmenu_t.exec(point);
}

void Kasablanca::SLOT_SkipTasks()
{
    QListViewItemIterator it(TaskView);
    while ( it.current() )
    {
        if (it.current()->isSelected()) delete it.current();
        else ++it;
    }
}

void Kasablanca::SLOT_Close()
{
	close();
}

void Kasablanca::closeEvent( QCloseEvent * e )
{
	if (m_status_a != disconnected)
	{
		m_proc_a.writeStdin("quit");
	}
	if (m_status_b != disconnected)
	{
		m_proc_b.writeStdin("quit");
	}
	
	if ((m_status_a == disconnected) && (m_status_b == disconnected)) e->accept();
}

void Kasablanca::SLOT_DeleteB()
{
	if (m_status_b == connected)
	{
		QListViewItemIterator it(BrowserB);
		while (it.current())
		{
			if (it.current()->isSelected())
			{
				QListViewItem* item = it.current();
	
				if (item->rtti() == 1001)
				{
					m_proc_b.writeStdin("rmdir " + static_cast<diritem*>(item)->m_file);
				}
				else if (item->rtti() == 1002)
				{
					m_proc_b.writeStdin("delete " + static_cast<fileitem*>(item)->m_file);
				}
			}
			it++;
		}
		UpdateRemote(B);
	}
	else if (m_status_b == disconnected)
	{
		QListViewItemIterator it(BrowserB);
		while (it.current())
		{
			if (it.current()->isSelected())
			{
				QListViewItem* item = it.current();
	
				if (item->rtti() == 1001)
				{
					m_currentlocaldir_b.rmdir(static_cast<diritem*>(item)->m_file);
				}
				else if (item->rtti() == 1002)
				{
					QFile::remove(m_currentlocaldir_b.absPath() + "/" + static_cast<fileitem*>(item)->m_file);
				}
			}
			it++;
		}
		UpdateLocalDisplay(B);	
	}
}

void Kasablanca::SLOT_DeleteA()
{
	if (m_status_a == connected)
	{
		QListViewItemIterator it(BrowserA);
		while (it.current())
		{
			if (it.current()->isSelected())
			{
				QListViewItem* item = it.current();
	
				if (item->rtti() == 1001)
				{
					m_proc_a.writeStdin("rmdir " + static_cast<diritem*>(item)->m_file);
				}
				else if (item->rtti() == 1002)
				{
					m_proc_a.writeStdin("delete " + static_cast<fileitem*>(item)->m_file);
				}
			}
			it++;
		}
		UpdateRemote(A);
	}
	else if (m_status_a == disconnected)
	{
		QListViewItemIterator it(BrowserA);
		while (it.current())
		{
			if (it.current()->isSelected())
			{
				QListViewItem* item = it.current();
	
				if (item->rtti() == 1001)
				{
					m_currentlocaldir_a.rmdir(static_cast<diritem*>(item)->m_file);
				}
				else if (item->rtti() == 1002)
				{
					QFile::remove(m_currentlocaldir_a.absPath() + "/" + static_cast<fileitem*>(item)->m_file);
				}
			}
			it++;
		}
		UpdateLocalDisplay(A);	
	}
}

void Kasablanca::InsertMarkedItems(transferitem::transfertype t, QListViewItem* begin)
{
	QListViewItem* lastitem = begin;

	switch (t)
	{
		case transferitem::download_a_to_b:
		{
			QListViewItemIterator it(BrowserA);
			while (it.current())
			{	
				if (it.current()->isSelected())
				{
					QListViewItem* item = it.current();

					QFileInfo filocal(m_currentlocaldir_b.absPath(), static_cast<diritem*>(item)->m_file);
					RemoteFileInfo firemote(static_cast<kbitem*>(item)->m_path,
						static_cast<diritem*>(item)->m_file,
						static_cast<diritem*>(item)->m_size,
						static_cast<diritem*>(item)->m_date,
						static_cast<diritem*>(item)->m_date_int);
					
					if (item->rtti() == 1001)
					{	
						qWarning("download a directory from a to b...");
						lastitem = new transferdir(TaskView, lastitem, filocal, firemote, t);
					}
					else if (item->rtti() == 1002)
					{
						qWarning("download a file from a to b...");
						lastitem = new transferfile(TaskView, lastitem, filocal, firemote, t);
					}
					item->setSelected(false);	
				}
				++it;
			}
			break;
		}
		case transferitem::download_b_to_a:
		{
			QListViewItemIterator it(BrowserB);
			while (it.current())
			{	
				if (it.current()->isSelected())
				{
					QListViewItem* item = it.current();

					QFileInfo filocal(m_currentlocaldir_a.absPath(), static_cast<diritem*>(item)->m_file);
					RemoteFileInfo firemote(static_cast<kbitem*>(item)->m_path,
						static_cast<diritem*>(item)->m_file,
						static_cast<diritem*>(item)->m_size,
						static_cast<diritem*>(item)->m_date,
						static_cast<diritem*>(item)->m_date_int);
					
					if (item->rtti() == 1001)
					{	
						qWarning("download a directory from b to a...");
						lastitem = new transferdir(TaskView, lastitem, filocal, firemote, t);
					}
					else if (item->rtti() == 1002)
					{
						qWarning("download a file from b to a...");
						lastitem = new transferfile(TaskView, lastitem, filocal, firemote, t);
					}
					item->setSelected(false);	
				}
				++it;
			}
			break;
		}
		case transferitem::upload_a_to_b:
		{
			QListViewItemIterator it(BrowserA);
			while (it.current())
			{	
				if (it.current()->isSelected())
				{
					QListViewItem* item = it.current();

					QFileInfo filocal(m_currentlocaldir_a.absPath(), static_cast<kbitem*>(item)->m_file);
					RemoteFileInfo firemote(m_currentremotedir_b,
						static_cast<diritem*>(item)->m_file,
						static_cast<diritem*>(item)->m_size,
						static_cast<diritem*>(item)->m_date,
						static_cast<diritem*>(item)->m_date_int);
					
					if (item->rtti() == 1001)
					{	
						qWarning("upload a directory from a to b...");
						lastitem = new transferdir(TaskView, lastitem, filocal, firemote, t);
					}
					else if (item->rtti() == 1002)
					{
						qWarning("upload a file from a to b...");
						lastitem = new transferfile(TaskView, lastitem, filocal, firemote, t);
					}
					item->setSelected(false);	
				}
				++it;
			}	
			break;
		}
		case transferitem::upload_b_to_a:
		{
			QListViewItemIterator it(BrowserB);
			while (it.current())
			{	
				if (it.current()->isSelected())
				{
					QListViewItem* item = it.current();

					QFileInfo filocal(m_currentlocaldir_b.absPath(), static_cast<kbitem*>(item)->m_file);
					RemoteFileInfo firemote(m_currentremotedir_a,
						static_cast<diritem*>(item)->m_file,
						static_cast<diritem*>(item)->m_size,
						static_cast<diritem*>(item)->m_date,
						static_cast<diritem*>(item)->m_date_int);
					
					if (item->rtti() == 1001)
					{	
						qWarning("upload a directory from b to a...");
						lastitem = new transferdir(TaskView, lastitem, filocal, firemote, t);
					}
					else if (item->rtti() == 1002)
					{
						qWarning("upload a file from b to a...");
						lastitem = new transferfile(TaskView, lastitem, filocal, firemote, t);
					}
					item->setSelected(false);	
				}
				++it;
			}	
			break;
		}
	}	
}

void Kasablanca::Xfer()
{
	kbprocess* proc;
	QString remotedir;
	QDir* localdir;
	Browser remotebrowser, localbrowser;
	siteinfo* site;
	QListView* remoteview;

	if (TaskView->childCount() == 0)
	{
		SLOT_RefreshBrowserA();
		SLOT_RefreshBrowserB();
		return;
	}
	else 
	{
		TaskView->firstChild()->setSelected(false);
		TaskView->firstChild()->setSelectable(false);
	}

	transferitem* item = static_cast<transferitem*>(TaskView->firstChild());
			
	qWarning("item: %s", item->text(0).latin1());
	
	if ((item->type() == transferitem::download_a_to_b) || (item->type() == transferitem::upload_b_to_a))
	{
		site = &m_site_a;
		proc = &m_proc_a;
		remotebrowser = A;
		localbrowser = B;	
		remoteview = BrowserA;
		remotedir = m_currentremotedir_a;
		localdir = &m_currentlocaldir_b;
	}
	else if ((item->type() == transferitem::download_b_to_a) || (item->type() == transferitem::upload_a_to_b))
	{
		site = &m_site_b;
		proc = &m_proc_b;
		remotebrowser = B;
		localbrowser = A;	
		remoteview = BrowserB;
		remotedir = m_currentremotedir_b;
		localdir = &m_currentlocaldir_a;
	}
	else return; /* shpuld not happen! */
	
	/* process first queue item */

	if (TaskView->firstChild()->rtti() == transferitem::dir)
	{
		qWarning("...is a dir");
		transferdir* dir = static_cast<transferdir*>(TaskView->firstChild());

		/* check if we're in the correct remote directory yet.
		if not issue a change to the transfer directory */

		if (dir->m_firemote.dirPath() != remotedir)
		{
			qWarning("in the wrong path");
			m_qstate = changedir;

			proc->writeStdin("chdir " + dir->m_firemote.dirPath());
			UpdateRemote(remotebrowser);
		}
		else
		{
			/* change to the local transfer directory */

			localdir->cd(dir->m_filocal.dirPath());
			UpdateLocalDisplay(localbrowser);

			/*-------------upload/download---------------*/

			if ((dir->type() == transferitem::upload_a_to_b) || (dir->type() == transferitem::upload_b_to_a))
			{
				/* the scanlocal flag is set */

				m_qstate = scanlocal;

				/* create dir on remote */

				proc->writeStdin("mkdir " + dir->m_firemote.fileName());

				/* change to remote dir */

				proc->writeStdin("chdir " + dir->m_firemote.fileName());

				/* enter dir on local */

				localdir->cd(dir->m_filocal.fileName());
				UpdateLocalDisplay(localbrowser);
			}
			else if ((dir->type() == transferitem::download_a_to_b) || (dir->type() == transferitem::download_b_to_a))
			{
				/* the scan dir flag is set */

				m_qstate = scanremote;

				/* cwd to new dir on remote.  */

				proc->writeStdin("chdir " + dir->m_firemote.fileName());

				/* create dir on local */

				localdir->mkdir(dir->m_filocal.fileName());

				/* enter dir on local */

				localdir->cd(dir->m_filocal.fileName());
				UpdateLocalDisplay(localbrowser);
			}

			/* the remote directory content gets updated for
			further processing */

			UpdateRemote(remotebrowser);
		}	
	}
	else if (TaskView->firstChild()->rtti() == transferitem::file)
	{
		qWarning("...is a file");
		transferfile* file = static_cast<transferfile*>(TaskView->firstChild());

		/* change to correct local directory */

		localdir->cd(file->m_filocal.dirPath());
		UpdateLocalDisplay(localbrowser);

		/* check if we're in the correct remote directory yet.
		if not issue a change to the transfer directory */

		if (remotedir != file->m_firemote.dirPath())
		{
			qWarning("path is '%s' and should be '%s'.", remotedir.latin1(), file->m_firemote.dirPath().latin1());
			m_qstate = changedir;
			proc->writeStdin("chdir " + file->m_firemote.dirPath());
			UpdateRemote(remotebrowser);
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

			if ((file->type() == transferitem::upload_a_to_b) || (file->type() == transferitem::upload_b_to_a))
			{
				uint offset = file->m_filocal.size();

				// search the whole remote browser list for the same filename

				QListViewItemIterator it(remoteview);
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
							
							/* a new name is entered and checked if it exists yet,
							if so, it's looped. */
							
							while ((filepresent) || (!b))
							{
								remotename = KInputDialog::getText("Enter New Name:", "Enter New Name:", remotename + "_alt", &b, this);
								filepresent = false;
												
								QListViewItemIterator it(remoteview);
								while ( it.current() )
								{
									if (remotename.lower() == it.current()->text(0).lower())
									{
										filepresent = true;
									}
									++it;
								}
							}
							break;
					}
				}
				if (skip)
				{
					delete TaskView->firstChild();
					Xfer();
				}
				else
				{
					/* if tls level is 2 then disable data encryption, on level 3 enable it */

					if (site->GetTls() == 2) proc->writeStdin("setdataencryption off");
					else if (site->GetTls() == 3) proc->writeStdin("setdataencryption on");

					/* resume the file if resume is set */

					if (resume) proc->writeStdin("putresume " + file->m_filocal.filePath() + " "
						+ remotename + " " + QString::number(offset));
					else proc->writeStdin("put " + file->m_filocal.filePath() + " "
						+ remotename);
				}
			}
			else if ((file->type() == transferitem::download_a_to_b) || (file->type() == transferitem::download_b_to_a))
			{
				uint offset = file->m_filocal.size();
				bool filepresent;
				
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
					filepresent = true;
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
						
						while ((filepresent) or (!b))
						//while ((localname.lower() == file->m_firemote.fileName().lower()) || (!b))
						{
							localname = KInputDialog::getText("Enter New Name:", "Enter New Name:", localname + "_alt", &b, this);
							filepresent = false;
							QFileInfo fi(file->m_filocal.dirPath() + "/" + localname);
							if ((fi.isFile() == true) or (fi.isDir() == true)) filepresent = true;
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

					if (site->GetTls() == 2) proc->writeStdin("setdataencryption off");
					else if (site->GetTls() == 3) proc->writeStdin("setdataencryption on");

					/* resume the file if resume is set */

					if (resume) proc->writeStdin("getresume " + file->m_filocal.dirPath() + "/" + localname + " "
						+ remotename + " " + QString::number(offset));
					else proc->writeStdin("get " + file->m_filocal.dirPath() + "/" + localname + " "
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

	flag = ((counter_a != 0) && ((m_status_a == connected) || (m_status_b == connected)));

	m_rclickmenu_a.setItemEnabled(Transfer, flag);
	m_rclickmenu_a.setItemEnabled(Queue, flag);
	TransferButtonA->setEnabled(flag);
	
	/* when exactly 1 item is selected then rename is enabled,
	when at least one item is selected, Delete is enabled. */
	
	m_rclickmenu_a.setItemEnabled(Rename, (counter_a == 1));
	m_rclickmenu_a.setItemEnabled(Delete, (counter_a >= 1));
	
	flag = ((counter_b != 0) && ((m_status_a == connected) || (m_status_b == connected)));

	m_rclickmenu_b.setItemEnabled(Transfer, flag);
	m_rclickmenu_b.setItemEnabled(Queue, flag);
	TransferButtonB->setEnabled(flag);
	
	/* when exactly 1 item is selected then rename is enabled,
	when at least one item is selected, Delete is enabled. */
	
	m_rclickmenu_b.setItemEnabled(Rename, (counter_b == 1));
	m_rclickmenu_b.setItemEnabled(Delete, (counter_b >= 1));
}

void Kasablanca::SLOT_TransferA()
{
	qWarning("slot: SLOT_TransferA()");

	if ((m_status_a == connected) && (m_status_b == disconnected))
	{
		InsertMarkedItems(transferitem::download_a_to_b);
		Xfer();
	}
	else if ((m_status_a == disconnected) && (m_status_b == connected))
	{
		InsertMarkedItems(transferitem::upload_a_to_b);
		Xfer();
	}
	else if ((m_status_a == connected) && (m_status_b == connected))
	{
		KMessageBox::error(0,"site to site transfer (fxp) is not supported yet.");
	}
}

void Kasablanca::SLOT_TransferB()
{
	qWarning("slot: SLOT_TransferB()");

	if ((m_status_a == connected) && (m_status_b == disconnected))
	{
		InsertMarkedItems(transferitem::upload_b_to_a);
		Xfer();
	}
	else if ((m_status_a == disconnected) && (m_status_b == connected))
	{
		InsertMarkedItems(transferitem::download_b_to_a);
		Xfer();
	}
	else if ((m_status_a == connected) && (m_status_b == connected))
	{
		KMessageBox::error(0,"site to site transfer (fxp) is not supported yet.");
	}
}

void Kasablanca::SLOT_QueueA() 
{ 
	qWarning("slot: SLOT_QueueA()");

	if ((m_status_a == connected) && (m_status_b == disconnected))
	{
		InsertMarkedItems(transferitem::download_a_to_b, TaskView->lastItem()); 
	}
	else if ((m_status_a == disconnected) && (m_status_b == connected))
	{
		InsertMarkedItems(transferitem::upload_a_to_b, TaskView->lastItem()); 
	}
	else if ((m_status_a == connected) && (m_status_b == connected))
	{
		KMessageBox::error(0,"site to site transfer (fxp) is not supported yet.");
	}
}

void Kasablanca::SLOT_QueueB() 
{ 	
	qWarning("slot: SLOT_QueueB()");

	if ((m_status_a == connected) && (m_status_b == disconnected))
	{
		InsertMarkedItems(transferitem::upload_b_to_a, TaskView->lastItem());
	}
	else if ((m_status_a == disconnected) && (m_status_b == connected))
	{
		InsertMarkedItems(transferitem::download_b_to_a, TaskView->lastItem());
	}	
	else if ((m_status_a == connected) && (m_status_b == connected))
	{
		KMessageBox::error(0,"site to site transfer (fxp) is not supported yet.");
	}
}

void Kasablanca::SLOT_StartQueue(){ Xfer();}

void Kasablanca::SLOT_EnterCwdB()
{
	if (m_status_b == connected)
	{
		m_proc_b.writeStdin("chdir " + CwdLineB->text());;
		UpdateRemote(B);
	}
	else if (m_status_b == disconnected)
	{
		m_currentlocaldir_b.cd(CwdLineB->text());
    	UpdateLocalDisplay(B);
	}
}

void Kasablanca::UpdateRemote(Browser b)
{
	kbprocess* proc;
	siteinfo* site;
	
	if (b == A) 
	{
		proc = &m_proc_a;
		site = &m_site_a;
	}
	else 
	{
		proc = &m_proc_b;
		site = &m_site_b;
	}
		
	proc->writeStdin("pwd");
	if (site->GetTls() >= 2) proc->writeStdin("setdataencryption on");
	proc->writeStdin("dir " + QDir::homeDirPath() + "/.kasablanca/" + m_tempdirname + "." + QString::number(m_dcount++));
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

	name = KInputDialog::getText("Enter Directory Name:", "Enter Directory Name:", "", &b, this);
	
	if (m_status_a == connected)
	{		
		if (!b) return;
		m_proc_a.writeStdin("mkdir " + name);
		UpdateRemote(A);
	}
	if (m_status_a == disconnected)
	{
		if (!b) return;
 		m_currentlocaldir_a.mkdir(name);
   	UpdateLocalDisplay(A);
	}
}

void Kasablanca::SLOT_MkdirB()
{
	bool b;
	QString name;

	name = KInputDialog::getText("Enter Directory Name:", "Enter Directory Name:", "", &b, this);

	if (m_status_b == connected)
	{
		if (!b) return;
		m_proc_b.writeStdin("mkdir " + name);
		UpdateRemote(B);
	}
	if (m_status_b == disconnected)
	{
		if (!b) return;
 		m_currentlocaldir_b.mkdir(name);
   	UpdateLocalDisplay(B);
	}
}

void Kasablanca::UpdateRemoteDisplay(QString dirfile, Browser b)
{
	vector <RemoteFileInfo> fitable;
	vector <RemoteFileInfo> ditable;
	KIconLoader * il;
	QListView* browser;
	QString remotedir;
	QLineEdit* cwdline;

	if (b == A)
	{
		cwdline = CwdLineA;
		browser = BrowserA;
		remotedir = m_currentremotedir_a;
	}
	else
	{
		cwdline = CwdLineB;
		browser = BrowserB;
		remotedir = m_currentremotedir_b;
	}
	
	il = KGlobal::iconLoader();

	browser->sortColumn();

	FormatFilelist(dirfile.latin1(), remotedir.latin1(), &fitable, &ditable);

	// delete temporary dirfile.

	QFile::remove(QDir::homeDirPath() + "/.kasablanca/" + m_tempdirname);
	QFile::remove(dirfile);

	while (QListViewItem* tmpviewitem = browser->firstChild()) delete tmpviewitem;

	QListViewItem* dirup = new QListViewItem(browser, "..");
	dirup->setPixmap(0, il->loadIcon("folder",KIcon::Small));
	dirup->setSelectable(false);

	for (int i = 0; i < static_cast<int>(ditable.size()); i++)
	{
		diritem * di = new diritem(browser,
			browser->lastItem(),
			ditable.at(i).fileName(),
			ditable.at(i).dirPath(),
			ditable.at(i).date(),
			ditable.at(i).size(),ditable.at(i).date_int());
		di->setPixmap(0, il->loadIcon("folder",KIcon::Small));
	}

	for (int i = 0; i < static_cast<int>(fitable.size()); i++)
	{
		fileitem * fi = new fileitem(browser,
			browser->lastItem(),
			fitable.at(i).fileName(),
			fitable.at(i).dirPath(),
			fitable.at(i).date(),
			fitable.at(i).size(),
			fitable.at(i).date_int());
		fi->setPixmap(0, il->loadIcon("files",KIcon::Small));
	}

	// update the remote cwd line

	cwdline->setText(remotedir);
}

int Kasablanca::FormatFilelist(const char *filename, QString current, vector<RemoteFileInfo> *filetable, vector<RemoteFileInfo> *dirtable)
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

void Kasablanca::SLOT_KbftpReadReady(kbprocess* p)
{
	QString s;
	QString *log;
	Browser b;
	QString *remotedir;
	QListView *browser, *otherbrowser;

	if (p == &m_proc_a)
	{
		log = &m_log_a;
		b = A;
		browser = BrowserA;
		otherbrowser = BrowserB;
		remotedir = &m_currentremotedir_a;
	}
	else
	{
		log = &m_log_b;
		b = B;
		browser = BrowserB;
		otherbrowser = BrowserA;
		remotedir = &m_currentremotedir_b;
	}
	
	while (p->canReadLineStdout())
	{
		s = p->readLineStdout();
		if (s.left(3) == "kb.")
		{
			if (s.left(10) == "kb.success")
			{
				qWarning("kb.success");
				SetGuiStatus(connected, b);
				LogWindow->setColor(green);
				LogWindow->append(*log);
				*log = "";
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

				SetGuiStatus(occupied, b);
				LogWindow->setColor(white);
				LogWindow->append(s.remove(0, 9));
			}
			else if (s.left(10) == "kb.failure")
			{
				qWarning("kb.failure");
				LogWindow->setColor(red);
				LogWindow->append(*log);
				*log = "";
				if (s.left(16) == "kb.failure.fatal")
				{
					SetGuiStatus(disconnected, b);
				}
				else
				{
					SetGuiStatus(connected, b);
				}
			}
  	 		else if (s.left(7) == "kb.pwd.")
			{
				// when the pwd command was sucessful, the current remote dir is
				// set to the value after kb.pwd.

				*remotedir = s.remove(0, 7);
			}
			else if (s.left(6) == "kb.dir")
			{
				// when the ls command was succesful, the remote view is updated.

				if (s.left(12) == "kb.dir.done.")
				{
					qWarning("dir sucessfully retrieved");
					UpdateRemoteDisplay(s.remove(0,12), b);

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
									QListViewItemIterator it(browser);
									while ( it.current() )
									{
										it.current()->setSelected(true);
										++it;
									}
									if (p == &m_proc_a) InsertMarkedItems(transferitem::download_a_to_b);
									else InsertMarkedItems(transferitem::download_b_to_a);
								}
								else if (m_qstate == scanlocal)
								{
									QListViewItemIterator it(otherbrowser);
									while ( it.current() )
									{
										it.current()->setSelected(true);
										++it;
									}
									if (p == &m_proc_a) InsertMarkedItems(transferitem::upload_b_to_a);
									else InsertMarkedItems(transferitem::upload_a_to_b);
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
				}
			}
			else if (s == "kb.quit")
			{
				UpdateLocalDisplay(b);
				SetGuiStatus(disconnected, b);
			}
		}
		else log->append(s + '\n');
	}
}

void Kasablanca::SLOT_About()
{
	QPixmap p(locate("appdata", "about.png"));
	
	KAboutDialog d;
	
	d.setLogo(p);
	d.setVersion(m_version);
	d.setAuthor("", "sikor_sxe@radicalapproach.de", "http://kasablanca.berlios.de", "");  		
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
	
	name = KInputDialog::getText("Enter New Name:", "Enter New Name:", item->text(0), &b, this);

	if (!b) return;
	
	if (m_status_a == connected)
	{
		m_proc_a.writeStdin("rename " + item->text(0) + " " + name);
		UpdateRemote(A);
	}
	else if (m_status_a == disconnected)
	{
		m_currentlocaldir_a.rename(item->text(0), name);
		UpdateLocalDisplay(A);
	}
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
	
	name = KInputDialog::getText("Enter New Name:", "Enter New Name:", item->text(0), &b, this);

	if (!b) return;
	
	if (m_status_b == connected)
	{
		m_proc_b.writeStdin("rename " + item->text(0) + " " + name);
		UpdateRemote(B);
	}
	else if (m_status_b == disconnected)
	{
		m_currentlocaldir_b.rename(item->text(0), name);
		UpdateLocalDisplay(B);
	}

}
