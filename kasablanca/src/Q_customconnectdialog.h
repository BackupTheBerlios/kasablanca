/****************************************************************************
** Form interface generated from reading ui file '/home/mkulke/Apps/kasablanca-0.3/src/Q_customconnectdialog.ui'
**
** Created: Mo Mär 22 15:51:20 2004
**      by: The User Interface Compiler ($Id: Q_customconnectdialog.h,v 1.1 2004/03/22 18:58:44 mkulke Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef KASABLANCACUSTOMCONNECTDIALOG_H
#define KASABLANCACUSTOMCONNECTDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;

class KasablancaCustomConnectDialog : public QDialog
{
    Q_OBJECT

public:
    KasablancaCustomConnectDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~KasablancaCustomConnectDialog();

    QLabel* textLabel3;
    QLabel* textLabel2;
    QLabel* textLabel4;
    QLabel* textLabel1;
    QLabel* textLabel1_2;
    QLineEdit* PassLineEdit;
    QLineEdit* UserLineEdit;
    QLineEdit* InfoLineEdit;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QComboBox* ModeComboBox;
    QComboBox* EncryptionComboBox;

protected:

protected slots:
    virtual void languageChange();

};

#endif // KASABLANCACUSTOMCONNECTDIALOG_H
