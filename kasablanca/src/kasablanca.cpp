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
      m_view(new KasablancaMainWindow(this))
{

	/* new stuff */
	mp_session_a = new FtpSession(this, "session a");
	/* / new stuff */
	
    setAcceptDrops(true);
    setCentralWidget(m_view);
    setupGUI();
    setupActions();
    setupStatusBar();
    setupMenu();
    statusBar()->show();
    //toolBar()->show();
    
    UpdateLocalDisplay(A);
    UpdateLocalDisplay(B);
    SetGuiStatus(disconnected, A);
    SetGuiStatus(disconnected, B);

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

  if (locate("exe", "kbftp") == QString::null) KMessageBox::error(0,"kbftp binary is not in kde binary path.");
    else 
    {
            m_proc_a.addArgument(locate("exe", "kbftp"));
            m_proc_b.addArgument(locate("exe", "kbftp"));
    }

    m_dcount = 0;
    m_qstate_a = done; m_qstate_b = done;
    m_fxpstate = stopped;
    m_fxpportinfo = "";

    connect(&m_proc_a, SIGNAL(readyToRead(kbprocess*)), this, SLOT(SLOT_KbftpReadReady(kbprocess*)));
    connect(&m_proc_b, SIGNAL(readyToRead(kbprocess*)), this, SLOT(SLOT_KbftpReadReady(kbprocess*)));
	 //connect(m_view->ConnectButtonA, SIGNAL(clicked()), SLOT(SLOT_ConnectButtonA()));
    //connect(m_view->CommandLineA, SIGNAL(returnPressed()), SLOT(SLOT_EnterCommandA()));
    //connect(m_view->CwdLineA, SIGNAL(returnPressed()), SLOT(SLOT_EnterCwdA()));
    connect(m_view->CommandLineB, SIGNAL(returnPressed()), SLOT(SLOT_EnterCommandB()));
    connect(m_view->BrowserB, SIGNAL(selectionChanged()), SLOT(SLOT_SelectionChanged()));
    connect(m_view->BrowserA, SIGNAL(selectionChanged()), SLOT(SLOT_SelectionChanged()));
    connect(m_view->CwdLineB, SIGNAL(returnPressed()), SLOT(SLOT_EnterCwdB()));
    connect(m_view->TaskView, SIGNAL(selectionChanged()), SLOT(SLOT_SelectionChanged()));
    connect(m_view->TransferButtonA, SIGNAL(clicked()), SLOT(SLOT_TransferA()));
    connect(m_view->TransferButtonB, SIGNAL(clicked()), SLOT(SLOT_TransferB()));
    connect(m_view->RefreshButtonB, SIGNAL(clicked()), SLOT(SLOT_RefreshBrowserB()));
    //connect(m_view->RefreshButtonA, SIGNAL(clicked()), SLOT(SLOT_RefreshBrowserA()));
    connect(m_view->ConnectButtonB, SIGNAL(clicked()), SLOT(SLOT_ConnectButtonB()));
	 
	 /* new stuff */
	 
	 mp_session_a->SetLogWindow(m_view->LogWindowA);
	 mp_session_a->SetBrowser(m_view->BrowserA);
	 mp_session_a->SetConnectButton(m_view->ConnectButtonA);
	 mp_session_a->SetRefreshButton(m_view->RefreshButtonA);
	 mp_session_a->SetCmdLine(m_view->CommandLineA);
	 mp_session_a->SetCwdLine(m_view->CwdLineA);
	 mp_session_a->SetBookmarksMenu(&m_bookmarksmenu_a);
	 mp_session_a->SetStatusLine(mp_statusline_a);
	 mp_session_a->SetEncryptionIcon(mp_encryptionicon_a);
	 mp_session_a->SetRclickMenu(&m_rclickmenu_a);
	 
	 connect(m_view->RefreshButtonA, SIGNAL(clicked()), mp_session_a, SLOT(SLOT_RefreshButton()));
	 connect(m_view->ConnectButtonA, SIGNAL(clicked()), mp_session_a, SLOT(SLOT_ConnectButton()));
	 connect(m_view->CwdLineA, SIGNAL(returnPressed()), mp_session_a, SLOT(SLOT_CwdLine()));
	 connect(m_view->CommandLineA, SIGNAL(returnPressed()), mp_session_a, SLOT(SLOT_CmdLine()));
	 
	 /* /new stuff */
	
}

