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
class QWidget;
class QCloseEvent;

#include <kprocess.h>
#include <klistview.h>
#include <qlistview.h>
#include <qdir.h>
#include <vector>
#include <qmenubar.h>
#include "transferitem.h"
#include "siteinfo.h"
#include "kbprocess.h"
#include "remotefileinfo.h"
#include "Q_mainwindow.h"

/** Kasablanca is the base class of the project */

class Kasablanca : public KasablancaMainWindow
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
	
	enum QueueState
	{
		done = 0,
		changedir,
		proceed,
		scanremote,
		scanlocal
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

	Kasablanca(QWidget* parent=0, const char *name=0);
	~Kasablanca();

	int FormatFilelist(const char *filename, QString current, vector<RemoteFileInfo> *filetable,
		vector<RemoteFileInfo> *dirtable);

	QString m_tempdirname;
	QString m_version;

public slots:
	void SLOT_MkdirA();
	void SLOT_MkdirB();
	void SLOT_EnterCommandA();
	void SLOT_EnterCommandB();
	void SLOT_EnterCwdA();
	void SLOT_EnterCwdB();
	void SLOT_ConnectA(int i);
	void SLOT_ConnectB(int i);
	void SLOT_ConnectButtonA();
	void SLOT_ConnectButtonB();
	void SLOT_EditBookmarks();
	void SLOT_ItemClickedA(QListViewItem * item);
	void SLOT_ItemClickedB(QListViewItem * item);
	void SLOT_ItemRightClickedA(QListViewItem * item, const QPoint & point, int col );
	void SLOT_ItemRightClickedB(QListViewItem * item, const QPoint & point, int col );
	void SLOT_ItemRightClickedT(QListViewItem * item, const QPoint & point, int col );
	void SLOT_TransferA();
	void SLOT_HeaderBClicked(int section);
	void SLOT_HeaderAClicked(int section);
	void SLOT_TransferB();
	void SLOT_QueueA();
	void SLOT_QueueB();
	void SLOT_SelectionChanged();
	void SLOT_StartQueue();
	void SLOT_SkipTasks();
	void SLOT_KbftpReadReady(kbprocess* p);
	void SLOT_About();
	void SLOT_RenameA();
	void SLOT_RenameB();
	void SLOT_DeleteA();
	void SLOT_DeleteB();
	void SLOT_RefreshBrowserA();
	void SLOT_RefreshBrowserB();
	void SLOT_ProcessExited(KProcess *proc);
	void SLOT_Close();

private:

	int m_dcount;
	int m_xferallsize;
	int m_xferresumesize;
	void Xfer();
	void InsertMarkedItems(transferitem::transfertype t, QListViewItem* begin = NULL);
	void UpdateLocalDisplay(Browser x);
	int ParseBookmarks();
	void SetGuiStatus(State s, Browser b);
	void UpdateRemote(Browser b);
	void UpdateRemoteDisplay(QString dirfile, Browser b);
	void ConnectCustom(Browser b);
	void ConnectBookmark(int n, Browser b);
	
	void closeEvent(QCloseEvent * e);
	
	kbprocess m_proc_a, m_proc_b;
	State m_status_a, m_status_b;
	QDir m_currentlocaldir_a, m_currentlocaldir_b;
	QString m_currentremotedir_a, m_currentremotedir_b;
	QPopupMenu m_bookmarksmenu_a, m_bookmarksmenu_b;
	QPopupMenu m_rclickmenu_a, m_rclickmenu_b, m_rclickmenu_t;
	QHeader *mp_header_a, *mp_header_b;
	bool m_sortascending_b, m_sortascending_a;
	siteinfo m_site_a, m_site_b;
	QString m_log_a, m_log_b;
	vector<siteinfo> m_bookmarks;
	QTime m_timer;
	QueueState m_qstate;
};

#endif
