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
#include <kmessagebox.h>
#include <qlistbox.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qpushbutton.h>

BookmarkDialog::BookmarkDialog(QWidget *parent, const char *name)
 : KasablancaBookmarkDialog(parent, name)
{
    m_bookmarks = bookmarks.getBookmarks();
    
    for (int i = 0; i < static_cast<int>(m_bookmarks.size()); i++)
    {
        BookmarkListBox->insertItem(m_bookmarks.at(i).GetName(),i);
    }
    
    if (m_bookmarks.empty())
    {
        EnableInput(false);
    }
    else
    {
        EnableInput(true);
        BookmarkListBox->setSelected(0, true);
        RefreshEntry(m_bookmarks.at(0));
    }
}


BookmarkDialog::~BookmarkDialog()
{
}


#include "bookmarkdialog.moc"


void BookmarkDialog::SLOT_Done()
{
	int n = BookmarkListBox->currentItem();

	if (m_bookmarks.empty())
	{
		if (bookmarks.setBookmarks(m_bookmarks) != 1)
		{
			KMessageBox::error(0,"Could not write Kasablanca bookmark xml file.");
			done(QDialog::Rejected);
		}
		else done(QDialog::Accepted);
		return;
	}

	if (ApplyEntry(&m_bookmarks.at(n)))
	{
		if (bookmarks.setBookmarks(m_bookmarks) != 1)
		{
			KMessageBox::error(0,"Could not write Kasablanca bookmark xml file.");
			done(QDialog::Rejected);
		}
		else done(QDialog::Accepted);
	}
	else KMessageBox::error(0,"entry form not complete!");
}

void BookmarkDialog::SLOT_EntrySelected(int n)
{
	RefreshEntry(m_bookmarks.at(n));
}

void BookmarkDialog::SLOT_ApplyEntry()
{
	// should not be available on empty list

	int n = BookmarkListBox->currentItem();

	if (!ApplyEntry(&m_bookmarks.at(n))) KMessageBox::error(0,"entry form not complete!");
	else
	{
		BookmarkListBox->clear();
		for (int i = 0; i < static_cast<int>(m_bookmarks.size()); i++)
   	{
			BookmarkListBox->insertItem(m_bookmarks.at(i).GetName(),i);
   	}
		BookmarkListBox->setSelected(n, true);
	}
}

void BookmarkDialog::SLOT_NewEntry()
{
	int n = m_bookmarks.size();

	EnableInput(true);

	siteinfo newsite;
	newsite.SetName("New Site");
	newsite.SetInfo("newftp:21");
	newsite.SetUser("anonymous");
	newsite.SetPass("me@my");
	newsite.SetPasv(1);
	newsite.SetTls(0);
	m_bookmarks.push_back(newsite);

	BookmarkListBox->insertItem("New Site");
	BookmarkListBox->setSelected(n, true);
	RefreshEntry(newsite);
}

void BookmarkDialog::SLOT_RemoveEntry()
{
	int n = BookmarkListBox->currentItem();

	vector<siteinfo>::iterator deleteIterator;
	deleteIterator = m_bookmarks.begin();
	for (int i = 0; i < n; i++) deleteIterator++;
	m_bookmarks.erase(deleteIterator);

	BookmarkListBox->clear();
	for (int i = 0; i < static_cast<int>(m_bookmarks.size()); i++)
   {
		BookmarkListBox->insertItem(m_bookmarks.at(i).GetName(),i);
   }

	if (m_bookmarks.empty()) EnableInput(false);
	else
	{
		EnableInput(true);
		BookmarkListBox->setSelected(0, true);
		RefreshEntry(m_bookmarks.at(0));
	}
}


/*!
    \fn BookmarkDialog::RefreshEntry()
 */
void BookmarkDialog::RefreshEntry(siteinfo site)
{
	NameEdit->setText(site.GetName());
	UserEdit->setText(site.GetUser());
	PassEdit->setText(site.GetPass());
	InfoEdit->setText(site.GetInfo());
	EncryptionComboBox->setCurrentItem(site.GetTls());
	ModeComboBox->setCurrentItem(site.GetPasv());
}

void BookmarkDialog::EnableInput(bool b)
{
	NameEdit->setEnabled(b);
	UserEdit->setEnabled(b);
	PassEdit->setEnabled(b);
	InfoEdit->setEnabled(b);
	EncryptionComboBox->setEnabled(b);
	ModeComboBox->setEnabled(b);
	RemoveButton->setEnabled(b);
	ApplyButton->setEnabled(b);
}

int BookmarkDialog::ApplyEntry(siteinfo * site)
{
	siteinfo entry;
	entry.SetName(NameEdit->text().latin1());
	entry.SetUser(UserEdit->text().latin1());
	entry.SetPass(PassEdit->text().latin1());
	entry.SetInfo(InfoEdit->text().latin1());
	entry.SetTls(EncryptionComboBox->currentItem());
	entry.SetPasv(ModeComboBox->currentItem());

	if (entry.CheckContent())
	{
		site->SetName(NameEdit->text().latin1());
		site->SetUser(UserEdit->text().latin1());
		site->SetPass(PassEdit->text().latin1());
		site->SetInfo(InfoEdit->text().latin1());
		site->SetTls(EncryptionComboBox->currentItem());
		site->SetPasv(ModeComboBox->currentItem());
		return 1;
	}
	else return 0;
}
