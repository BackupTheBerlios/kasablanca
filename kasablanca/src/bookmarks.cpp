//
// C++ Implementation: bookmarks
//
// Description: 
//
//
// Author: Big Biff <bigbiff@chunkyfilms.org>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "bookmarks.h"
#include "siteinfo.h"

#include <qdom.h>
#include <qfile.h>
#include <qdir.h>
#include <qtextstream.h>

Bookmarks::Bookmarks()
{
}

Bookmarks::~Bookmarks()
{
}

vector<siteinfo> Bookmarks::getBookmarks()
{
    ParseBookmarks();

    return(m_bookmarks);
}

int Bookmarks::setBookmarks(vector<siteinfo> bmarks)
{
    m_bookmarks = bmarks;
    return(WriteBookmarks());
}

int Bookmarks::ParseBookmarks()
{
    QDomDocument doc( "KasablancaBookmarks" );
    QFile filein(QDir::homeDirPath() + "/.kasablanca/bookmarks.xml");
    
    if( !filein.open( IO_ReadOnly ) ) return -1;
    
    if( !doc.setContent( &filein ) )
    {
        filein.close();
         return -2;
    }
    
    filein.close();

    QDomElement root = doc.documentElement();

    if( root.tagName() != "kasablanca" ) return -3;

    QDomNode curNode = root.firstChild();
    m_bookmarks.clear();
    
    while( !curNode.isNull() )
    {
        QDomElement siteElement = curNode.toElement();

        if( !siteElement.isNull() )
        {
            if( siteElement.tagName() == "site" )
            {
                siteinfo entry;
                entry.SetPasv(1);
                entry.SetTls(0);
                entry.SetName(siteElement.attribute( "name", "" ).latin1());

                QDomNode siteNode = siteElement.firstChild();

                while( !siteNode.isNull() )
                {
                    QDomElement curElement = siteNode.toElement();

                    if( !curElement.isNull() )
                    {
                        if( curElement.tagName() == "user" ) entry.SetUser(curElement.text().latin1());
                        if( curElement.tagName() == "pass" ) entry.SetPass(curElement.text().latin1());
                        if( curElement.tagName() == "info" ) entry.SetInfo(curElement.text().latin1());
                        if( curElement.tagName() == "pasv" ) entry.SetPasv(curElement.text().toInt());
                        if( curElement.tagName() == "tls" ) entry.SetTls(curElement.text().toInt());
                    }

                    siteNode = siteNode.nextSibling();
                }

                if (entry.CheckContent()) m_bookmarks.push_back(entry);
            }
        }

        curNode = curNode.nextSibling();
    }

    return 1;
}

int Bookmarks::WriteBookmarks()
{
    QDomDocument doc( "KasablancaBookmarks" );
    QDomElement root = doc.createElement( "kasablanca" );
    doc.appendChild( root );

    for (int i = 0; i < static_cast<int>(m_bookmarks.size()); i++)
    {
        if (m_bookmarks.at(i).CheckContent())
        {
                QDomElement siteElement = doc.createElement("site");
                siteElement.setAttribute("name", m_bookmarks.at(i).GetName());
                root.appendChild( siteElement );

                QDomText text;
                QDomElement curElement;

                curElement = doc.createElement( "user" );
                siteElement.appendChild( curElement );
                text = doc.createTextNode(m_bookmarks.at(i).GetUser());
                curElement.appendChild(text);

                curElement = doc.createElement( "pass" );
                siteElement.appendChild( curElement );
                text = doc.createTextNode(m_bookmarks.at(i).GetPass());
                curElement.appendChild(text);

                curElement = doc.createElement( "info" );
                siteElement.appendChild( curElement );
                text = doc.createTextNode(m_bookmarks.at(i).GetInfo());
                curElement.appendChild(text);

                curElement = doc.createElement( "pasv" );
                siteElement.appendChild( curElement );
                text = doc.createTextNode(QString::number(m_bookmarks.at(i).GetPasv()));
                curElement.appendChild(text);

                curElement = doc.createElement( "tls" );
                siteElement.appendChild( curElement );
                text = doc.createTextNode(QString::number(m_bookmarks.at(i).GetTls()));
                curElement.appendChild(text);
        }
    }

    QFile fileout( QDir::homeDirPath() + "/.kasablanca/bookmarks.xml" );
    
    if( !fileout.open( IO_WriteOnly ) ) return -1;

    QTextStream ts( &fileout );
    ts << doc.toString();

    fileout.close();

    return 1;
}