void Kasablanca::setupGUI() 
{
    m_view->TaskView->setSorting(-1);
    m_view->BrowserA->setSorting(-1);
    m_view->BrowserB->setSorting(-1);

    mp_header_a = m_view->BrowserA->header();
    mp_header_b = m_view->BrowserB->header();

    m_view->TransferButtonA->setIconSet(KGlobal::iconLoader()->loadIconSet("forward",KIcon::Toolbar));
    m_view->TransferButtonB->setIconSet(KGlobal::iconLoader()->loadIconSet("back",KIcon::Toolbar));
    m_view->RefreshButtonA->setIconSet(KGlobal::iconLoader()->loadIconSet("reload",KIcon::Toolbar));
    m_view->RefreshButtonB->setIconSet(KGlobal::iconLoader()->loadIconSet("reload",KIcon::Toolbar));
    m_view->ConnectButtonA->setIconSet(KGlobal::iconLoader()->loadIconSet("connect_no",KIcon::Toolbar));
    m_view->ConnectButtonB->setIconSet(KGlobal::iconLoader()->loadIconSet("connect_no",KIcon::Toolbar));
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
    KKeyDialog::configure(actionCollection(), m_view);
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
	connect(m_view->BrowserA, SIGNAL(doubleClicked(QListViewItem*)), mp_session_a, SLOT(SLOT_ItemClicked(QListViewItem*)));
	connect(m_view->BrowserA, SIGNAL(rightButtonPressed(QListViewItem *, const QPoint &, int)), 
		mp_session_a, SLOT(SLOT_ItemRClicked(QListViewItem *, const QPoint &, int)));
	
	/* /new stuff */
	
	//connect( &m_bookmarksmenu_a, SIGNAL( activated(int) ), this, SLOT( SLOT_ConnectA(int) ) );
	connect( &m_bookmarksmenu_b, SIGNAL( activated(int) ), this, SLOT( SLOT_ConnectB(int) ) );
		
	//connect( m_view->BrowserA, SIGNAL (doubleClicked(QListViewItem*) ), this, SLOT ( SLOT_ItemClickedA(QListViewItem*) ) );
	connect( m_view->BrowserB, SIGNAL (doubleClicked(QListViewItem*) ), this, SLOT ( SLOT_ItemClickedB(QListViewItem*) ) );

	//connect( m_view->BrowserA, SIGNAL (rightButtonPressed( QListViewItem *, const QPoint &, int )), this,
	//		SLOT (SLOT_ItemRightClickedA(QListViewItem *, const QPoint &, int )));
	connect( m_view->BrowserB, SIGNAL (rightButtonPressed( QListViewItem *, const QPoint &, int )), this,
			SLOT (SLOT_ItemRightClickedB(QListViewItem *, const QPoint &, int )));
		
	connect( m_view->TaskView, SIGNAL (rightButtonPressed( QListViewItem *, const QPoint &, int )), this,
			SLOT (SLOT_ItemRightClickedT(QListViewItem *, const QPoint &, int )));
		
	connect( mp_header_b, SIGNAL (clicked (int )), this, SLOT (SLOT_HeaderBClicked(int)));
	connect( mp_header_a, SIGNAL (clicked (int )), this, SLOT (SLOT_HeaderAClicked(int)));

	//m_rclickmenu_a.connectItem(Transfer, this, SLOT(SLOT_TransferA()));
	m_rclickmenu_b.connectItem(Transfer, this, SLOT(SLOT_TransferB()));

	//m_rclickmenu_a.connectItem(Queue, this, SLOT(SLOT_QueueA()));
	m_rclickmenu_b.connectItem(Queue, this, SLOT(SLOT_QueueB()));

	//m_rclickmenu_a.connectItem(Delete, this, SLOT(SLOT_DeleteA()));
	m_rclickmenu_b.connectItem(Delete, this, SLOT(SLOT_DeleteB()));

	//m_rclickmenu_a.connectItem(Rename, this, SLOT(SLOT_RenameA()));
	m_rclickmenu_b.connectItem(Rename, this, SLOT(SLOT_RenameB()));

	//m_rclickmenu_a.connectItem(Mkdir, this, SLOT(SLOT_MkdirA()));
	m_rclickmenu_b.connectItem(Mkdir, this, SLOT(SLOT_MkdirB()));

	m_rclickmenu_t.connectItem(Start, this, SLOT(SLOT_StartQueue()));

	m_rclickmenu_t.connectItem(Skip, this, SLOT(SLOT_SkipTasks()));

	//m_rclickmenu_a.setItemEnabled(Transfer, false);
	//m_rclickmenu_a.setItemEnabled(Queue, false);
	//m_rclickmenu_a.setItemEnabled(Delete, false);
	//m_rclickmenu_a.setItemEnabled(Rename, false);
	//m_rclickmenu_a.setItemEnabled(Mkdir, false);
	
	m_rclickmenu_b.setItemEnabled(Transfer, false);
	m_rclickmenu_b.setItemEnabled(Queue, false);
	m_rclickmenu_b.setItemEnabled(Delete, false);
	m_rclickmenu_b.setItemEnabled(Rename, false);
	m_rclickmenu_b.setItemEnabled(Mkdir, false);
	
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
    
    /* on connection changes the task view has to be cleared */

    while (QListViewItem* tmpviewitem = m_view->TaskView->firstChild()) delete tmpviewitem;
	
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

        if (site->GetPasv() == 1) proc->writeStdin("setpasv");
        else proc->writeStdin("setport");

        proc->writeStdin("connect");
        proc->writeStdin(QString(site->GetInfo()));
        if (site->GetTls() > 0) proc->writeStdin("negotiateencryption");
        proc->writeStdin("login");
        proc->writeStdin(QString(site->GetUser()));
        proc->writeStdin(QString(site->GetPass()));

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
	QToolButton* transferbutton, *connectbutton, /**connectbutton_other,*/ *refreshbutton;
	QLineEdit* commandline, *cwdline;
	QPopupMenu* bookmarksmenu, *rclickmenu;
	QLabel* encryptionicon, *mp_statusline;
		
	if (b == A)
	{
		m_status_a = s;
		mp_statusline = mp_statusline_a;
		
		encryptionicon = mp_encryptionicon_a;
		browser = m_view->BrowserA;
		transferbutton = m_view->TransferButtonA;
		connectbutton = m_view->ConnectButtonA;
		refreshbutton = m_view->RefreshButtonA;
		commandline = m_view->CommandLineA;
		cwdline = m_view->CwdLineA;
		bookmarksmenu = &m_bookmarksmenu_a;
		rclickmenu = &m_rclickmenu_a;
	}
	else
	{
		m_status_b = s;
		mp_statusline = mp_statusline_b;
//		mp_statusline_b->setText("");
		
		encryptionicon = mp_encryptionicon_b;
		browser = m_view->BrowserB;
		transferbutton = m_view->TransferButtonB;
		connectbutton = m_view->ConnectButtonB;
		refreshbutton = m_view->RefreshButtonB;
		commandline = m_view->CommandLineB;
 		cwdline = m_view->CwdLineB;
		bookmarksmenu = &m_bookmarksmenu_b;
		rclickmenu = &m_rclickmenu_b;
	}
	
        switch (s)
	{
   	case connected:

			mp_statusline->setText(i18n("Connected"));
                        browser->setEnabled(true);
			transferbutton->setEnabled(false);
			connectbutton->setEnabled(true);
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

			mp_statusline->setText(i18n("Disconnected"));
			encryptionicon->setPixmap(m_iconunencrypted);
			browser->setEnabled(true);
			transferbutton->setEnabled(false);
			connectbutton->setEnabled(true);
			connectbutton->setIconSet(KGlobal::iconLoader()->loadIconSet("connect_no",KIcon::Toolbar));
			refreshbutton->setEnabled(true);
			commandline->setEnabled(true);
			cwdline->setEnabled(true);
			bookmarksmenu->setEnabled(true);
			rclickmenu->setEnabled(true);
			m_rclickmenu_t.setItemEnabled(Start, true);
			m_rclickmenu_t.setItemEnabled(Skip, true);
			rclickmenu->setItemEnabled(Mkdir, true);
			break;

		case occupied:
		
			mp_statusline->setText(i18n("Occupied"));
			browser->setEnabled(false);
			transferbutton->setEnabled(false);
			connectbutton->setEnabled(true);
			//connectbutton_other->setEnabled(false);
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
        initBookmarks();
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
	
	/* on connection changes the task view has to be cleared */

	while (QListViewItem* tmpviewitem = m_view->TaskView->firstChild()) delete tmpviewitem;
			
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

	if (site->GetPasv() == 1) proc->writeStdin("setpasv");
	else proc->writeStdin("setport");

	proc->writeStdin("connect");
	proc->writeStdin(QString(site->GetInfo()));
	if (site->GetTls() > 0) proc->writeStdin("negotiateencryption");
	proc->writeStdin("login");
	proc->writeStdin(QString(site->GetUser()));
	proc->writeStdin(QString(site->GetPass()));

	UpdateRemote(b);
}

void Kasablanca::SLOT_EnterCommandA()
{
	if (m_status_a == connected)
	{
		m_proc_a.writeStdin("raw");
		m_proc_a.writeStdin(m_view->CommandLineA->text());
		m_view->CommandLineA->setText("");
	}
	else if (m_status_a == disconnected)
	{
		KProcess* p = new KProcess();
		connect( p, SIGNAL(processExited (KProcess*)), this, SLOT(SLOT_ProcessExited(KProcess*)));
 		p->setWorkingDirectory(m_currentlocaldir_a.absPath());
		*p << QStringList::split(" ", m_view->CommandLineA->text());
		if (p->start() == TRUE) m_view->CommandLineA->setText("");
	}
}

void Kasablanca::SLOT_EnterCwdA()
{
	if (m_status_a == connected)
	{
		m_proc_a.writeStdin("chdir");
		m_proc_a.writeStdin(m_view->CwdLineA->text());
		UpdateRemote(A);
	}
	else if (m_status_a == disconnected)
	{
		m_currentlocaldir_a.cd(m_view->CwdLineA->text());
    	UpdateLocalDisplay(A);
	}
}

void Kasablanca::SLOT_ConnectButtonA()
{
	if (m_qstate_b != done) 
	{
		int x;
		x = KMessageBox::warningYesNo(this,"Cancel transfer?");
		if (x == KMessageBox::Yes) SLOT_ConnectButtonB();
		else return;
	}
	
	while (QListViewItem* tmpviewitem = m_view->TaskView->firstChild()) delete tmpviewitem;
	 
	if (m_status_a == disconnected)
	{
		m_bookmarksmenu_a.exec(mapToGlobal(m_view->ConnectButtonA->pos()));
	}
	else if (m_status_a == occupied)
	{
		m_qstate_a = done;
		m_proc_a.tryTerminate();
		m_proc_a.kill();	
		UpdateLocalDisplay(A);
		SetGuiStatus(disconnected, A);
		m_view->LogWindowA->setColor(red);
		m_view->LogWindowA->append("killed ftp connection");
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
	if (m_qstate_a != done) 
	{
		int x;
		x = KMessageBox::warningYesNo(this,"Cancel transfer?");
		if (x == KMessageBox::Yes) SLOT_ConnectButtonA();
		else return;
	}

	while (QListViewItem* tmpviewitem = m_view->TaskView->firstChild()) delete tmpviewitem;

	if (m_status_b == disconnected)
	{
		m_bookmarksmenu_b.exec(mapToGlobal(m_view->ConnectButtonB->pos()));
	}
	else if (m_status_b == occupied)
	{
		m_qstate_b = done;
		m_proc_b.tryTerminate();
		m_proc_b.kill();
		UpdateLocalDisplay(B);
		SetGuiStatus(disconnected, B);
		m_view->LogWindowB->setColor(red);
		m_view->LogWindowB->append("killed ftp connection");
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

void Kasablanca::SLOT_ItemClickedB(QListViewItem * item)
{
	if (m_status_b == connected)
	{
		if (item->text(0) == "..") m_proc_b.writeStdin("cdup");
		else
		{
			m_proc_b.writeStdin("chdir");
			m_proc_b.writeStdin(item->text(0));
		}
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
		else
		{
			m_proc_a.writeStdin("chdir");
			m_proc_a.writeStdin(item->text(0));
		}
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
		m_proc_b.writeStdin("raw");
		m_proc_b.writeStdin(m_view->CommandLineB->text());
		m_view->CommandLineB->setText("");
	}
	else if (m_status_b == disconnected)
	{
		KProcess* p = new KProcess();
		connect( p, SIGNAL(processExited (KProcess*)), this, SLOT(SLOT_ProcessExited(KProcess*)));
 		p->setWorkingDirectory(m_currentlocaldir_b.absPath());
		*p << QStringList::split(" ", m_view->CommandLineB->text());
		if (p->start() == TRUE) m_view->CommandLineB->setText("");
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
		browser = m_view->BrowserA;
		localdir = &m_currentlocaldir_a;
		transferbutton = m_view->TransferButtonA;
		cwdline = m_view->CwdLineA;
	}
	else 
	{
		browser = m_view->BrowserB;
		localdir = &m_currentlocaldir_b;
		transferbutton = m_view->TransferButtonB;
		cwdline = m_view->CwdLineB;
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
    QListViewItemIterator it(m_view->TaskView);
    while ( it.current() )
    {
        if (it.current()->isSelected()) delete it.current();
        else ++it;
    }
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

    kapp->exit();
}

void Kasablanca::SLOT_DeleteB()
{
	if (m_status_b == connected)
	{
		QListViewItemIterator it(m_view->BrowserB);
		while (it.current())
		{
			if (it.current()->isSelected())
			{
				QListViewItem* item = it.current();
	
				if (item->rtti() == 1001)
				{
					m_proc_b.writeStdin("rmdir");
					m_proc_b.writeStdin(static_cast<diritem*>(item)->m_file);
				}
				else if (item->rtti() == 1002)
				{
					m_proc_b.writeStdin("delete");
					m_proc_b.writeStdin(static_cast<fileitem*>(item)->m_file);
				}
			}
			it++;
		}
		UpdateRemote(B);
	}
	else if (m_status_b == disconnected)
	{
		QListViewItemIterator it(m_view->BrowserB);
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
		QListViewItemIterator it(m_view->BrowserA);
		while (it.current())
		{
			if (it.current()->isSelected())
			{
				QListViewItem* item = it.current();

				if (item->rtti() == 1001)
				{
					m_proc_a.writeStdin("rmdir");
					m_proc_a.writeStdin(static_cast<diritem*>(item)->m_file);
				}
				else if (item->rtti() == 1002)
				{
					m_proc_a.writeStdin("delete");
					m_proc_a.writeStdin(static_cast<fileitem*>(item)->m_file);
				}
			}
			it++;
		}
		UpdateRemote(A);
	}
	else if (m_status_a == disconnected)
	{
		QListViewItemIterator it(m_view->BrowserA);
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
			QListViewItemIterator it(m_view->BrowserA);
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
						lastitem = new transferdir(m_view->TaskView, lastitem, filocal, firemote, t);
					}
					else if (item->rtti() == 1002)
					{
						qWarning("download a file from a to b...");
						lastitem = new transferfile(m_view->TaskView, lastitem, filocal, firemote, t);
					}
					item->setSelected(false);	
				}
				++it;
			}
			break;
		}
		case transferitem::download_b_to_a:
		{
			QListViewItemIterator it(m_view->BrowserB);
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
						lastitem = new transferdir(m_view->TaskView, lastitem, filocal, firemote, t);
					}
					else if (item->rtti() == 1002)
					{
						qWarning("download a file from b to a...");
						lastitem = new transferfile(m_view->TaskView, lastitem, filocal, firemote, t);
					}
					item->setSelected(false);	
				}
				++it;
			}
			break;
		}
		case transferitem::upload_a_to_b:
		{
			QListViewItemIterator it(m_view->BrowserA);
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
						lastitem = new transferdir(m_view->TaskView, lastitem, filocal, firemote, t);
					}
					else if (item->rtti() == 1002)
					{
						qWarning("upload a file from a to b...");
						lastitem = new transferfile(m_view->TaskView, lastitem, filocal, firemote, t);
					}
					item->setSelected(false);	
				}
				++it;
			}	
			break;
		}
		case transferitem::upload_b_to_a:
		{
			QListViewItemIterator it(m_view->BrowserB);
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
						lastitem = new transferdir(m_view->TaskView, lastitem, filocal, firemote, t);
					}
					else if (item->rtti() == 1002)
					{
						qWarning("upload a file from b to a...");
						lastitem = new transferfile(m_view->TaskView, lastitem, filocal, firemote, t);
					}
					item->setSelected(false);	
				}
				++it;
			}	
			break;
		}
		case transferitem::fxp_a_to_b:
		{
			QListViewItemIterator it(m_view->BrowserA);
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
						lastitem = new transferdir(m_view->TaskView, lastitem, fifxpsrc, fifxpdst, t);
					}
					else if (item->rtti() == 1002)
					{
						qWarning("fxp a file from a to b...");
						lastitem = new transferfile(m_view->TaskView, lastitem, fifxpsrc, fifxpdst, t);
					}
					item->setSelected(false);
				}
				++it;
			}
			break;
		}
		case transferitem::fxp_b_to_a:
		{
			QListViewItemIterator it(m_view->BrowserB);
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
						lastitem = new transferdir(m_view->TaskView, lastitem, fifxpsrc, fifxpdst, t);
					}
					else if (item->rtti() == 1002)
					{
						qWarning("fxp a file from b to a...");
						lastitem = new transferfile(m_view->TaskView, lastitem, fifxpsrc, fifxpdst, t);
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
	kbprocess* proc, *proc_fxpsrc, *proc_fxpdst;
	QString remotedir, dir_fxpsrc, dir_fxpdst;
	QDir* localdir;
	Browser remotebrowser, localbrowser, browser_fxpsrc, browser_fxpdst;
	siteinfo* site, *site_fxpsrc, *site_fxpdst;
	QListView* remoteview, *view_fxpsrc, *view_fxpdst;
	QueueState* qstate, *qstate_fxpsrc, *qstate_fxpdst;

	if (m_view->TaskView->childCount() == 0)
	{
		SLOT_RefreshBrowserA();
		SLOT_RefreshBrowserB();
		return;
	}
	else
	{
		m_view->TaskView->firstChild()->setSelected(false);
		m_view->TaskView->firstChild()->setSelectable(false);
	}

	transferitem* item = static_cast<transferitem*>(m_view->TaskView->firstChild());

	qWarning("item: %s", item->text(0).latin1());

	if ((item->type() == transferitem::download_a_to_b) || (item->type() == transferitem::upload_b_to_a))
	{
		site = &m_site_a;
		proc = &m_proc_a;
		remotebrowser = A;
		localbrowser = B;
		remoteview = m_view->BrowserA;
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
		remoteview = m_view->BrowserB;
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
		view_fxpsrc = m_view->BrowserA;
		view_fxpdst = m_view->BrowserB;
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
		view_fxpsrc = m_view->BrowserB;
		view_fxpdst = m_view->BrowserA;
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
			UpdateRemote(browser_fxpsrc);
			return;
		}
		else if (item->m_fifxpdst.dirPath() != dir_fxpdst)
		{
			qWarning("in the wrong path, change to %s", item->m_fifxpdst.dirPath().latin1());
			*qstate_fxpdst = changedir;

			proc_fxpdst->writeStdin("chdir");
			proc_fxpdst->writeStdin(item->m_fifxpdst.dirPath());
			UpdateRemote(browser_fxpdst);
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
			UpdateRemote(remotebrowser);
			return;
		}

		localdir->cd(item->m_filocal.dirPath());
		UpdateLocalDisplay(localbrowser);
	}

	// handle items

	if (item->rtti() == transferitem::dir)
	{
		qWarning("...is a dir");
		transferdir* dir = static_cast<transferdir*>(m_view->TaskView->firstChild());

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

			UpdateLocalDisplay(localbrowser);
			UpdateRemote(remotebrowser);
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

			UpdateLocalDisplay(localbrowser);
			UpdateRemote(remotebrowser);
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
			
			UpdateRemote(A);
			UpdateRemote(B);
		}
	}
	else if (item->rtti() == transferitem::file)
	{
		qWarning("...is a file");
		transferfile* file = static_cast<transferfile*>(m_view->TaskView->firstChild());

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
	qWarning("slot: SLOT_SelectionChanged()");

	int counter_a = 0;
	int counter_b = 0;
	bool flag;

	QListViewItemIterator ita(m_view->BrowserA);
 	while ( ita.current() )
 	{
   	if (ita.current()->isSelected()) counter_a++;
   	++ita;
 	}

	QListViewItemIterator itb(m_view->BrowserB);
 	while ( itb.current() )
 	{
   	if (itb.current()->isSelected()) counter_b++;
   	++itb;
 	}
	
	//flag = ((counter_a != 0) && ((m_status_a == connected) || (m_status_b == connected)));

	m_rclickmenu_a.setItemEnabled(Transfer, false);
	m_rclickmenu_a.setItemEnabled(Queue, false);
	m_view->TransferButtonA->setEnabled(false);
	
	/* when exactly 1 item is selected then rename is enabled,
	when at least one item is selected, Delete is enabled. */
	
	m_rclickmenu_a.setItemEnabled(Rename, (counter_a == 1));
	m_rclickmenu_a.setItemEnabled(Delete, (counter_a >= 1));
	
	flag = ((counter_b != 0) && ((m_status_a == connected) || (m_status_b == connected)));

	m_rclickmenu_b.setItemEnabled(Transfer, flag);
	m_rclickmenu_b.setItemEnabled(Queue, flag);
	m_view->TransferButtonB->setEnabled(flag);
	
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
		InsertMarkedItems(transferitem::fxp_a_to_b);
		Xfer();
		//KMessageBox::error(0,"site to site transfer (fxp) is not supported yet.");
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
		InsertMarkedItems(transferitem::download_a_to_b, m_view->TaskView->lastItem()); 
	}
	else if ((m_status_a == disconnected) && (m_status_b == connected))
	{
		InsertMarkedItems(transferitem::upload_a_to_b, m_view->TaskView->lastItem()); 
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
		InsertMarkedItems(transferitem::upload_b_to_a, m_view->TaskView->lastItem());
	}
	else if ((m_status_a == disconnected) && (m_status_b == connected))
	{
		InsertMarkedItems(transferitem::download_b_to_a, m_view->TaskView->lastItem());
	}	
	else if ((m_status_a == connected) && (m_status_b == connected))
	{
		InsertMarkedItems(transferitem::fxp_b_to_a);
		//KMessageBox::error(0,"site to site transfer (fxp) is not supported yet.");
	}
}

