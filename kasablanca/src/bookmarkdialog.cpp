/***************************************************************************
 *   Copyright (C) 2004 by Magnus Kulke                                    *
 *   mkulke@magnusmachine                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "bookmarkdialog.h"
#include "siteinfo.h"
#include "bookmarks.h"

#include <qdom.h>
#include <qfile.h>
#include <qdir.h>

#include <kcombobox.h>
#include <kcompletion.h>
#include <klocale.h>
#include <klineedit.h>
#include <kmessagebox.h>
#include <kpushbutton.h>

BookmarkDialog::BookmarkDialog(QWidget *parent, const char *name)
: KDialogBase(parent, name, true, i18n( "Bookmarks" ),
              KDialogBase::Ok | KDialogBase::Cancel,
              KDialogBase::Ok, true)
{
    ui_ = new KasablancaBookmarkDialog(this);
    setMainWidget(ui_);
    m_bookmarks = bookmarks.getBookmarks();
    
    for (int i = 0; i < static_cast<int>(m_bookmarks.size()); i++)
    {
        ui_->BookmarkListBox->insertItem(m_bookmarks.at(i).GetName(),i);
    }
    
    if (m_bookmarks.empty())
    {
        EnableInput(false);
    }
    else
    {
        EnableInput(true);
        ui_->BookmarkListBox->setSelected(0, true);
        RefreshEntry(m_bookmarks.at(0));
    }

    ui_->ApplyButton->setEnabled(false);

    // Connect all the slots
    connect(ui_->BookmarkListBox, SIGNAL(highlighted(int)), SLOT(SLOT_EntrySelected(int)));
    connect(ui_->ApplyButton, SIGNAL(clicked()), SLOT(SLOT_ApplyEntry()));
    connect(ui_->NewButton, SIGNAL(clicked()), SLOT(SLOT_NewEntry()));
    connect(ui_->RemoveButton, SIGNAL(clicked()), SLOT(SLOT_RemoveEntry()));
	 
	 connect(ui_->NameEdit, SIGNAL(textChanged(const QString&)), SLOT(SLOT_InputChanged(const QString&)));
	 connect(ui_->InfoEdit, SIGNAL(textChanged(const QString&)), SLOT(SLOT_InputChanged(const QString&)));
	 connect(ui_->UserEdit, SIGNAL(textChanged(const QString&)), SLOT(SLOT_InputChanged(const QString&)));
	 connect(ui_->PassEdit, SIGNAL(textChanged(const QString&)), SLOT(SLOT_InputChanged(const QString&)));
	 connect(ui_->EncryptionComboBox, SIGNAL(textChanged(const QString&)), SLOT(SLOT_InputChanged(const QString&)));
	 connect(ui_->ModeComboBox, SIGNAL(textChanged(const QString&)), SLOT(SLOT_InputChanged(const QString&))); 
}

BookmarkDialog::~BookmarkDialog()
{
}

#include "bookmarkdialog.moc"

void BookmarkDialog::SLOT_EntrySelected(int n)
{
    //ui_->ApplyButton->setEnabled(true);
    RefreshEntry(m_bookmarks.at(n));
}

void BookmarkDialog::slotOk()
{
	SLOT_ApplyEntry();
	bookmarks.setBookmarks(m_bookmarks);
	accept();
}

void BookmarkDialog::SLOT_ApplyEntry()
{
    // should not be available on empty list

    int n = ui_->BookmarkListBox->currentItem();
    
    if(n < 0)
    {
        bookmarks.setBookmarks(m_bookmarks);
        ui_->ApplyButton->setEnabled(false);
        
        return;
    }

    if (!ApplyEntry(&m_bookmarks.at(n))) KMessageBox::error(0,i18n("entry form not complete!"));
    else
    {
        ui_->BookmarkListBox->clear();
        for (int i = 0; i < static_cast<int>(m_bookmarks.size()); i++)
        {
            ui_->BookmarkListBox->insertItem(m_bookmarks.at(i).GetName(),i);
        }
        
        ui_->BookmarkListBox->setSelected(n, true);
    }

	 //bookmarks.WriteBookmarks();
    ui_->ApplyButton->setEnabled(false);
}

void BookmarkDialog::SLOT_NewEntry()
{
    int n = m_bookmarks.size();

    EnableInput(true);
    //ui_->ApplyButton->setEnabled(true);

    siteinfo newsite;
    newsite.SetName("New Site");
    newsite.SetInfo("newftp:21");
    newsite.SetUser("anonymous");
    newsite.SetPass("me@my");
    newsite.SetPasv(1);
    newsite.SetTls(0);
    m_bookmarks.push_back(newsite);

    ui_->BookmarkListBox->insertItem("New Site");
    ui_->BookmarkListBox->setSelected(n, true);
    RefreshEntry(newsite);
}

void BookmarkDialog::SLOT_RemoveEntry()
{
    int n = ui_->BookmarkListBox->currentItem();
    
    vector<siteinfo>::iterator deleteIterator;
    deleteIterator = m_bookmarks.begin();
    for (int i = 0; i < n; i++) deleteIterator++;
    m_bookmarks.erase(deleteIterator);

    ui_->BookmarkListBox->clear();
    
    for (int i = 0; i < static_cast<int>(m_bookmarks.size()); i++)
    {
        ui_->BookmarkListBox->insertItem(m_bookmarks.at(i).GetName(),i);
    }
    
    if (m_bookmarks.empty())
    {
        clearInput();
        EnableInput(false);
    }
    else
    {
        EnableInput(true);
        ui_->BookmarkListBox->setSelected(0, true);
        RefreshEntry(m_bookmarks.at(0));
    }

    ui_->ApplyButton->setEnabled(true);
}

void BookmarkDialog::RefreshEntry(siteinfo site)
{
    ui_->NameEdit->setText(site.GetName());
    ui_->UserEdit->setText(site.GetUser());
    ui_->PassEdit->setText(site.GetPass());
    ui_->InfoEdit->setText(site.GetInfo());
    ui_->EncryptionComboBox->setCurrentItem(site.GetTls());
    ui_->ModeComboBox->setCurrentItem(site.GetPasv());
}

void BookmarkDialog::EnableInput(bool b)
{
    ui_->NameEdit->setEnabled(b);
    ui_->UserEdit->setEnabled(b);
    ui_->PassEdit->setEnabled(b);
    ui_->InfoEdit->setEnabled(b);
    ui_->EncryptionComboBox->setEnabled(b);
    ui_->ModeComboBox->setEnabled(b);
    ui_->RemoveButton->setEnabled(b);
}

void BookmarkDialog::clearInput()
{
    ui_->NameEdit->clear();
    ui_->UserEdit->clear();
    ui_->PassEdit->clear();
    ui_->InfoEdit->clear();
    ui_->EncryptionComboBox->clear();
    ui_->ModeComboBox->clear();
}

void BookmarkDialog::SLOT_InputChanged(const QString&)
{
	ui_->ApplyButton->setEnabled(true);
}

int BookmarkDialog::ApplyEntry(siteinfo * site)
{
    siteinfo entry;
    entry.SetName(ui_->NameEdit->text().latin1());
    entry.SetUser(ui_->UserEdit->text().latin1());
    entry.SetPass(ui_->PassEdit->text().latin1());
    entry.SetInfo(ui_->InfoEdit->text().latin1());
    entry.SetTls(ui_->EncryptionComboBox->currentItem());
    entry.SetPasv(ui_->ModeComboBox->currentItem());

    if (entry.CheckContent())
    {
        site->SetName(ui_->NameEdit->text().latin1());
        site->SetUser(ui_->UserEdit->text().latin1());
        site->SetPass(ui_->PassEdit->text().latin1());
        site->SetInfo(ui_->InfoEdit->text().latin1());
        site->SetTls(ui_->EncryptionComboBox->currentItem());
        site->SetPasv(ui_->ModeComboBox->currentItem());
        
        return 1;
    }
    else return 0;
}
