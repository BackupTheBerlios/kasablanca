#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file '/home/mkulke/Apps/kasablanca-0.3/src/Q_bookmarkdialog.ui'
**
** Created: Mo Mär 22 15:51:13 2004
**      by: The User Interface Compiler ($Id: Q_bookmarkdialog.cpp,v 1.1 2004/03/22 18:58:46 mkulke Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "Q_bookmarkdialog.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a KasablancaBookmarkDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
KasablancaBookmarkDialog::KasablancaBookmarkDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "KasablancaBookmarkDialog" );
    setSizeGripEnabled( FALSE );
    setModal( TRUE );

    textLabel1_2_2_2 = new QLabel( this, "textLabel1_2_2_2" );
    textLabel1_2_2_2->setGeometry( QRect( 260, 170, 71, 27 ) );

    textLabel1 = new QLabel( this, "textLabel1" );
    textLabel1->setGeometry( QRect( 263, 10, 71, 27 ) );

    textLabel1_2_2_3 = new QLabel( this, "textLabel1_2_2_3" );
    textLabel1_2_2_3->setGeometry( QRect( 260, 210, 71, 27 ) );

    EncryptionComboBox = new QComboBox( FALSE, this, "EncryptionComboBox" );
    EncryptionComboBox->setGeometry( QRect( 340, 170, 150, 27 ) );

    UserEdit = new QLineEdit( this, "UserEdit" );
    UserEdit->setGeometry( QRect( 340, 90, 150, 27 ) );

    ModeComboBox = new QComboBox( FALSE, this, "ModeComboBox" );
    ModeComboBox->setGeometry( QRect( 340, 210, 150, 27 ) );

    textLabel1_2_2 = new QLabel( this, "textLabel1_2_2" );
    textLabel1_2_2->setGeometry( QRect( 260, 130, 71, 27 ) );

    DoneButton = new QPushButton( this, "DoneButton" );
    DoneButton->setGeometry( QRect( 420, 250, 72, 29 ) );

    textLabel1_3 = new QLabel( this, "textLabel1_3" );
    textLabel1_3->setGeometry( QRect( 260, 90, 71, 27 ) );

    PassEdit = new QLineEdit( this, "PassEdit" );
    PassEdit->setGeometry( QRect( 340, 130, 150, 27 ) );

    InfoEdit = new QLineEdit( this, "InfoEdit" );
    InfoEdit->setGeometry( QRect( 340, 50, 150, 27 ) );

    textLabel1_2 = new QLabel( this, "textLabel1_2" );
    textLabel1_2->setGeometry( QRect( 260, 50, 71, 27 ) );

    NameEdit = new QLineEdit( this, "NameEdit" );
    NameEdit->setGeometry( QRect( 340, 10, 150, 27 ) );
    NameEdit->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, NameEdit->sizePolicy().hasHeightForWidth() ) );

    ApplyButton = new QPushButton( this, "ApplyButton" );
    ApplyButton->setGeometry( QRect( 172, 250, 70, 29 ) );

    RemoveButton = new QPushButton( this, "RemoveButton" );
    RemoveButton->setGeometry( QRect( 90, 250, 70, 29 ) );

    NewButton = new QPushButton( this, "NewButton" );
    NewButton->setGeometry( QRect( 10, 250, 70, 29 ) );

    BookmarkListBox = new QListBox( this, "BookmarkListBox" );
    BookmarkListBox->setGeometry( QRect( 10, 10, 230, 230 ) );
    BookmarkListBox->setResizePolicy( QListBox::Manual );
    BookmarkListBox->setVScrollBarMode( QListBox::Auto );
    languageChange();
    resize( QSize(502, 290).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( BookmarkListBox, SIGNAL( highlighted(int) ), this, SLOT( SLOT_EntrySelected(int) ) );
    connect( ApplyButton, SIGNAL( clicked() ), this, SLOT( SLOT_ApplyEntry() ) );
    connect( NewButton, SIGNAL( clicked() ), this, SLOT( SLOT_NewEntry() ) );
    connect( RemoveButton, SIGNAL( clicked() ), this, SLOT( SLOT_RemoveEntry() ) );
    connect( DoneButton, SIGNAL( clicked() ), this, SLOT( SLOT_Done() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
KasablancaBookmarkDialog::~KasablancaBookmarkDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void KasablancaBookmarkDialog::languageChange()
{
    setCaption( tr2i18n( "Bookmarks" ) );
    textLabel1_2_2_2->setText( tr2i18n( "<p align=\"right\">Encryption</p>" ) );
    textLabel1->setText( tr2i18n( "<p align=\"right\">Name</p>" ) );
    textLabel1_2_2_3->setText( tr2i18n( "<p align=\"right\">Mode</p>" ) );
    EncryptionComboBox->clear();
    EncryptionComboBox->insertItem( tr2i18n( "Unencrypted" ) );
    EncryptionComboBox->insertItem( tr2i18n( "Level 1" ) );
    EncryptionComboBox->insertItem( tr2i18n( "Level 2" ) );
    EncryptionComboBox->insertItem( tr2i18n( "Level 3" ) );
    ModeComboBox->clear();
    ModeComboBox->insertItem( tr2i18n( "Active Mode" ) );
    ModeComboBox->insertItem( tr2i18n( "Passive Mode" ) );
    textLabel1_2_2->setText( tr2i18n( "<p align=\"right\">Password</p>" ) );
    DoneButton->setText( tr2i18n( "Done" ) );
    textLabel1_3->setText( tr2i18n( "<p align=\"right\">Username</p>" ) );
    textLabel1_2->setText( tr2i18n( "<p align=\"right\">IP:Port</p>" ) );
    ApplyButton->setText( tr2i18n( "Apply" ) );
    RemoveButton->setText( tr2i18n( "Remove" ) );
    NewButton->setText( tr2i18n( "New" ) );
}

void KasablancaBookmarkDialog::SLOT_EntrySelected(int)
{
    qWarning( "KasablancaBookmarkDialog::SLOT_EntrySelected(int): Not implemented yet" );
}

void KasablancaBookmarkDialog::SLOT_ApplyEntry()
{
    qWarning( "KasablancaBookmarkDialog::SLOT_ApplyEntry(): Not implemented yet" );
}

void KasablancaBookmarkDialog::SLOT_NewEntry()
{
    qWarning( "KasablancaBookmarkDialog::SLOT_NewEntry(): Not implemented yet" );
}

void KasablancaBookmarkDialog::SLOT_RemoveEntry()
{
    qWarning( "KasablancaBookmarkDialog::SLOT_RemoveEntry(): Not implemented yet" );
}

void KasablancaBookmarkDialog::SLOT_Done()
{
    qWarning( "KasablancaBookmarkDialog::SLOT_Done(): Not implemented yet" );
}

#include "Q_bookmarkdialog.moc"
