/***************************************************************************
                          customconnectdialog.cpp  -  description
                             -------------------
    begin                : Mit Aug 20 2003
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

#include "siteinfo.h"
#include <qstring.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qcombobox.h>
 
#include "customconnectdialog.h"

CustomConnectDialog::CustomConnectDialog(QWidget *parent, const char *name ) : KasablancaCustomConnectDialog(parent,name){}
CustomConnectDialog::~CustomConnectDialog(){
}

void CustomConnectDialog::accept()
{
    mp_site->SetName("Custom Site");
    mp_site->SetInfo(InfoLineEdit->text().latin1());
    mp_site->SetUser(UserLineEdit->text().latin1());
    mp_site->SetPass(PassLineEdit->text().latin1());
    mp_site->SetTls(EncryptionComboBox->currentItem());
    mp_site->SetPasv(ModeComboBox->currentItem() ^ 1);
    
    done(QDialog::Accepted);
}
