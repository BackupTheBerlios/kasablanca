/****************************************************************************
** Form interface generated from reading ui file '/home/mkulke/Apps/kasablanca-0.3/src/Q_bookmarkdialog.ui'
**
** Created: Mo Mär 22 15:51:13 2004
**      by: The User Interface Compiler ($Id: Q_bookmarkdialog.h,v 1.1 2004/03/22 18:58:44 mkulke Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef KASABLANCABOOKMARKDIALOG_H
#define KASABLANCABOOKMARKDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class QComboBox;
class QLineEdit;
class QPushButton;
class QListBox;
class QListBoxItem;

class KasablancaBookmarkDialog : public QDialog
{
    Q_OBJECT

public:
    KasablancaBookmarkDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~KasablancaBookmarkDialog();

    QLabel* textLabel1_2_2_2;
    QLabel* textLabel1;
    QLabel* textLabel1_2_2_3;
    QComboBox* EncryptionComboBox;
    QLineEdit* UserEdit;
    QComboBox* ModeComboBox;
    QLabel* textLabel1_2_2;
    QPushButton* DoneButton;
    QLabel* textLabel1_3;
    QLineEdit* PassEdit;
    QLineEdit* InfoEdit;
    QLabel* textLabel1_2;
    QLineEdit* NameEdit;
    QPushButton* ApplyButton;
    QPushButton* RemoveButton;
    QPushButton* NewButton;
    QListBox* BookmarkListBox;

public slots:
    virtual void SLOT_EntrySelected(int i);
    virtual void SLOT_ApplyEntry();
    virtual void SLOT_NewEntry();
    virtual void SLOT_RemoveEntry();
    virtual void SLOT_Done();

protected:

protected slots:
    virtual void languageChange();

};

#endif // KASABLANCABOOKMARKDIALOG_H
