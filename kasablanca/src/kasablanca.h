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

class KbDir;
class KbFile;
class KToggleAction;
class QWidget;
class QCloseEvent;
class QLabel;
class QPixmap;
class FtpSession;

#include <kapplication.h>
#include <klistview.h>
#include <kmainwindow.h>
#include <kprocess.h>

#include <qdir.h>
#include <qlistview.h>
#include <qmenubar.h>

#include <vector>

#include "bookmarks.h"
#include "kbtransferitem.h"
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
    
    Kasablanca();
    ~Kasablanca();

    QString m_tempdirname;
    QString m_version;
	 vector<siteinfo> m_bookmarks;

public slots:
    void SLOT_EditBookmarks();
    void SLOT_ItemRightClickedT(QListViewItem * item, const QPoint & point, int col );
    void SLOT_SelectionChanged();
    void SLOT_SkipTasks();
	 void SLOT_ProcessQueue();
	 void SLOT_QueueItems(KbDirInfo *dir, FtpSession* src, FtpSession* dst);
	 void SLOT_NextTransfer(QListViewItem* item);
	 
private slots:
    void optionsShowToolbar();
    void optionsShowStatusbar();
    void optionsConfigureKeys();
    void optionsConfigureToolbars();
    void optionsPreferences();
    void newToolbarConfig();

private:
	 void QueueItemsRecurse(KbDirInfo *dir, FtpSession* src, FtpSession* dst, QListViewItem* parent = NULL);
    int initBookmarks();
    void setupStatusBar();
    void setupMenu();
    void setupGUI();
    void setupActions();
    void timerEvent(QTimerEvent* e);
	 void ProcessQueue(KbTransferItem* item);
	 
    KasablancaMainWindow *mp_view;
    Bookmarks bookmarks;
    QPopupMenu m_bookmarksmenu_a, m_bookmarksmenu_b;
    QPopupMenu m_rclickmenu_a, m_rclickmenu_b, m_rclickmenu_t;
    QTime m_time;
    QLabel *mp_statusline_a, *mp_statusline_b, *mp_encryptionicon_a, *mp_encryptionicon_b;
    QPixmap m_iconencrypted, m_iconunencrypted;
    KToggleAction *m_toolbarAction;
    KToggleAction *m_statusbarAction;	 
	 FtpSession *mp_session_a, *mp_session_b;
	 list<FtpSession*> *mp_sessionlist;
};

#endif
