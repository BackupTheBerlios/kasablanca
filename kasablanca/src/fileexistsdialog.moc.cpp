/****************************************************************************
** FileExistsDialog meta object code from reading C++ file 'fileexistsdialog.h'
**
** Created: Mon Mar 22 15:51:42 2004
**      by: The Qt MOC ($Id: fileexistsdialog.moc.cpp,v 1.1 2004/03/22 18:58:36 mkulke Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "fileexistsdialog.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *FileExistsDialog::className() const
{
    return "FileExistsDialog";
}

QMetaObject *FileExistsDialog::metaObj = 0;
static QMetaObjectCleanUp cleanUp_FileExistsDialog( "FileExistsDialog", &FileExistsDialog::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString FileExistsDialog::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "FileExistsDialog", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString FileExistsDialog::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "FileExistsDialog", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* FileExistsDialog::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = KasablancaFileExistsDialog::staticMetaObject();
    static const QUMethod slot_0 = {"Resume", 0, 0 };
    static const QUMethod slot_1 = {"Overwrite", 0, 0 };
    static const QUMethod slot_2 = {"Skip", 0, 0 };
    static const QUMethod slot_3 = {"Rename", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "Resume()", &slot_0, QMetaData::Public },
	{ "Overwrite()", &slot_1, QMetaData::Public },
	{ "Skip()", &slot_2, QMetaData::Public },
	{ "Rename()", &slot_3, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"FileExistsDialog", parentObject,
	slot_tbl, 4,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_FileExistsDialog.setMetaObject( metaObj );
    return metaObj;
}

void* FileExistsDialog::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "FileExistsDialog" ) )
	return this;
    return KasablancaFileExistsDialog::qt_cast( clname );
}

bool FileExistsDialog::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: Resume(); break;
    case 1: Overwrite(); break;
    case 2: Skip(); break;
    case 3: Rename(); break;
    default:
	return KasablancaFileExistsDialog::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool FileExistsDialog::qt_emit( int _id, QUObject* _o )
{
    return KasablancaFileExistsDialog::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool FileExistsDialog::qt_property( int id, int f, QVariant* v)
{
    return KasablancaFileExistsDialog::qt_property( id, f, v);
}

bool FileExistsDialog::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
