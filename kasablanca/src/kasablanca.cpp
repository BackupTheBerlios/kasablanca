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

#include "customconnectdialog.h"
#include "fileexistsdialog.h"
#include "bookmarks.h"
#include "bookmarkdialog.h"
#include "kbtransferdir.h"
#include "kbtransferfile.h"
#include "kbdirinfo.h"

#include "ftpsession.h"

#include "kasablanca.h"

using namespace std;

Kasablanca::Kasablanca() : KMainWindow( 0, "Kasablanca" ), mp_view(new KasablancaMainWindow(this))
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
	connect(mp_session_a, SIGNAL(gui_queueitems(KbDirInfo*, FtpSession*, FtpSession*, bool)), 
	 	SLOT(SLOT_QueueItems(KbDirInfo*, FtpSession*, FtpSession*, bool)));
	connect(mp_session_b, SIGNAL(gui_queueitems(KbDirInfo*, FtpSession*, FtpSession*, bool)), 
	 	SLOT(SLOT_QueueItems(KbDirInfo*, FtpSession*, FtpSession*, bool)));
	connect(mp_session_a, SIGNAL(gui_succeedtransfer(QListViewItem*)), SLOT(SLOT_NextTransfer(QListViewItem*)));
	connect(mp_session_b, SIGNAL(gui_succeedtransfer(QListViewItem*)), SLOT(SLOT_NextTransfer(QListViewItem*)));	
	
	 mp_session_a->Disconnect();
	 mp_session_b->Disconnect();
	 
	 /* /new stuff */
	
}

void Kasablanca::setupGUI() 
{
    mp_view->TaskView->setSorting(-1);
    mp_view->BrowserA->setSorting(-1);
    mp_view->BrowserB->setSorting(-1);

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

	m_rclickmenu_t.connectItem(Start, this, SLOT(SLOT_ProcessQueue()));
	m_rclickmenu_t.connectItem(Skip, this, SLOT(SLOT_SkipTasks()));
	
	//m_rclickmenu_t.setItemEnabled(Start, false);
	//m_rclickmenu_t.setItemEnabled(Skip, false);	 
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

void Kasablanca::SLOT_QueueItems(KbDirInfo *dir, FtpSession* src, FtpSession* dst, bool startqueue)
{
	QueueItemsRecurse(dir, src, dst);
	if ((mp_view->TaskView->firstChild()) && (startqueue)) static_cast<KbTransferItem*>(mp_view->TaskView->firstChild())->Init();
}

void Kasablanca::QueueItemsRecurse(KbDirInfo *dir, FtpSession* src, FtpSession* dst, QListViewItem* parent)
{
	list<KbFileInfo>* filelist;
	list<KbDirInfo*>* dirlist;
	QListViewItem *after; 	
	
	filelist = dir->Filelist();
	dirlist = dir->Dirlist();
		
	after = NULL;	
	for(list<KbFileInfo>::iterator fileIterator = filelist->begin(); fileIterator != filelist->end(); fileIterator++)
	{
		KbFileInfo *srcfi = new KbFileInfo(*fileIterator);
		KbFileInfo *dstfi = new KbFileInfo(*fileIterator);
		srcfi->SetDirPath(src->WorkingDir() + srcfi->dirPath());
		dstfi->SetDirPath(dst->WorkingDir() + dstfi->dirPath());
		if (parent) after = new KbTransferFile(parent, after, src, dst, srcfi, dstfi);
		else new KbTransferFile(mp_view->TaskView, mp_view->TaskView->lastItem(), src, dst, srcfi, dstfi);
	}
		
	after = NULL;
	for(list<KbDirInfo*>::iterator dirIterator = dirlist->begin(); dirIterator != dirlist->end(); dirIterator++)
	{
		KbFileInfo *srcfi = new KbFileInfo(*(*dirIterator));
		KbFileInfo *dstfi = new KbFileInfo(*(*dirIterator));
		srcfi->SetDirPath(src->WorkingDir() + srcfi->dirPath());
		dstfi->SetDirPath(dst->WorkingDir() + dstfi->dirPath());
		if (parent) after = new KbTransferDir(parent, after, src, dst, srcfi, dstfi);
		else after = new KbTransferDir(mp_view->TaskView, mp_view->TaskView->lastItem(), src, dst, srcfi, dstfi);
		QueueItemsRecurse(*dirIterator, src, dst, after);
	}
	delete dir;
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

void Kasablanca::timerEvent(QTimerEvent*)
{
	/*
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
	*/
}

void Kasablanca::SLOT_NextTransfer(QListViewItem* item)
{
	FtpSession *src, *dst;
	QListViewItem *parent, *next;
	src = static_cast<KbTransferItem*>(item)->SrcSession();
	dst = static_cast<KbTransferItem*>(item)->DstSession();
	next = item->nextSibling();
	parent = item->parent();
	if (item->childCount() == 0) // if the item contains no subelements - delete it, if it doesn't - start with the next
	{
		static_cast<KbTransferItem*>(item)->Finish();
		if (next) ProcessQueue(static_cast<KbTransferItem*>(next));
		else if (parent) SLOT_NextTransfer(parent);
		else 
		{
			src->SLOT_RefreshButton();
			dst->SLOT_RefreshButton();
		}			
	}
	else ProcessQueue(static_cast<KbTransferItem*>(item->firstChild()));	
}

// called from menu

void Kasablanca::SLOT_ProcessQueue()
{
	KbTransferItem *ti = static_cast<KbTransferItem*>(mp_view->TaskView->firstChild());
	if (ti) ProcessQueue(ti);
}

void Kasablanca::ProcessQueue(KbTransferItem* item)
{
	FtpSession *srcsession = item->SrcSession();
	srcsession->Transfer(item);
}
