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

#include <kaboutdialog.h>
#include <kaction.h>
#include <kapp.h>
#include <kapplication.h>
#include <kdeversion.h>
#include <kedittoolbar.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kicontheme.h>
#include <kinputdialog.h>
#include <kkeydialog.h>
#include <klocale.h>
#include <kmainwindow.h>
#include <kmenubar.h>
#include <kmessagebox.h>
#include <kprocess.h>
#include <kprocio.h>
#include <kstandarddirs.h>
#include <kstatusbar.h>
#include <kstdaction.h>

#include <qhbox.h>
#include <qstatusbar.h>
#include <qtextedit.h>
#include <qsizepolicy.h>
#include <qlineedit.h>
#include <qcolor.h>
#include <qpixmap.h>
#include <qlistview.h>
#include <qframe.h>
#include <qpushbutton.h>
#include <qlabel.h>
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
#include <qaction.h>
#include <qtoolbutton.h>
#include <qcursor.h>
//#include <qmutex.h>

#include <vector>

#include "diritem.h"
#include "fileitem.h"
#include "customconnectdialog.h"
#include "fileexistsdialog.h"
#include "bookmarks.h"
#include "bookmarkdialog.h"
#include "remotefileinfo.h"
#include "transferdir.h"
#include "transferfile.h"
#include "kbprocess.h"

#include "ftpsession.h"
//#include "eventhandler.h"
//#include "ftpthread.h"

#include "kasablanca.h"

Kasablanca::Kasablanca()
    : KMainWindow( 0, "Kasablanca" ),
      mp_view(new KasablancaMainWindow(this))
{

	/* new stuff */
	mp_session_a = new FtpSession(this, "session a");
	mp_session_b = new FtpSession(this, "session b");
	mp_sessionlist = new list<FtpSession*>;
	mp_sessionlist->push_back(mp_session_a);
	mp_sessionlist->push_back(mp_session_b);
	/* / new stuff */
	
    setAcceptDrops(true);
    setCentralWidget(mp_view);
    setupGUI();
    setupActions();
    setupStatusBar();
    setupMenu();
    statusBar()->show();
    //toolBar()->show();
    
    //UpdateLocalDisplay(A);
    //UpdateLocalDisplay(B);
    //SetGuiStatus(disconnected, A);
    //SetGuiStatus(disconnected, B);

    setAutoSaveSettings();
    
    /* when the qmainwindow is closed the object gets deleted,
    so the destructers of kbprocesses are called. */

    setWFlags(WDestructiveClose);
                    
    /* if initbookmarks returns false, an error box appears */
    
    if (initBookmarks() != 1) KMessageBox::error(0, i18n("Could not open Kasablanca bookmark xml."));

    //when developing you might want to change the following line to the kbftp path
    //and disable the other two lines. */

   // m_proc_a.addArgument("kbftp/kbftp");
   // m_proc_b.addArgument("kbftp/kbftp");

  	//if (locate("exe", "kbftp") == QString::null) KMessageBox::error(0,"kbftp binary is not in kde binary path.");
   // else 
   // {
   //        m_proc_a.addArgument(locate("exe", "kbftp"));
   //         m_proc_b.addArgument(locate("exe", "kbftp"));
   // }

    m_dcount = 0;
    m_qstate_a = done; m_qstate_b = done;
    m_fxpstate = stopped;
    m_fxpportinfo = "";

    //connect(&m_proc_a, SIGNAL(readyToRead(kbprocess*)), this, SLOT(SLOT_KbftpReadReady(kbprocess*)));
    //connect(&m_proc_b, SIGNAL(readyToRead(kbprocess*)), this, SLOT(SLOT_KbftpReadReady(kbprocess*)));
    
	 connect(mp_view->BrowserB, SIGNAL(selectionChanged()), SLOT(SLOT_SelectionChanged()));
    connect(mp_view->BrowserA, SIGNAL(selectionChanged()), SLOT(SLOT_SelectionChanged()));
    connect(mp_view->TaskView, SIGNAL(selectionChanged()), SLOT(SLOT_SelectionChanged()));
	 
	 /* new stuff */
	 
	 mp_session_a->SetLogWindow(mp_view->LogWindowA);
	 mp_session_a->SetBrowser(mp_view->BrowserA);
	 mp_session_a->SetConnectButton(mp_view->ConnectButtonA);
	 mp_session_a->SetRefreshButton(mp_view->RefreshButtonA);
	 mp_session_a->SetTransferButton(mp_view->TransferButtonA);
	 mp_session_a->SetCmdLine(mp_view->CommandLineA);
	 mp_session_a->SetCwdLine(mp_view->CwdLineA);
	 mp_session_a->SetBookmarksMenu(&m_bookmarksmenu_a);
	 mp_session_a->SetStatusLine(mp_statusline_a);
	 mp_session_a->SetEncryptionIcon(mp_encryptionicon_a);
	 mp_session_a->SetRclickMenu(&m_rclickmenu_a);
	 mp_session_a->SetSessionList(mp_sessionlist);
	 
	 connect(mp_view->RefreshButtonA, SIGNAL(clicked()), mp_session_a, SLOT(SLOT_RefreshButton()));
	 connect(mp_view->ConnectButtonA, SIGNAL(clicked()), mp_session_a, SLOT(SLOT_ConnectButton()));
	 connect(mp_view->TransferButtonA, SIGNAL(clicked()), mp_session_a, SLOT(SLOT_TransferButton()));
	 connect(mp_view->CwdLineA, SIGNAL(returnPressed()), mp_session_a, SLOT(SLOT_CwdLine()));
	 connect(mp_view->CommandLineA, SIGNAL(returnPressed()), mp_session_a, SLOT(SLOT_CmdLine()));
	 
	 mp_session_b->SetLogWindow(mp_view->LogWindowB);
	 mp_session_b->SetBrowser(mp_view->BrowserB);
	 mp_session_b->SetConnectButton(mp_view->ConnectButtonB);
	 mp_session_b->SetRefreshButton(mp_view->RefreshButtonB);
	 mp_session_b->SetTransferButton(mp_view->TransferButtonB);
	 mp_session_b->SetCmdLine(mp_view->CommandLineB);
	 mp_session_b->SetCwdLine(mp_view->CwdLineB);
	 mp_session_b->SetBookmarksMenu(&m_bookmarksmenu_b);
	 mp_session_b->SetStatusLine(mp_statusline_b);
	 mp_session_b->SetEncryptionIcon(mp_encryptionicon_b);
	 mp_session_b->SetRclickMenu(&m_rclickmenu_b);
	 mp_session_b->SetSessionList(mp_sessionlist);
	 
	 connect(mp_view->RefreshButtonB, SIGNAL(clicked()), mp_session_b, SLOT(SLOT_RefreshButton()));
	 connect(mp_view->ConnectButtonB, SIGNAL(clicked()), mp_session_b, SLOT(SLOT_ConnectButton()));
    connect(mp_view->TransferButtonB, SIGNAL(clicked()), mp_session_b, SLOT(SLOT_TransferButton()));
	 connect(mp_view->CwdLineB, SIGNAL(returnPressed()), mp_session_b, SLOT(SLOT_CwdLine()));
	 connect(mp_view->CommandLineB, SIGNAL(returnPressed()), mp_session_b, SLOT(SLOT_CmdLine()));
	 
	 connect(mp_session_a, SIGNAL(gui_update()), SLOT(SLOT_SelectionChanged()));
	 connect(mp_session_b, SIGNAL(gui_update()), SLOT(SLOT_SelectionChanged()));
	 connect(mp_session_a, SIGNAL(gui_queueitems(list<kbitem*>, FtpSession*, FtpSession*, bool)), 
	 	SLOT(SLOT_QueueItems(list<kbitem*>, FtpSession*, FtpSession*, bool)));
	connect(mp_session_b, SIGNAL(gui_queueitems(list<kbitem*>, FtpSession*, FtpSession*, bool)), 
	 	SLOT(SLOT_QueueItems(list<kbitem*>, FtpSession*, FtpSession*, bool)));
	 	 
	 mp_session_a->Disconnect();
	 mp_session_b->Disconnect();
	 
	 /* /new stuff */
	
}

