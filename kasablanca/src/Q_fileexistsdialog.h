/****************************************************************************
** Form interface generated from reading ui file '/home/mkulke/Apps/kasablanca-0.3/src/Q_fileexistsdialog.ui'
**
** Created: Mo Mär 22 15:51:27 2004
**      by: The User Interface Compiler ($Id: Q_fileexistsdialog.h,v 1.1 2004/03/22 18:58:39 mkulke Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef KASABLANCAFILEEXISTSDIALOG_H
#define KASABLANCAFILEEXISTSDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QPushButton;

class KasablancaFileExistsDialog : public QDialog
{
    Q_OBJECT

public:
    KasablancaFileExistsDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~KasablancaFileExistsDialog();

    QPushButton* SkipButton;
    QPushButton* ResumeButton;
    QPushButton* RenameButton;
    QPushButton* OverwriteButton;

public slots:
    virtual void Skip();
    virtual void Overwrite();
    virtual void Resume();
    virtual void Rename();

protected:
    QGridLayout* KasablancaFileExistsDialogLayout;

protected slots:
    virtual void languageChange();

};

#endif // KASABLANCAFILEEXISTSDIALOG_H
