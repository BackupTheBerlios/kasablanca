/*
 * Copyright (C) 2004 Magnus Kulke <mkulke@magnusmachine>
 */

#include "kasablanca.h"
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <qtextedit.h>

static const char description[] =
    I18N_NOOP("A KDE Ftp Client");

static const char version[] = "0.4-cvs";

static KCmdLineOptions options[] =
{
//    { "+[URL]", I18N_NOOP( "Document to open." ), 0 },
    KCmdLineLastOption
};

int main(int argc, char **argv)
{
    KAboutData about("kasablanca", I18N_NOOP("kasablanca"), version, description,
                     KAboutData::License_GPL, "(C) 2004 Magnus Kulke", 0, 0,
							"sikor_sxe@radicalapproach.de");
    about.addAuthor( "Magnus Kulke", 0, "sikor_sxe@radicalapproach.de" );
    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions( options );
    KApplication app;
    Kasablanca *mainWin = 0;

    if (app.isRestored())
    {
        //RESTORE(Kasablanca);
    }
    else
    {
        // no session.. just start up normally
        KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

        /// @todo do something with the command line args here

        mainWin = new Kasablanca();
        app.setMainWidget( mainWin );

		mainWin->m_tempdirname = QString::number((int) time(NULL) & 0xffff) + ".dir";
		mainWin->m_version = version;
		mainWin->setCaption("Kasablanca " + QString(version));
		
		mainWin->show();

      args->clear();
    }

    // mainWin has WDestructiveClose flag by default, so it will delete itself.
    return app.exec();
}

