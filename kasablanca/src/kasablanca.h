/***************************************************************************
                          kasablanca.h  -  description
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

#ifndef KASABLANCA_H
#define KASABLANCA_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

class diritem;
class fileitem;
class KToggleAction;
class QWidget;
class QCloseEvent;
class QLabel;
class QPixmap;
class FtpSession;
class kbitem;

#include <kapplication.h>
#include <klistview.h>
#include <kmainwindow.h>
#include <kprocess.h>

#include <qdir.h>
#include <qlistview.h>
#include <qmenubar.h>

#include <vector>

#include "bookmarks.h"
#include "transferitem.h"
#include "siteinfo.h"
#include "kbprocess.h"
#include "remotefileinfo.h"
#include "Q_mainwindow.h"

/** Kasablanca is the base class of the project */

class Kasablanca : public KMainWindow
{
    Q_OBJECT
public:
    enum RightClickMenu
    {
        Transfer = 1001,
        Queue,
        Delete,
        Rename,
        Mkdir,
        Start,
        Skip
    };
    
    enum FxpState
    {
        stopped = 0,
        initpasv,
        initport,
        get,
        put,
        waitsrc,
        abort
    };

    enum QueueState
    {
        done = 0,
        changedir,
        proceed,
        scanremote,
        scanlocal,
        scanfxp,
        xfer
    };

    enum State
    {
        disconnected = 0,
        connected,
        occupied
    };

    enum TransferMethod
    {
        download = 0,
        upload,
        fxp
    };
    
    enum Browser
    {
        A = 0,
        B
    };

    Kasablanca();
    ~Kasablanca();

    QString m_tempdirname;
    QString m_version;
	 vector<siteinfo> m_bookmarks;

public slots:
    void SLOT_EditBookmarks();
    void SLOT_ItemRightClickedT(QListViewItem * item, const QPoint & point, int col );
    void SLOT_TransferA();
    void SLOT_TransferB();
    void SLOT_QueueA();
    void SLOT_QueueB();
    void SLOT_SelectionChanged();
    void SLOT_StartQueue();
    void SLOT_SkipTasks();
    void SLOT_KbftpReadReady(kbprocess* p);
	 void SLOT_QueueItems(list<kbitem*> items, FtpSession* src, FtpSession* dst, bool start);
	 void SLOT_ProcessQueue();

private slots:
    void optionsShowToolbar();
    void optionsShowStatusbar();
    void optionsConfigureKeys();
    void optionsConfigureToolbars();
    void optionsPreferences();
    void newToolbarConfig();

private:
    FxpState m_fxpstate;
    QString m_fxpportinfo;
    int m_dcount;
    int m_xferallsize;
    int m_xferresumesize;
    int m_xfered;
    void Xfer();
    void InsertMarkedItems(transferitem::transfertype t, QListViewItem* begin = NULL);
    int initBookmarks();
    void EndXfer();
    void setupStatusBar();
    void setupMenu();
    void setupGUI();
    void setupActions();
    void timerEvent(QTimerEvent* e);

    KasablancaMainWindow *mp_view;
    kbprocess m_proc_a, m_proc_b;
    Bookmarks bookmarks;
    State m_status_a, m_status_b;
    QDir m_currentlocaldir_a, m_currentlocaldir_b;
    QString m_currentremotedir_a, m_currentremotedir_b;
    QPopupMenu m_bookmarksmenu_a, m_bookmarksmenu_b;
    QPopupMenu m_rclickmenu_a, m_rclickmenu_b, m_rclickmenu_t;
    QHeader *mp_header_a, *mp_header_b;
    bool m_sortascending_b, m_sortascending_a;
    siteinfo m_site_a, m_site_b;
    QString m_log_a, m_log_b;
    QTime m_time;
    QueueState m_qstate_a, m_qstate_b;
    QLabel* mp_statusline_a, *mp_statusline_b, *mp_encryptionicon_a, *mp_encryptionicon_b;
    QPixmap m_iconencrypted, m_iconunencrypted;
    KToggleAction *m_toolbarAction;
    KToggleAction *m_statusbarAction;
	 
	 /* new stuff */
	 
	 FtpSession *mp_session_a, *mp_session_b;
	 list<FtpSession*> *mp_sessionlist;
};

#endif
