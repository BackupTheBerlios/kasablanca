#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file '/home/mkulke/Apps/kasablanca-0.3/src/Q_customconnectdialog.ui'
**
** Created: Mo Mär 22 15:51:21 2004
**      by: The User Interface Compiler ($Id: Q_customconnectdialog.cpp,v 1.1 2004/03/22 18:58:44 mkulke Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "Q_customconnectdialog.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a KasablancaCustomConnectDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
KasablancaCustomConnectDialog::KasablancaCustomConnectDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "KasablancaCustomConnectDialog" );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setMaximumSize( QSize( 32767, 32767 ) );
    setSizeGripEnabled( FALSE );

    textLabel3 = new QLabel( this, "textLabel3" );
    textLabel3->setGeometry( QRect( 7, 50, 70, 27 ) );

    textLabel2 = new QLabel( this, "textLabel2" );
    textLabel2->setGeometry( QRect( 10, 10, 70, 27 ) );

    textLabel4 = new QLabel( this, "textLabel4" );
    textLabel4->setGeometry( QRect( 7, 90, 70, 27 ) );

    textLabel1 = new QLabel( this, "textLabel1" );
    textLabel1->setGeometry( QRect( 10, 130, 70, 27 ) );

    textLabel1_2 = new QLabel( this, "textLabel1_2" );
    textLabel1_2->setGeometry( QRect( 10, 170, 70, 27 ) );

    PassLineEdit = new QLineEdit( this, "PassLineEdit" );
    PassLineEdit->setGeometry( QRect( 90, 90, 289, 27 ) );

    UserLineEdit = new QLineEdit( this, "UserLineEdit" );
    UserLineEdit->setGeometry( QRect( 90, 50, 289, 27 ) );

    InfoLineEdit = new QLineEdit( this, "InfoLineEdit" );
    InfoLineEdit->setGeometry( QRect( 90, 10, 289, 27 ) );

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setGeometry( QRect( 300, 210, 80, 30 ) );
    buttonOk->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)5, 0, 0, buttonOk->sizePolicy().hasHeightForWidth() ) );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setGeometry( QRect( 210, 210, 80, 30 ) );
    buttonCancel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)5, 0, 0, buttonCancel->sizePolicy().hasHeightForWidth() ) );
    buttonCancel->setAutoDefault( TRUE );

    ModeComboBox = new QComboBox( FALSE, this, "ModeComboBox" );
    ModeComboBox->setGeometry( QRect( 90, 170, 289, 27 ) );

    EncryptionComboBox = new QComboBox( FALSE, this, "EncryptionComboBox" );
    EncryptionComboBox->setGeometry( QRect( 90, 130, 289, 27 ) );
    languageChange();
    resize( QSize(392, 253).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );

    // tab order
    setTabOrder( InfoLineEdit, UserLineEdit );
    setTabOrder( UserLineEdit, PassLineEdit );
    setTabOrder( PassLineEdit, ModeComboBox );
    setTabOrder( ModeComboBox, EncryptionComboBox );
    setTabOrder( EncryptionComboBox, buttonOk );
    setTabOrder( buttonOk, buttonCancel );
}

/*
 *  Destroys the object and frees any allocated resources
 */
KasablancaCustomConnectDialog::~KasablancaCustomConnectDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void KasablancaCustomConnectDialog::languageChange()
{
    setCaption( tr2i18n( "Connect to Custom Site" ) );
    textLabel3->setText( tr2i18n( "Username" ) );
    textLabel2->setText( tr2i18n( "IP:Port" ) );
    textLabel4->setText( tr2i18n( "Password" ) );
    textLabel1->setText( tr2i18n( "Encryption" ) );
    textLabel1_2->setText( tr2i18n( "Mode" ) );
    buttonOk->setText( tr2i18n( "&OK" ) );
    buttonOk->setAccel( QKeySequence( QString::null ) );
    buttonCancel->setText( tr2i18n( "&Cancel" ) );
    buttonCancel->setAccel( QKeySequence( QString::null ) );
    ModeComboBox->clear();
    ModeComboBox->insertItem( tr2i18n( "Passive Mode" ) );
    ModeComboBox->insertItem( tr2i18n( "Active Mode" ) );
    EncryptionComboBox->clear();
    EncryptionComboBox->insertItem( tr2i18n( "Unencrypted" ) );
    EncryptionComboBox->insertItem( tr2i18n( "Level 1" ) );
    EncryptionComboBox->insertItem( tr2i18n( "Level 2" ) );
    EncryptionComboBox->insertItem( tr2i18n( "Level 3" ) );
}

#include "Q_customconnectdialog.moc"