void Kasablanca::setupGUI() 
{
    mp_view->TaskView->setSorting(-1);
    mp_view->BrowserA->setSorting(-1);
    mp_view->BrowserB->setSorting(-1);

    mp_header_a = mp_view->BrowserA->header();
    mp_header_b = mp_view->BrowserB->header();

    mp_view->TransferButtonA->setIconSet(KGlobal::iconLoader()->loadIconSet("forward",KIcon::Toolbar));
    mp_view->TransferButtonB->setIconSet(KGlobal::iconLoader()->loadIconSet("back",KIcon::Toolbar));
    mp_view->RefreshButtonA->setIconSet(KGlobal::iconLoader()->loadIconSet("reload",KIcon::Toolbar));
    mp_view->RefreshButtonB->setIconSet(KGlobal::iconLoader()->loadIconSet("reload",KIcon::Toolbar));
    mp_view->ConnectButtonA->setIconSet(KGlobal::iconLoader()->loadIconSet("connect_no",KIcon::Toolbar));
    mp_view->ConnectButtonB->setIconSet(KGlobal::iconLoader()->loadIconSet("connect_no",KIcon::Toolbar));
}

void Kasablanca::setupActions()
{
	KStdAction::quit(kapp, SLOT(quit()), actionCollection());

	//m_toolbarAction = KStdAction::showToolbar(this, SLOT(optionsShowToolbar()), actionCollection());
	m_statusbarAction = KStdAction::showStatusbar(this, SLOT(optionsShowStatusbar()), actionCollection());

	KStdAction::keyBindings(this, SLOT(optionsConfigureKeys()), actionCollection());
	//KStdAction::configureToolbars(this, SLOT(optionsConfigureToolbars()), actionCollection());
	//KStdAction::preferences(this, SLOT(optionsPreferences()), actionCollection());

	/*
	// this doesn't do anything useful.  it's just here to illustrate
	// how to insert a custom menu and menu item
	KAction *custom = new KAction(i18n("Cus&tom Menuitem"), 0,
											this, SLOT(optionsPreferences()),
											actionCollection(), "custom_action");
	*/									 
	
	new KAction(i18n("Edit"), CTRL+Key_E, this, SLOT(SLOT_EditBookmarks()), actionCollection(), "edit bookmarks action");
											
	createGUI();
}

void Kasablanca::optionsShowToolbar()
{
    if (m_toolbarAction->isChecked())
        toolBar()->show();
    else
        toolBar()->hide();
}

void Kasablanca::optionsShowStatusbar()
{
    if (m_statusbarAction->isChecked())
        statusBar()->show();
    else
        statusBar()->hide();
}

void Kasablanca::optionsConfigureKeys()
{
    KKeyDialog::configure(actionCollection(), mp_view);
}

void Kasablanca::optionsConfigureToolbars()
{
    // use the standard toolbar editor
#if defined(KDE_MAKE_VERSION)
# if KDE_VERSION >= KDE_MAKE_VERSION(3,1,0)
    saveMainWindowSettings(KGlobal::config(), autoSaveGroup());
# else
    saveMainWindowSettings(KGlobal::config());
# endif
#else
    saveMainWindowSettings(KGlobal::config());
#endif
}

void Kasablanca::optionsPreferences()
{
    // popup a preference dialog
/*
    KasablancaPreferences dlg;
    if (dlg.exec())
    {
        ;
    }
*/
}

void Kasablanca::newToolbarConfig()
{
    // this slot is called when user clicks "Ok" or "Apply" in the toolbar editor.
    // recreate our GUI, and re-apply the settings (e.g. "text under icons", etc.)
    createGUI();

#if defined(KDE_MAKE_VERSION)
# if KDE_VERSION >= KDE_MAKE_VERSION(3,1,0)
    applyMainWindowSettings(KGlobal::config(), autoSaveGroup());
# else
    applyMainWindowSettings(KGlobal::config());
# endif
#else
    applyMainWindowSettings(KGlobal::config());
#endif
}

