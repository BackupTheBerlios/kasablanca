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

#include "diritem.h"
#include "fileitem.h"
#include <kapp.h>
#include <qwidget.h>
#include <qmenubar.h>
#include <kprocess.h>
#include <vector>
#include "Q_mainwindow.h"
#include "siteinfo.h"
#include "remotefileinfo.h"
#include "kbprocess.h"
#include <kprocio.h>
#include <klistview.h>
#include <qlistview.h>
#include <qdir.h>

/** Kasablanca is the base class of the project */

class Kasablanca : public KasablancaMainWindow
{
	Q_OBJECT
public:
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

	Kasablanca(QWidget* parent=0, const char *name=0);
	~Kasablanca();

	int FormatFilelist(const char *filename, QString current, vector<RemoteFileInfo> *filetable,
		vector<RemoteFileInfo> *dirtable);

	QString m_tempdirname;
	QString m_version;

public slots:
	void SLOT_MkdirA();
	void SLOT_MkdirB();
	void SLOT_ConnectCustom();
	void SLOT_EnterCommandA();
	void SLOT_EnterCommandB();
	void SLOT_EnterCwdA();
	void SLOT_EnterCwdB();
	void SLOT_DisconnectA();
	void SLOT_EditBookmarks();
	void SLOT_ConnectBookmark(int n);
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
	void SkipTasks();
	void SLOT_UpdateA();
	void SLOT_KbftpReadReady();
	void SLOT_About();
	void SLOT_RenameA();
	void SLOT_RenameB();
	void SLOT_Quit();
	void SLOT_DeleteA();
	void SLOT_DeleteB();

private:

	int m_dcount;
	int m_xferallsize;
	int m_xferresumesize;
	void Xfer();
	void InsertMarkedItems(TransferMethod m, QListViewItem* begin = NULL);
	void UpdateLocalDisplay();
	int ParseBookmarks();
	void SetGuiStatus(State);
   void UpdateRemote();
   void UpdateRemoteDisplay(QString dirfile);

	kbprocess m_proc;
	State m_status;
	QDir m_currentlocaldir;
	QString m_currentremotedir_a;
	QPopupMenu m_bookmarksmenu;
	QPopupMenu m_rclickmenu_a;
	QPopupMenu m_rclickmenu_b;
	QPopupMenu m_rclickmenu_t;
	QHeader* mp_header_a;
	QHeader* mp_header_b;
	bool m_sortascending_b;
	bool m_sortascending_a;
	siteinfo m_site_a;
	QString m_log;
	vector<siteinfo> m_bookmarks;
	QTime m_timer;
	QueueState m_qstate;
};

#endif
