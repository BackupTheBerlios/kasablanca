/****************************************************************************
** CustomConnectDialog meta object code from reading C++ file 'customconnectdialog.h'
**
** Created: Mon Mar 22 15:51:47 2004
**      by: The Qt MOC ($Id: customconnectdialog.moc.cpp,v 1.1 2004/03/22 18:58:45 mkulke Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "customconnectdialog.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *CustomConnectDialog::className() const
{
    return "CustomConnectDialog";
}

QMetaObject *CustomConnectDialog::metaObj = 0;
static QMetaObjectCleanUp cleanUp_CustomConnectDialog( "CustomConnectDialog", &CustomConnectDialog::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString CustomConnectDialog::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "CustomConnectDialog", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString CustomConnectDialog::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "CustomConnectDialog", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* CustomConnectDialog::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = KasablancaCustomConnectDialog::staticMetaObject();
    static const QUMethod slot_0 = {"accept", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "accept()", &slot_0, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"CustomConnectDialog", parentObject,
	slot_tbl, 1,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_CustomConnectDialog.setMetaObject( metaObj );
    return metaObj;
}

void* CustomConnectDialog::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "CustomConnectDialog" ) )
	return this;
    return KasablancaCustomConnectDialog::qt_cast( clname );
}

bool CustomConnectDialog::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: accept(); break;
    default:
	return KasablancaCustomConnectDialog::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool CustomConnectDialog::qt_emit( int _id, QUObject* _o )
{
    return KasablancaCustomConnectDialog::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool CustomConnectDialog::qt_property( int id, int f, QVariant* v)
{
    return KasablancaCustomConnectDialog::qt_property( id, f, v);
}

bool CustomConnectDialog::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