void Kasablanca::setupMenu()
{	
	QWidget *w;
	w = factory()->container("connect menu", this);
	if (w == NULL) qWarning("ERROR: 'connect menu' not found in .ui file");
	else
	{
		QPopupMenu *menu = static_cast<QPopupMenu *>(w);
		menu->insertItem(i18n("Session A"),  &m_bookmarksmenu_a);
		menu->insertItem(i18n("Session B"), &m_bookmarksmenu_b);
	}
	w = factory()->container("action menu", this);
	if (w == NULL) qWarning("ERROR: 'action menu' not found in .ui file");
	else
	{
		QPopupMenu *menu = static_cast<QPopupMenu *>(w);
		menu->insertItem(i18n("Session A"),  &m_rclickmenu_a);
		menu->insertItem(i18n("Session B"), &m_rclickmenu_b);
		menu->insertItem(i18n("Queue"), &m_rclickmenu_t);
	}	
	 
	m_rclickmenu_a.insertItem("Transfer", Transfer);
	m_rclickmenu_a.insertItem("Put in Queue", Queue);
	m_rclickmenu_a.insertSeparator();
	m_rclickmenu_a.insertItem("Delete", Delete);
	m_rclickmenu_a.insertItem("Rename", Rename);
	m_rclickmenu_a.insertSeparator();
	m_rclickmenu_a.insertItem("Mkdir", Mkdir);
		
	m_rclickmenu_b.insertItem("Transfer", Transfer);
	m_rclickmenu_b.insertItem("Put in Queue", Queue);
	m_rclickmenu_b.insertSeparator();
	m_rclickmenu_b.insertItem("Delete", Delete);
	m_rclickmenu_b.insertItem("Rename", Rename);
	m_rclickmenu_b.insertSeparator();
	m_rclickmenu_b.insertItem("Mkdir", Mkdir);

	m_rclickmenu_t.insertItem("Start Queue", Start);
	m_rclickmenu_t.insertSeparator();
	m_rclickmenu_t.insertItem("Skip Item(s)", Skip);
   
	/* new stuff */
	
	connect(&m_rclickmenu_a, SIGNAL(activated(int)), mp_session_a, SLOT(SLOT_ActionMenu(int)));
	connect(&m_bookmarksmenu_a, SIGNAL(activated(int)), mp_session_a, SLOT(SLOT_ConnectMenu(int)));     
	connect(mp_view->BrowserA, SIGNAL(doubleClicked(QListViewItem*)), mp_session_a, SLOT(SLOT_ItemClicked(QListViewItem*)));
	connect(mp_view->BrowserA, SIGNAL(rightButtonPressed(QListViewItem *, const QPoint &, int)), 
		mp_session_a, SLOT(SLOT_ItemRClicked(QListViewItem *, const QPoint &, int)));
	connect(mp_view->BrowserA->header(), SIGNAL (clicked(int)), mp_session_a, SLOT (SLOT_HeaderClicked(int)));
		
	connect(&m_rclickmenu_b, SIGNAL(activated(int)), mp_session_b, SLOT(SLOT_ActionMenu(int)));
	connect(&m_bookmarksmenu_b, SIGNAL(activated(int)), mp_session_b, SLOT(SLOT_ConnectMenu(int)));     
	connect(mp_view->BrowserB, SIGNAL(doubleClicked(QListViewItem*)), mp_session_b, SLOT(SLOT_ItemClicked(QListViewItem*)));
	connect(mp_view->BrowserB, SIGNAL(rightButtonPressed(QListViewItem *, const QPoint &, int)), 
		mp_session_b, SLOT(SLOT_ItemRClicked(QListViewItem *, const QPoint &, int)));
	connect(mp_view->BrowserB->header(), SIGNAL(clicked(int)), mp_session_b, SLOT(SLOT_HeaderClicked(int)));
		
	/* /new stuff */
		
	connect( mp_view->TaskView, SIGNAL (rightButtonPressed( QListViewItem *, const QPoint &, int )), this,
			SLOT (SLOT_ItemRightClickedT(QListViewItem *, const QPoint &, int )));

	m_rclickmenu_t.connectItem(Start, this, SLOT(SLOT_StartQueue()));
	m_rclickmenu_t.connectItem(Skip, this, SLOT(SLOT_SkipTasks()));
	
	m_rclickmenu_t.setItemEnabled(Start, false);
	m_rclickmenu_t.setItemEnabled(Skip, false);	 
}

void Kasablanca::setupStatusBar()
{
    m_iconencrypted = KGlobal::iconLoader()->loadIconSet("encrypted",KIcon::Small).pixmap(QIconSet::Small,QIconSet::Normal);
    m_iconunencrypted = KGlobal::iconLoader()->loadIconSet("encrypted",KIcon::Small).pixmap(QIconSet::Small,QIconSet::Disabled);
            
    mp_statusline_a = new QLabel("", statusBar());
    mp_encryptionicon_a = new QLabel(statusBar());
    mp_encryptionicon_a->setPixmap(m_iconunencrypted);

    mp_statusline_b = new QLabel("", statusBar());
    mp_encryptionicon_b = new QLabel(statusBar());
    mp_encryptionicon_b->setPixmap(m_iconunencrypted);

    statusBar()->addWidget(mp_statusline_a, 1);
    statusBar()->addWidget(mp_encryptionicon_a, 0);
    statusBar()->addWidget(mp_statusline_b, 1, true);
    statusBar()->addWidget(mp_encryptionicon_b, 0, true);
}

Kasablanca::~Kasablanca()
{
}	

