/****************************************************************************
** Kasablanca meta object code from reading C++ file 'kasablanca.h'
**
** Created: Mon Mar 22 15:51:51 2004
**      by: The Qt MOC ($Id: kasablanca.moc.cpp,v 1.1 2004/03/22 18:58:45 mkulke Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "kasablanca.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *Kasablanca::className() const
{
    return "Kasablanca";
}

QMetaObject *Kasablanca::metaObj = 0;
static QMetaObjectCleanUp cleanUp_Kasablanca( "Kasablanca", &Kasablanca::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString Kasablanca::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "Kasablanca", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString Kasablanca::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "Kasablanca", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* Kasablanca::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = KasablancaMainWindow::staticMetaObject();
    static const QUMethod slot_0 = {"SLOT_MkdirA", 0, 0 };
    static const QUMethod slot_1 = {"SLOT_MkdirB", 0, 0 };
    static const QUMethod slot_2 = {"SLOT_ConnectCustom", 0, 0 };
    static const QUMethod slot_3 = {"SLOT_EnterCommandA", 0, 0 };
    static const QUMethod slot_4 = {"SLOT_EnterCommandB", 0, 0 };
    static const QUMethod slot_5 = {"SLOT_EnterCwdA", 0, 0 };
    static const QUMethod slot_6 = {"SLOT_EnterCwdB", 0, 0 };
    static const QUMethod slot_7 = {"SLOT_DisconnectA", 0, 0 };
    static const QUMethod slot_8 = {"SLOT_EditBookmarks", 0, 0 };
    static const QUParameter param_slot_9[] = {
	{ "n", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_9 = {"SLOT_ConnectBookmark", 1, param_slot_9 };
    static const QUParameter param_slot_10[] = {
	{ "item", &static_QUType_ptr, "QListViewItem", QUParameter::In }
    };
    static const QUMethod slot_10 = {"SLOT_ItemClickedA", 1, param_slot_10 };
    static const QUParameter param_slot_11[] = {
	{ "item", &static_QUType_ptr, "QListViewItem", QUParameter::In }
    };
    static const QUMethod slot_11 = {"SLOT_ItemClickedB", 1, param_slot_11 };
    static const QUParameter param_slot_12[] = {
	{ "item", &static_QUType_ptr, "QListViewItem", QUParameter::In },
	{ "point", &static_QUType_varptr, "\x0e", QUParameter::In },
	{ "col", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_12 = {"SLOT_ItemRightClickedA", 3, param_slot_12 };
    static const QUParameter param_slot_13[] = {
	{ "item", &static_QUType_ptr, "QListViewItem", QUParameter::In },
	{ "point", &static_QUType_varptr, "\x0e", QUParameter::In },
	{ "col", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_13 = {"SLOT_ItemRightClickedB", 3, param_slot_13 };
    static const QUParameter param_slot_14[] = {
	{ "item", &static_QUType_ptr, "QListViewItem", QUParameter::In },
	{ "point", &static_QUType_varptr, "\x0e", QUParameter::In },
	{ "col", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_14 = {"SLOT_ItemRightClickedT", 3, param_slot_14 };
    static const QUMethod slot_15 = {"SLOT_TransferA", 0, 0 };
    static const QUParameter param_slot_16[] = {
	{ "section", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_16 = {"SLOT_HeaderBClicked", 1, param_slot_16 };
    static const QUParameter param_slot_17[] = {
	{ "section", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_17 = {"SLOT_HeaderAClicked", 1, param_slot_17 };
    static const QUMethod slot_18 = {"SLOT_TransferB", 0, 0 };
    static const QUMethod slot_19 = {"SLOT_QueueA", 0, 0 };
    static const QUMethod slot_20 = {"SLOT_QueueB", 0, 0 };
    static const QUMethod slot_21 = {"SLOT_SelectionChanged", 0, 0 };
    static const QUMethod slot_22 = {"SLOT_StartQueue", 0, 0 };
    static const QUMethod slot_23 = {"SkipTasks", 0, 0 };
    static const QUMethod slot_24 = {"SLOT_UpdateA", 0, 0 };
    static const QUMethod slot_25 = {"SLOT_KbftpReadReady", 0, 0 };
    static const QUMethod slot_26 = {"SLOT_About", 0, 0 };
    static const QUMethod slot_27 = {"SLOT_RenameA", 0, 0 };
    static const QUMethod slot_28 = {"SLOT_RenameB", 0, 0 };
    static const QUMethod slot_29 = {"SLOT_Quit", 0, 0 };
    static const QUMethod slot_30 = {"SLOT_DeleteA", 0, 0 };
    static const QUMethod slot_31 = {"SLOT_DeleteB", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "SLOT_MkdirA()", &slot_0, QMetaData::Public },
	{ "SLOT_MkdirB()", &slot_1, QMetaData::Public },
	{ "SLOT_ConnectCustom()", &slot_2, QMetaData::Public },
	{ "SLOT_EnterCommandA()", &slot_3, QMetaData::Public },
	{ "SLOT_EnterCommandB()", &slot_4, QMetaData::Public },
	{ "SLOT_EnterCwdA()", &slot_5, QMetaData::Public },
	{ "SLOT_EnterCwdB()", &slot_6, QMetaData::Public },
	{ "SLOT_DisconnectA()", &slot_7, QMetaData::Public },
	{ "SLOT_EditBookmarks()", &slot_8, QMetaData::Public },
	{ "SLOT_ConnectBookmark(int)", &slot_9, QMetaData::Public },
	{ "SLOT_ItemClickedA(QListViewItem*)", &slot_10, QMetaData::Public },
	{ "SLOT_ItemClickedB(QListViewItem*)", &slot_11, QMetaData::Public },
	{ "SLOT_ItemRightClickedA(QListViewItem*,const QPoint&,int)", &slot_12, QMetaData::Public },
	{ "SLOT_ItemRightClickedB(QListViewItem*,const QPoint&,int)", &slot_13, QMetaData::Public },
	{ "SLOT_ItemRightClickedT(QListViewItem*,const QPoint&,int)", &slot_14, QMetaData::Public },
	{ "SLOT_TransferA()", &slot_15, QMetaData::Public },
	{ "SLOT_HeaderBClicked(int)", &slot_16, QMetaData::Public },
	{ "SLOT_HeaderAClicked(int)", &slot_17, QMetaData::Public },
	{ "SLOT_TransferB()", &slot_18, QMetaData::Public },
	{ "SLOT_QueueA()", &slot_19, QMetaData::Public },
	{ "SLOT_QueueB()", &slot_20, QMetaData::Public },
	{ "SLOT_SelectionChanged()", &slot_21, QMetaData::Public },
	{ "SLOT_StartQueue()", &slot_22, QMetaData::Public },
	{ "SkipTasks()", &slot_23, QMetaData::Public },
	{ "SLOT_UpdateA()", &slot_24, QMetaData::Public },
	{ "SLOT_KbftpReadReady()", &slot_25, QMetaData::Public },
	{ "SLOT_About()", &slot_26, QMetaData::Public },
	{ "SLOT_RenameA()", &slot_27, QMetaData::Public },
	{ "SLOT_RenameB()", &slot_28, QMetaData::Public },
	{ "SLOT_Quit()", &slot_29, QMetaData::Public },
	{ "SLOT_DeleteA()", &slot_30, QMetaData::Public },
	{ "SLOT_DeleteB()", &slot_31, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"Kasablanca", parentObject,
	slot_tbl, 32,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_Kasablanca.setMetaObject( metaObj );
    return metaObj;
}

void* Kasablanca::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "Kasablanca" ) )
	return this;
    return KasablancaMainWindow::qt_cast( clname );
}

bool Kasablanca::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: SLOT_MkdirA(); break;
    case 1: SLOT_MkdirB(); break;
    case 2: SLOT_ConnectCustom(); break;
    case 3: SLOT_EnterCommandA(); break;
    case 4: SLOT_EnterCommandB(); break;
    case 5: SLOT_EnterCwdA(); break;
    case 6: SLOT_EnterCwdB(); break;
    case 7: SLOT_DisconnectA(); break;
    case 8: SLOT_EditBookmarks(); break;
    case 9: SLOT_ConnectBookmark((int)static_QUType_int.get(_o+1)); break;
    case 10: SLOT_ItemClickedA((QListViewItem*)static_QUType_ptr.get(_o+1)); break;
    case 11: SLOT_ItemClickedB((QListViewItem*)static_QUType_ptr.get(_o+1)); break;
    case 12: SLOT_ItemRightClickedA((QListViewItem*)static_QUType_ptr.get(_o+1),(const QPoint&)*((const QPoint*)static_QUType_ptr.get(_o+2)),(int)static_QUType_int.get(_o+3)); break;
    case 13: SLOT_ItemRightClickedB((QListViewItem*)static_QUType_ptr.get(_o+1),(const QPoint&)*((const QPoint*)static_QUType_ptr.get(_o+2)),(int)static_QUType_int.get(_o+3)); break;
    case 14: SLOT_ItemRightClickedT((QListViewItem*)static_QUType_ptr.get(_o+1),(const QPoint&)*((const QPoint*)static_QUType_ptr.get(_o+2)),(int)static_QUType_int.get(_o+3)); break;
    case 15: SLOT_TransferA(); break;
    case 16: SLOT_HeaderBClicked((int)static_QUType_int.get(_o+1)); break;
    case 17: SLOT_HeaderAClicked((int)static_QUType_int.get(_o+1)); break;
    case 18: SLOT_TransferB(); break;
    case 19: SLOT_QueueA(); break;
    case 20: SLOT_QueueB(); break;
    case 21: SLOT_SelectionChanged(); break;
    case 22: SLOT_StartQueue(); break;
    case 23: SkipTasks(); break;
    case 24: SLOT_UpdateA(); break;
    case 25: SLOT_KbftpReadReady(); break;
    case 26: SLOT_About(); break;
    case 27: SLOT_RenameA(); break;
    case 28: SLOT_RenameB(); break;
    case 29: SLOT_Quit(); break;
    case 30: SLOT_DeleteA(); break;
    case 31: SLOT_DeleteB(); break;
    default:
	return KasablancaMainWindow::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool Kasablanca::qt_emit( int _id, QUObject* _o )
{
    return KasablancaMainWindow::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool Kasablanca::qt_property( int id, int f, QVariant* v)
{
    return KasablancaMainWindow::qt_property( id, f, v);
}

bool Kasablanca::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