void Kasablanca::SLOT_StartQueue(){ Xfer();}

void Kasablanca::SLOT_EnterCwdB()
{
	if (m_status_b == connected)
	{
		m_proc_b.writeStdin("chdir");
		m_proc_b.writeStdin(m_view->CwdLineB->text());;
		UpdateRemote(B);
	}
	else if (m_status_b == disconnected)
	{
		m_currentlocaldir_b.cd(m_view->CwdLineB->text());
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
	if (site->GetTls() >= 2)
	{
		proc->writeStdin("setdataencryption");
		proc->writeStdin("on");
	}
	proc->writeStdin("dir");
	proc->writeStdin(QDir::homeDirPath() + "/.kasablanca/" + m_tempdirname + "." + QString::number(m_dcount++));
}

void Kasablanca::SLOT_HeaderBClicked(int section)
{
	m_sortascending_b = m_sortascending_b ^ true;

	QListViewItem* x = m_view->BrowserB->firstChild();
	m_view->BrowserB->takeItem(x);

	m_view->BrowserB->setSorting(section, m_sortascending_b);
	m_view->BrowserB->sort();
	m_view->BrowserB->setSorting(-1);

	m_view->BrowserB->insertItem(x);

}

void Kasablanca::SLOT_HeaderAClicked(int section)
{
	m_sortascending_a = m_sortascending_a ^ true;

	QListViewItem* x = m_view->BrowserA->firstChild();
	m_view->BrowserA->takeItem(x);

	m_view->BrowserA->setSorting(section, m_sortascending_a);
	m_view->BrowserA->sort();
	m_view->BrowserA->setSorting(-1);

	m_view->BrowserA->insertItem(x);

}

void Kasablanca::SLOT_MkdirA()
{
	bool b;
	QString name;

	#if KDE_IS_VERSION(3,2,0)
	name = KInputDialog::getText("Enter Directory Name:", "Enter Directory Name:", "", &b, this);
	#else
	name = KLineEditDlg::getText("Enter Directory Name:", "", &b, this);
	#endif

	if (m_status_a == connected)
	{
		if (!b) return;
		m_proc_a.writeStdin("mkdir");
		m_proc_a.writeStdin(name);
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

	#if KDE_IS_VERSION(3,2,0)
	name = KInputDialog::getText("Enter Directory Name:", "Enter Directory Name:", "", &b, this);
	#else
	name = KLineEditDlg::getText("Enter Directory Name:", "", &b, this);
	#endif

	if (m_status_b == connected)
	{
		if (!b) return;
		m_proc_b.writeStdin("mkdir");
		m_proc_b.writeStdin(name);
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
		cwdline = m_view->CwdLineA;
		browser = m_view->BrowserA;
		remotedir = m_currentremotedir_a;
	}
	else
	{
		cwdline = m_view->CwdLineB;
		browser = m_view->BrowserB;
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
	QueueState* qstate;
	QTextEdit* logwindow;
	QLabel* statusline, *encryptionicon;
	siteinfo* site;

	if (p == &m_proc_a)
	{
		site = &m_site_a;
		encryptionicon = mp_encryptionicon_a;
		statusline = mp_statusline_a;
		logwindow = m_view->LogWindowA;
		log = &m_log_a;
		b = A;
		browser = m_view->BrowserA;
		otherbrowser = m_view->BrowserB;
		remotedir = &m_currentremotedir_a;
		qstate = &m_qstate_a;
	}
	else
	{
		site = &m_site_b;
		encryptionicon = mp_encryptionicon_b;
		statusline = mp_statusline_b;
		logwindow = m_view->LogWindowB;
		log = &m_log_b;
		b = B;
		browser = m_view->BrowserB;
		otherbrowser = m_view->BrowserA;
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
				SetGuiStatus(connected, b);
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
					m_xferallsize = static_cast<transferitem*>(m_view->TaskView->firstChild())->m_firemote.size();
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
					m_xferallsize = static_cast<transferitem*>(m_view->TaskView->firstChild())->m_firemote.size();
					m_xferresumesize = static_cast<transferitem*>(m_view->TaskView->firstChild())->m_filocal.size();
					m_xfered = 0;
					m_time.start();
					startTimer(200);
				}
				else if (s == "kb.issue.put")
				{
					if (site->GetTls() > 2) encryptionicon->setPixmap(m_iconencrypted);
					else encryptionicon->setPixmap(m_iconunencrypted);
				
					*qstate = xfer;
					m_xferallsize = static_cast<transferitem*>(m_view->TaskView->firstChild())->m_filocal.size();
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
					m_xferallsize = static_cast<transferitem*>(m_view->TaskView->firstChild())->m_filocal.size();
					m_xferresumesize = static_cast<transferitem*>(m_view->TaskView->firstChild())->m_firemote.size();
					m_xfered = 0;
					m_time.start();
					startTimer(200);
				}
				else if (s.left(12) == "kb.issue.fxp")
				{
					mp_encryptionicon_a->setPixmap(m_iconunencrypted);
					mp_encryptionicon_b->setPixmap(m_iconunencrypted);
				
					SetGuiStatus(occupied, A);
					SetGuiStatus(occupied, B);
				}
				
				SetGuiStatus(occupied, b);
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
					if (p == &m_proc_a) SLOT_ConnectButtonA();
					else SLOT_ConnectButtonB();
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
					
					if (*qstate > changedir)
					{
						qWarning("delete item: %s", m_view->TaskView->firstChild()->text(0).latin1());
						delete m_view->TaskView->firstChild();
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
							UpdateRemote(B);
						}
						else
						{
							InsertMarkedItems(transferitem::fxp_b_to_a);
							UpdateRemote(A);
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
					SetGuiStatus(connected, A);
					SetGuiStatus(connected, B);
					
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
				while (QListViewItem* tmpviewitem = m_view->TaskView->firstChild()) delete tmpviewitem;
				
				UpdateLocalDisplay(b);
				SetGuiStatus(disconnected, b);
			}
		}
		else log->append(s + '\n');
	}
}

void Kasablanca::SLOT_RenameA()
{
	QListViewItem* item = 0;
	bool b;
	QString name;

	QListViewItemIterator it(m_view->BrowserA);
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
	
	#if KDE_IS_VERSION(3,2,0)
	name = KInputDialog::getText("Enter New Name:", "Enter New Name:", item->text(0), &b, this);
	#else
	name = KLineEditDlg::getText("Enter New Name:", item->text(0), &b, this);	
	#endif
	
	if (!b) return;
	
	if (m_status_a == connected)
	{
		m_proc_a.writeStdin("rename");
		m_proc_a.writeStdin(item->text(0));
		m_proc_a.writeStdin(name);
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

	QListViewItemIterator it(m_view->BrowserB);
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
	
	#if KDE_IS_VERSION(3,2,0)
	name = KInputDialog::getText("Enter New Name:", "Enter New Name:", item->text(0), &b, this);
	#else
	name = KLineEditDlg::getText("Enter New Name:", item->text(0), &b, this);	
	#endif
	
	if (!b) return;
	
	if (m_status_b == connected)
	{
		m_proc_b.writeStdin("rename");
		m_proc_b.writeStdin(item->text(0));
		m_proc_b.writeStdin(name);
		UpdateRemote(B);
	}
	else if (m_status_b == disconnected)
	{
		m_currentlocaldir_b.rename(item->text(0), name);
		UpdateLocalDisplay(B);
	}
}

void Kasablanca::timerEvent(QTimerEvent*)
{
	if (!m_view->TaskView->firstChild()) return;
	else m_view->TaskView->firstChild()->setText(1, 
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
	if (m_view->TaskView->firstChild()) delete m_view->TaskView->firstChild();
	m_qstate_a = done;
	m_qstate_b = done;
}