int Kasablanca::initBookmarks()
{
    m_bookmarks.clear();
    m_bookmarks = bookmarks.getBookmarks();
    
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

void Kasablanca::SLOT_QueueItems(list<kbitem*> items, FtpSession* src, FtpSession* dst, bool start)
{
	for (list<kbitem*>::iterator i = items.begin(); i != items.end(); i++)
	{
		if ((*i)->rtti() == kbitem::dir) new transferdir(
			mp_view->TaskView, 
			mp_view->TaskView->lastItem(), 
			src,
			dst,
			new KbFileInfo(*i, src->WorkingDir()),
			new KbFileInfo(*i, dst->WorkingDir())
		);
		else if ((*i)->rtti() == kbitem::file) new transferfile(
			mp_view->TaskView, 
			mp_view->TaskView->lastItem(), 
			src,
			dst,
			new KbFileInfo(*i, src->WorkingDir()),
			new KbFileInfo(*i, dst->WorkingDir())
		);
		(*i)->setSelected(false);
		(*i)->repaint();
	}
	if (start) SLOT_ProcessQueue();
}

void Kasablanca::SLOT_ProcessQueue()
{
	while (mp_view->TaskView->firstChild())
	{
		transferitem *ti = static_cast<transferitem*>(mp_view->TaskView->firstChild());
		ti->Transfer();
		delete ti;
	}
}

void Kasablanca::SLOT_EditBookmarks()
{
    BookmarkDialog dlg;
    
    if (dlg.exec() == QDialog::Accepted)
    {
        initBookmarks();
    }
}

void Kasablanca::SLOT_ItemRightClickedT(QListViewItem *, const QPoint & point, int)
{
	qWarning("slot: SLOT_ItemRightClickedT()");
	m_rclickmenu_t.exec(point);
}

void Kasablanca::SLOT_SkipTasks()
{
    QListViewItemIterator it(mp_view->TaskView);
    while ( it.current() )
    {
        if (it.current()->isSelected()) delete it.current();
        else ++it;
    }
}

void Kasablanca::InsertMarkedItems(transferitem::transfertype t, QListViewItem* begin)
{
	QListViewItem* lastitem = begin;

	switch (t)
	{
		case transferitem::download_a_to_b:
		{
			QListViewItemIterator it(mp_view->BrowserA);
			while (it.current())
			{	
				if (it.current()->isSelected())
				{
					QListViewItem* item = it.current();

					QFileInfo filocal(m_currentlocaldir_b.absPath(), static_cast<diritem*>(item)->File());
					RemoteFileInfo firemote(static_cast<kbitem*>(item)->Path(),
						static_cast<diritem*>(item)->File(),
						static_cast<diritem*>(item)->Size(),
						static_cast<diritem*>(item)->Date(),
						static_cast<diritem*>(item)->DateInt());
					
					if (item->rtti() == 1001)
					{	
						qWarning("download a directory from a to b...");
						lastitem = new transferdir(mp_view->TaskView, lastitem, filocal, firemote, t);
					}
					else if (item->rtti() == 1002)
					{
						qWarning("download a file from a to b...");
						lastitem = new transferfile(mp_view->TaskView, lastitem, filocal, firemote, t);
					}
					item->setSelected(false);	
				}
				++it;
			}
			break;
		}
		case transferitem::download_b_to_a:
		{
			QListViewItemIterator it(mp_view->BrowserB);
			while (it.current())
			{	
				if (it.current()->isSelected())
				{
					QListViewItem* item = it.current();

					QFileInfo filocal(m_currentlocaldir_a.absPath(), static_cast<diritem*>(item)->File());
					RemoteFileInfo firemote(static_cast<kbitem*>(item)->Path(),
						static_cast<diritem*>(item)->File(),
						static_cast<diritem*>(item)->Size(),
						static_cast<diritem*>(item)->Date(),
						static_cast<diritem*>(item)->DateInt());
					
					if (item->rtti() == 1001)
					{	
						qWarning("download a directory from b to a...");
						lastitem = new transferdir(mp_view->TaskView, lastitem, filocal, firemote, t);
					}
					else if (item->rtti() == 1002)
					{
						qWarning("download a file from b to a...");
						lastitem = new transferfile(mp_view->TaskView, lastitem, filocal, firemote, t);
					}
					item->setSelected(false);	
				}
				++it;
			}
			break;
		}
		case transferitem::upload_a_to_b:
		{
			QListViewItemIterator it(mp_view->BrowserA);
			while (it.current())
			{	
				if (it.current()->isSelected())
				{
					QListViewItem* item = it.current();

					QFileInfo filocal(m_currentlocaldir_a.absPath(), static_cast<kbitem*>(item)->File());
					RemoteFileInfo firemote(m_currentremotedir_b,
						static_cast<diritem*>(item)->File(),
						static_cast<diritem*>(item)->Size(),
						static_cast<diritem*>(item)->Date(),
						static_cast<diritem*>(item)->DateInt());
					
					if (item->rtti() == 1001)
					{	
						qWarning("upload a directory from a to b...");
						lastitem = new transferdir(mp_view->TaskView, lastitem, filocal, firemote, t);
					}
					else if (item->rtti() == 1002)
					{
						qWarning("upload a file from a to b...");
						lastitem = new transferfile(mp_view->TaskView, lastitem, filocal, firemote, t);
					}
					item->setSelected(false);	
				}
				++it;
			}	
			break;
		}
		case transferitem::upload_b_to_a:
		{
			QListViewItemIterator it(mp_view->BrowserB);
			while (it.current())
			{	
				if (it.current()->isSelected())
				{
					QListViewItem* item = it.current();

					QFileInfo filocal(m_currentlocaldir_b.absPath(), static_cast<kbitem*>(item)->File());
					RemoteFileInfo firemote(m_currentremotedir_a,
						static_cast<diritem*>(item)->File(),
						static_cast<diritem*>(item)->Size(),
						static_cast<diritem*>(item)->Date(),
						static_cast<diritem*>(item)->DateInt());
					
					if (item->rtti() == 1001)
					{	
						qWarning("upload a directory from b to a...");
						lastitem = new transferdir(mp_view->TaskView, lastitem, filocal, firemote, t);
					}
					else if (item->rtti() == 1002)
					{
						qWarning("upload a file from b to a...");
						lastitem = new transferfile(mp_view->TaskView, lastitem, filocal, firemote, t);
					}
					item->setSelected(false);	
				}
				++it;
			}	
			break;
		}
		/*
		case transferitem::fxp_a_to_b:
		{
			QListViewItemIterator it(mp_view->BrowserA);
			while (it.current())
			{
				if (it.current()->isSelected())
				{
					QListViewItem* item = it.current();

					RemoteFileInfo fifxpsrc(m_currentremotedir_a,
						static_cast<diritem*>(item)->m_file,
						static_cast<diritem*>(item)->m_size,
						static_cast<diritem*>(item)->m_date,
						static_cast<diritem*>(item)->m_date_int);

					RemoteFileInfo fifxpdst(m_currentremotedir_b,
						static_cast<diritem*>(item)->m_file,
						static_cast<diritem*>(item)->m_size,
						static_cast<diritem*>(item)->m_date,
						static_cast<diritem*>(item)->m_date_int);

					if (item->rtti() == 1001)
					{
						qWarning("fxp a directory from a to b...");
						lastitem = new transferdir(mp_view->TaskView, lastitem, fifxpsrc, fifxpdst, t);
					}
					else if (item->rtti() == 1002)
					{
						qWarning("fxp a file from a to b...");
						lastitem = new transferfile(mp_view->TaskView, lastitem, fifxpsrc, fifxpdst, t);
					}
					item->setSelected(false);
				}
				++it;
			}
			break;
		}
		case transferitem::fxp_b_to_a:
		{
			QListViewItemIterator it(mp_view->BrowserB);
			while (it.current())
			{
				if (it.current()->isSelected())
				{
					QListViewItem* item = it.current();

					RemoteFileInfo fifxpsrc(m_currentremotedir_b,
						static_cast<diritem*>(item)->m_file,
						static_cast<diritem*>(item)->m_size,
						static_cast<diritem*>(item)->m_date,
						static_cast<diritem*>(item)->m_date_int);

					RemoteFileInfo fifxpdst(m_currentremotedir_a,
						static_cast<diritem*>(item)->m_file,
						static_cast<diritem*>(item)->m_size,
						static_cast<diritem*>(item)->m_date,
						static_cast<diritem*>(item)->m_date_int);

					if (item->rtti() == 1001)
					{
						qWarning("fxp a directory from b to a...");
						lastitem = new transferdir(mp_view->TaskView, lastitem, fifxpsrc, fifxpdst, t);
					}
					else if (item->rtti() == 1002)
					{
						qWarning("fxp a file from b to a...");
						lastitem = new transferfile(mp_view->TaskView, lastitem, fifxpsrc, fifxpdst, t);
					}
					item->setSelected(false);
				}
				++it;
			}
			break;
		}
		*/
	}	
}

void Kasablanca::Xfer()
{
	kbprocess* proc, *proc_fxpsrc, *proc_fxpdst;
	QString remotedir, dir_fxpsrc, dir_fxpdst;
	QDir* localdir;
	Browser remotebrowser, localbrowser, browser_fxpsrc, browser_fxpdst;
	siteinfo* site, *site_fxpsrc, *site_fxpdst;
	QListView* remoteview, *view_fxpsrc, *view_fxpdst;
	QueueState* qstate, *qstate_fxpsrc, *qstate_fxpdst;

	if (mp_view->TaskView->childCount() == 0)
	{
		return;
	}
	else
	{
		mp_view->TaskView->firstChild()->setSelected(false);
		mp_view->TaskView->firstChild()->setSelectable(false);
	}

	transferitem* item = static_cast<transferitem*>(mp_view->TaskView->firstChild());

	qWarning("item: %s", item->text(0).latin1());

	if ((item->type() == transferitem::download_a_to_b) || (item->type() == transferitem::upload_b_to_a))
	{
		site = &m_site_a;
		proc = &m_proc_a;
		remotebrowser = A;
		localbrowser = B;
		remoteview = mp_view->BrowserA;
		remotedir = m_currentremotedir_a;
		localdir = &m_currentlocaldir_b;
		qstate = &m_qstate_a;
	}
	else if ((item->type() == transferitem::download_b_to_a) || (item->type() == transferitem::upload_a_to_b))
	{
		site = &m_site_b;
		proc = &m_proc_b;
		remotebrowser = B;
		localbrowser = A;
		remoteview = mp_view->BrowserB;
		remotedir = m_currentremotedir_b;
		localdir = &m_currentlocaldir_a;
		qstate = &m_qstate_b;
	}
	else if (item->type() == transferitem::fxp_a_to_b)
	{
		site_fxpsrc = &m_site_a;
		site_fxpdst = &m_site_b;
		proc_fxpsrc = &m_proc_a;
		proc_fxpdst = &m_proc_b;
		browser_fxpsrc = A;
		browser_fxpdst = B;
		view_fxpsrc = mp_view->BrowserA;
		view_fxpdst = mp_view->BrowserB;
		dir_fxpsrc = m_currentremotedir_a;
		dir_fxpdst = m_currentremotedir_b;
		qstate_fxpsrc = &m_qstate_a;
		qstate_fxpdst = &m_qstate_b;
	}
	else if (item->type() == transferitem::fxp_b_to_a)
	{
		site_fxpsrc = &m_site_b;
		site_fxpdst = &m_site_a;
		proc_fxpsrc = &m_proc_b;
		proc_fxpdst = &m_proc_a;
		browser_fxpsrc = B;
		browser_fxpdst = A;
		view_fxpsrc = mp_view->BrowserB;
		view_fxpdst = mp_view->BrowserA;
		dir_fxpsrc = m_currentremotedir_b;
		dir_fxpdst = m_currentremotedir_a;
		qstate_fxpsrc = &m_qstate_b;
		qstate_fxpdst = &m_qstate_a;
	}
	else return; /* should not happen! */

	/* process first queue item */

	/* check if it's fxp and if we're in the correct directory
	yet. if not issue a change to the transfer directory. */

	if ((item->type() == transferitem::fxp_a_to_b) || (item->type() == transferitem::fxp_b_to_a))
	{
		if (item->m_fifxpsrc.dirPath() != dir_fxpsrc)
		{
			qWarning("in the wrong path, change to %s", item->m_fifxpsrc.dirPath().latin1());
			*qstate_fxpsrc = changedir;

			proc_fxpsrc->writeStdin("chdir");
			proc_fxpsrc->writeStdin(item->m_fifxpsrc.dirPath());
			//UpdateRemote(browser_fxpsrc);
			return;
		}
		else if (item->m_fifxpdst.dirPath() != dir_fxpdst)
		{
			qWarning("in the wrong path, change to %s", item->m_fifxpdst.dirPath().latin1());
			*qstate_fxpdst = changedir;

			proc_fxpdst->writeStdin("chdir");
			proc_fxpdst->writeStdin(item->m_fifxpdst.dirPath());
			//UpdateRemote(browser_fxpdst);
			return;
		}
	}
	else
	{
		/* check if we're in the correct remote directory yet.
		if not issue a change to the transfer directory */

		if (item->m_firemote.dirPath() != remotedir)
		{
			qWarning("in the wrong path, change to %s", item->m_firemote.dirPath().latin1());
			*qstate = changedir;

			proc->writeStdin("chdir");
			proc->writeStdin(item->m_firemote.dirPath());
			//UpdateRemote(remotebrowser);
			return;
		}

		localdir->cd(item->m_filocal.dirPath());
		//UpdateLocalDisplay(localbrowser);
	}

	// handle items

	if (item->rtti() == transferitem::dir)
	{
		qWarning("...is a dir");
		transferdir* dir = static_cast<transferdir*>(mp_view->TaskView->firstChild());

		/*-------------upload/download---------------*/

		if ((dir->type() == transferitem::upload_a_to_b) || (dir->type() == transferitem::upload_b_to_a))
		{
			/* the scanlocal flag is set */

			*qstate = scanlocal;

			/* create dir on remote */

			proc->writeStdin("mkdir");
			proc->writeStdin(dir->m_firemote.fileName());

			/* change to remote dir */

			proc->writeStdin("chdir");
			proc->writeStdin(dir->m_firemote.fileName());

			/* enter dir on local */

			localdir->cd(dir->m_filocal.fileName());

			//UpdateLocalDisplay(localbrowser);
			//UpdateRemote(remotebrowser);
		}
		else if ((dir->type() == transferitem::download_a_to_b) || (dir->type() == transferitem::download_b_to_a))
		{
			/* the scan dir flag is set */

			*qstate = scanremote;

			/* cwd to new dir on remote.  */

			proc->writeStdin("chdir");
			proc->writeStdin(dir->m_firemote.fileName());

			/* create dir on local */

			localdir->mkdir(dir->m_filocal.fileName());

			/* enter dir on local */

			localdir->cd(dir->m_filocal.fileName());

			//UpdateLocalDisplay(localbrowser);
			//UpdateRemote(remotebrowser);
		}
		else if ((dir->type() == transferitem::fxp_a_to_b) or (dir->type() == transferitem::fxp_b_to_a))
		{
			/* the scan dir flag is set */

			*qstate_fxpsrc = scanfxp;
		
			/* cwd to new dir on src.  */

			proc_fxpsrc->writeStdin("chdir");
			proc_fxpsrc->writeStdin(dir->m_fifxpsrc.fileName());

			/* create dir on dst*/

			proc_fxpdst->writeStdin("mkdir");
			proc_fxpdst->writeStdin(dir->m_fifxpdst.fileName());

			/* enter dir on dst */

			proc_fxpdst->writeStdin("chdir");
			proc_fxpdst->writeStdin(dir->m_fifxpdst.fileName());
			
			/* update remotes */
			
			//UpdateRemote(A);
			//UpdateRemote(B);
		}
	}
	else if (item->rtti() == transferitem::file)
	{
		qWarning("...is a file");
		transferfile* file = static_cast<transferfile*>(mp_view->TaskView->firstChild());

		/*-------------upload/download---------------*/

		QString remotename = file->m_firemote.fileName();
		QString localname = file->m_filocal.fileName();
		FileExistsDialog dialog;
		bool resume = false;
		bool skip = false;
		bool filepresent = false;

		if ((file->type() == transferitem::fxp_a_to_b) || (file->type() == transferitem::fxp_b_to_a))
		{					
			if (m_fxpstate == stopped) proc_fxpdst->writeStdin("fxpinitpasv");
			else if (m_fxpstate == initpasv)
			{
				proc_fxpsrc->writeStdin("fxpinitport");
				proc_fxpsrc->writeStdin(m_fxpportinfo);
			}
			else if (m_fxpstate == initport)
			{
				proc_fxpsrc->writeStdin("fxpget");
				proc_fxpsrc->writeStdin(file->m_fifxpsrc.fileName());
			}
			else if (m_fxpstate == get)
			{
				proc_fxpdst->writeStdin("fxpput");
				proc_fxpdst->writeStdin(file->m_fifxpdst.fileName());
			}
			else if (m_fxpstate == put) proc_fxpsrc->writeStdin("fxpxferfinished");
			else if (m_fxpstate == waitsrc) proc_fxpdst->writeStdin("fxpxferfinished");
			else if (m_fxpstate == abort) proc_fxpsrc->writeStdin("abor");
		}
		else if ((file->type() == transferitem::upload_a_to_b) || (file->type() == transferitem::upload_b_to_a))
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
						if (offset < firemote->Size())
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
							#if KDE_IS_VERSION(3,2,0)
							remotename = KInputDialog::getText("Enter New Name:", "Enter New Name:", remotename + "_alt", &b, this);
							#else
							remotename = KLineEditDlg::getText("Enter New Name:", remotename + "_alt", &b, this);
							#endif
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
				EndXfer();
				Xfer();
			}
			else
			{
				/* if tls level is 2 then disable data encryption, on level 3 enable it */

				if (site->GetTls() == 2)
				{
					proc->writeStdin("setdataencryption");
				 	proc->writeStdin("off");
				}
				else if (site->GetTls() == 3)
				{
					proc->writeStdin("setdataencryption");
					proc->writeStdin("on");
				}

				/* resume the file if resume is set */

				if (resume)
				{
					proc->writeStdin("putresume");
					proc->writeStdin(file->m_filocal.filePath());
					proc->writeStdin(remotename);
					proc->writeStdin(QString::number(offset));
				}
				else
				{
					proc->writeStdin("put");
					proc->writeStdin(file->m_filocal.filePath());
					proc->writeStdin(remotename);
				}
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
						#if KDE_IS_VERSION(3,2,0)
						localname = KInputDialog::getText("Enter New Name:", "Enter New Name:", localname + "_alt", &b, this);
						#else
						localname = KLineEditDlg::getText("Enter New Name:", localname + "_alt", &b, this);
						#endif

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
				EndXfer();
				Xfer();
			}
			else
			{
				/* if tls level is 2 then disable data encryption, on level 3 enable it */

				if (site->GetTls() == 2)
				{
					proc->writeStdin("setdataencryption");
					proc->writeStdin("off");
				}
				else if (site->GetTls() == 3)
				{
					proc->writeStdin("setdataencryption");
					proc->writeStdin("on");
				}
				/* resume the file if resume is set */

				if (resume) 
				{
					proc->writeStdin("getresume");
					proc->writeStdin(file->m_filocal.dirPath() + "/" + localname);
					proc->writeStdin(remotename);
					proc->writeStdin(QString::number(offset));
				}
				else
				{
					proc->writeStdin("get");
				 	proc->writeStdin(file->m_filocal.dirPath() + "/" + localname);
					proc->writeStdin(remotename);
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
		
	QListViewItemIterator ita(mp_view->BrowserA);
 	while ( ita.current() )
 	{
   	if (ita.current()->isSelected()) counter_a++;
   	++ita;
 	}
	
	flag = ((counter_a) && (!mp_session_a->Occupied()) && (!mp_session_b->Occupied()));

	m_rclickmenu_a.setItemEnabled(Transfer, flag);
	m_rclickmenu_a.setItemEnabled(Queue, flag);
	mp_view->TransferButtonA->setEnabled(flag);
	
	/* when at least one item is selected, Delete and Rename are
	enabled. */
	
	m_rclickmenu_a.setItemEnabled(Rename, (counter_a >= 1));
	m_rclickmenu_a.setItemEnabled(Delete, (counter_a >= 1));

	QListViewItemIterator itb(mp_view->BrowserB);
 	while ( itb.current() )
 	{
   	if (itb.current()->isSelected()) counter_b++;
   	++itb;
 	}
		
	flag = ((counter_b) && (!mp_session_a->Occupied()) && (!mp_session_b->Occupied()));

	m_rclickmenu_b.setItemEnabled(Transfer, flag);
	m_rclickmenu_b.setItemEnabled(Queue, flag);
	mp_view->TransferButtonB->setEnabled(flag);
	
	/* when at least one item is selected, Delete and Rename are
	enabled. */
	
	m_rclickmenu_b.setItemEnabled(Rename, (counter_b >= 1));
	m_rclickmenu_b.setItemEnabled(Delete, (counter_b >= 1));
}

void Kasablanca::SLOT_TransferA()
{
	if ((mp_session_a->Occupied()) || (mp_session_b->Occupied()))
	{
		qWarning("ERROR: transfer button clicked while one of the sessions is occupied");
		return;
	}
	else if ((mp_session_a->Connected()) || (!mp_session_b->Connected()))
	{
		QListViewItemIterator it(mp_view->BrowserA);
		while (it.current())
		{
			if (it.current()->isSelected()) 
			{	
				kbitem* item = static_cast<kbitem*>(it.current());
				mp_session_a->Get(item->File(), mp_view->CwdLineB->text() + "/" + item->File(), mp_session_b);
				return;
			}
			it++;
		}
	}
	/*
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
		InsertMarkedItems(transferitem::fxp_a_to_b);
		Xfer();
		//KMessageBox::error(0,"site to site transfer (fxp) is not supported yet.");
	}
	*/
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
		InsertMarkedItems(transferitem::fxp_b_to_a);
		Xfer();
		//KMessageBox::error(0,"site to site transfer (fxp) is not supported yet.");
	}
}

void Kasablanca::SLOT_QueueA() 
{ 
	qWarning("slot: SLOT_QueueA()");

	if ((m_status_a == connected) && (m_status_b == disconnected))
	{
		InsertMarkedItems(transferitem::download_a_to_b, mp_view->TaskView->lastItem()); 
	}
	else if ((m_status_a == disconnected) && (m_status_b == connected))
	{
		InsertMarkedItems(transferitem::upload_a_to_b, mp_view->TaskView->lastItem()); 
	}
	else if ((m_status_a == connected) && (m_status_b == connected))
	{
		InsertMarkedItems(transferitem::fxp_a_to_b);
		//KMessageBox::error(0,"site to site transfer (fxp) is not supported yet.");
	}
}

void Kasablanca::SLOT_QueueB() 
{ 	
	qWarning("slot: SLOT_QueueB()");

	if ((m_status_a == connected) && (m_status_b == disconnected))
	{
		InsertMarkedItems(transferitem::upload_b_to_a, mp_view->TaskView->lastItem());
	}
	else if ((m_status_a == disconnected) && (m_status_b == connected))
	{
		InsertMarkedItems(transferitem::download_b_to_a, mp_view->TaskView->lastItem());
	}	
	else if ((m_status_a == connected) && (m_status_b == connected))
	{
		InsertMarkedItems(transferitem::fxp_b_to_a);
		//KMessageBox::error(0,"site to site transfer (fxp) is not supported yet.");
	}
}

void Kasablanca::SLOT_StartQueue()
{ 
	Xfer();
}

void Kasablanca::SLOT_KbftpReadReady(kbprocess* p)
{
	QString s;
	QString *log;
	Browser b;
	QString *remotedir;
	QListView *browser, *otherbrowser;
	QueueState* qstate;
	QTextEdit* logwindow;
	QLabel* statusline, *encryptionicon;
	siteinfo* site;

	if (p == &m_proc_a)
	{
		site = &m_site_a;
		encryptionicon = mp_encryptionicon_a;
		statusline = mp_statusline_a;
		logwindow = mp_view->LogWindowA;
		log = &m_log_a;
		b = A;
		browser = mp_view->BrowserA;
		otherbrowser = mp_view->BrowserB;
		remotedir = &m_currentremotedir_a;
		qstate = &m_qstate_a;
	}
	else
	{
		site = &m_site_b;
		encryptionicon = mp_encryptionicon_b;
		statusline = mp_statusline_b;
		logwindow = mp_view->LogWindowB;
		log = &m_log_b;
		b = B;
		browser = mp_view->BrowserB;
		otherbrowser = mp_view->BrowserA;
		remotedir = &m_currentremotedir_b;
		qstate = &m_qstate_b;
	}
	
	while (p->canReadLineStdout())
	{
		s = p->readLineStdout();
		if (s.left(3) == "kb.")
		{
			if (s.left(10) == "kb.success")
			{
				if (site->GetTls() > 0) encryptionicon->setPixmap(m_iconencrypted);
				else encryptionicon->setPixmap(m_iconunencrypted);
			
				qWarning("kb.success");
				//SetGuiStatus(connected, b);
				logwindow->setColor(green);
				logwindow->append(*log);
				*log = "";
			}
			else if (s.left(9) == "kb.issue.")
			{
				qWarning("%s", s.latin1());
				
				/* kill the process timer */
	
				killTimers();
				
				if (s == "kb.issue.dir")
				{
					if (site->GetTls() > 1) encryptionicon->setPixmap(m_iconencrypted);
					else encryptionicon->setPixmap(m_iconunencrypted);		
				}
				
				if (s == "kb.issue.get")
				{
					if (site->GetTls() > 2) encryptionicon->setPixmap(m_iconencrypted);
					else encryptionicon->setPixmap(m_iconunencrypted);	
				
					*qstate = xfer;
					m_xferallsize = static_cast<transferitem*>(mp_view->TaskView->firstChild())->m_firemote.size();
					m_xferresumesize = 0;
					m_xfered = 0;
					m_time.start();
					startTimer(200);
				}
				else if (s == "kb.issue.getresume")
				{
					if (site->GetTls() > 2) encryptionicon->setPixmap(m_iconencrypted);
					else encryptionicon->setPixmap(m_iconunencrypted);
				
					*qstate = xfer;
					m_xferallsize = static_cast<transferitem*>(mp_view->TaskView->firstChild())->m_firemote.size();
					m_xferresumesize = static_cast<transferitem*>(mp_view->TaskView->firstChild())->m_filocal.size();
					m_xfered = 0;
					m_time.start();
					startTimer(200);
				}
				else if (s == "kb.issue.put")
				{
					if (site->GetTls() > 2) encryptionicon->setPixmap(m_iconencrypted);
					else encryptionicon->setPixmap(m_iconunencrypted);
				
					*qstate = xfer;
					m_xferallsize = static_cast<transferitem*>(mp_view->TaskView->firstChild())->m_filocal.size();
					m_xferresumesize = 0;
					m_xfered = 0;
					m_time.start();
					startTimer(200);
				}
				else if (s == "kb.issue.putresume")
				{
					if (site->GetTls() > 2) encryptionicon->setPixmap(m_iconencrypted);
					else encryptionicon->setPixmap(m_iconunencrypted);
				
					*qstate = xfer;
					m_xferallsize = static_cast<transferitem*>(mp_view->TaskView->firstChild())->m_filocal.size();
					m_xferresumesize = static_cast<transferitem*>(mp_view->TaskView->firstChild())->m_firemote.size();
					m_xfered = 0;
					m_time.start();
					startTimer(200);
				}
				else if (s.left(12) == "kb.issue.fxp")
				{
					mp_encryptionicon_a->setPixmap(m_iconunencrypted);
					mp_encryptionicon_b->setPixmap(m_iconunencrypted);
				
					//SetGuiStatus(occupied, A);
					//SetGuiStatus(occupied, B);
				}
				
				//SetGuiStatus(occupied, b);
				logwindow->setColor(white);
				logwindow->append(s.remove(0, 9));
			}
			else if (s.left(10) == "kb.failure")
			{
				if (site->GetTls() > 0) encryptionicon->setPixmap(m_iconencrypted);
				else encryptionicon->setPixmap(m_iconunencrypted);
			
				qWarning("kb.failure");
				logwindow->setColor(red);
				logwindow->append(*log);
				*log = "";
								
				if (s.left(16) == "kb.failure.fatal")
				{				
					//if (p == &m_proc_a) SLOT_ConnectButtonA();
					//else SLOT_ConnectButtonB();
				}
				else if (s.left(14) == "kb.failure.fxp")
				{							
					if (s == "kb.failure.fxp.put")  m_fxpstate = abort;
					else  
					{
						m_fxpstate = stopped;
						EndXfer();
					}
					Xfer();	
				}
				else
				{
					//SetGuiStatus(connected, b);
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
					//UpdateRemoteDisplay(s.remove(0,12), b);

					/* when the queue flag is on all items in dir are
					selected and Xfer() is called */
					
					if (*qstate > changedir)
					{
						qWarning("delete item: %s", mp_view->TaskView->firstChild()->text(0).latin1());
						delete mp_view->TaskView->firstChild();
					}
					
					if (*qstate == scanremote)
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
					else if (*qstate == scanlocal)
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
					else if (*qstate == scanfxp)
					{
						QListViewItemIterator it(browser);
						while ( it.current() )
						{
							it.current()->setSelected(true);
							++it;
						}
						if (p == &m_proc_a)
						{
							InsertMarkedItems(transferitem::fxp_a_to_b);
							//UpdateRemote(B);
						}
						else
						{
							InsertMarkedItems(transferitem::fxp_b_to_a);
							//UpdateRemote(A);
						}
					}
					
					if (*qstate > done)
					{
						*qstate = done;
						Xfer();
					}
				}
				else
				{
					statusline->setText("dir: " + s.remove(0, 7) + "kb");
				}
			}
			else if (s.left(7) == "kb.xfer")
			{
				if (s.left(12) == "kb.xfer.done")
				{
					EndXfer();
					Xfer();
				}
				else
				{
					m_xfered = s.remove(0, 8).toInt();
				}
			}
			else if (s.left(15) == "kb.fxpinit.pasv")
			{
				m_fxpstate = initpasv;
				m_fxpportinfo = s.remove(0, 16);
				Xfer();
			}
			else if (s.left(15) == "kb.fxpinit.port")
			{
				m_fxpstate = initport;
				Xfer();
			}
			else if (s.left(9) == "kb.fxpget")
			{
				*qstate = xfer;
				m_fxpstate = get;
				Xfer();
			}
			else if (s.left(9) == "kb.fxpput")
			{
				*qstate = xfer;
				m_fxpstate = put;
				Xfer();
			}
			else if (s.left(14) == "kb.fxpfinished")
			{
				if (m_fxpstate == put) 
				{
					m_fxpstate = waitsrc;
					Xfer();
				}
				if (m_fxpstate == waitsrc) 
				{
					//SetGuiStatus(connected, A);
					//SetGuiStatus(connected, B);
					
					m_fxpstate = stopped;
					EndXfer();
					Xfer();
				}
			}
			else if (s == "kb.abor")
			{
				m_fxpstate = stopped;
				EndXfer();
				Xfer();	
			}
			else if (s == "kb.quit")
			{
				/* on connection changes the task view has to be cleared */
				while (QListViewItem* tmpviewitem = mp_view->TaskView->firstChild()) delete tmpviewitem;
				
				//UpdateLocalDisplay(b);
				//SetGuiStatus(disconnected, b);
			}
		}
		else log->append(s + '\n');
	}
}

void Kasablanca::timerEvent(QTimerEvent*)
{
	if (!mp_view->TaskView->firstChild()) return;
	else mp_view->TaskView->firstChild()->setText(1, 
		"[" +
		QString::number((m_xfered + m_xferresumesize) >> 10) +
		"kb of " +
		QString::number(m_xferallsize >> 10) +
		"kb] [" +
		QString::number(((m_xfered + m_xferresumesize)* 100 ) / (m_xferallsize + 1)) + 
		"%] [" +
		QString::number(m_xfered / (m_time.elapsed() + 1)) +
		" kb/s]"
	);
}

void Kasablanca::EndXfer()
{
	if (mp_view->TaskView->firstChild()) delete mp_view->TaskView->firstChild();
	m_qstate_a = done;
	m_qstate_b = done;
}
