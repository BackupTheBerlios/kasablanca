/****************************************************************************
** Form interface generated from reading ui file '/home/mkulke/Apps/kasablanca-0.3/src/Q_mainwindow.ui'
**
** Created: Mo Mär 22 17:35:50 2004
**      by: The User Interface Compiler ($Id: Q_mainwindow.h,v 1.1 2004/03/22 18:58:41 mkulke Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef KASABLANCAMAINWINDOW_H
#define KASABLANCAMAINWINDOW_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qmainwindow.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QAction;
class QActionGroup;
class QToolBar;
class QPopupMenu;
class QLineEdit;
class QListView;
class QListViewItem;
class QToolButton;
class QTextEdit;

class KasablancaMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    KasablancaMainWindow( QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel );
    ~KasablancaMainWindow();

    QLineEdit* CommandLineB;
    QLineEdit* CommandLineA;
    QListView* BrowserA;
    QListView* BrowserB;
    QToolButton* ConnectButtonA;
    QToolButton* RefreshButtonA;
    QLineEdit* CwdLineA;
    QToolButton* TransferButtonA;
    QToolButton* ConnectButtonB;
    QToolButton* RefreshButtonB;
    QToolButton* TransferButtonB;
    QLineEdit* CwdLineB;
    QListView* TaskView;
    QTextEdit* LogWindow;
    QMenuBar *menubar;
    QPopupMenu *PopupMenu;
    QPopupMenu *EditMenu;
    QPopupMenu *HelpMenu;
    QToolBar *Toolbar;
    QAction* ActionConnectCustom;
    QAction* ActionDisconnectA;
    QAction* ActionUpdateA;
    QAction* ActionTransferB;
    QAction* ActionTransferA;
    QAction* ActionEditBookmarks;
    QAction* ActionQuit;
    QAction* ActionAbout;

public slots:
    virtual void SLOT_DisconnectA();
    virtual void SLOT_EnterCommandA();
    virtual void SLOT_ConnectCustom();
    virtual void SLOT_ConnectBookmark( int n );
    virtual void SLOT_EnterCwdA();
    virtual void SLOT_EnterCommandB();
    virtual void SLOT_UpdateA();
    virtual void SLOT_EnterCwdB();
    virtual void SLOT_SelectionChanged();
    virtual void SLOT_EditBookmarks();
    virtual void SLOT_Quit();
    virtual void SLOT_About();
    virtual void SLOT_TransferA();
    virtual void SLOT_TransferB();

protected:
    QGridLayout* KasablancaMainWindowLayout;

protected slots:
    virtual void languageChange();

private:
    QPixmap image0;
    QPixmap image1;
    QPixmap image2;
    QPixmap image3;

};

#endif // KASABLANCAMAINWINDOW_H
