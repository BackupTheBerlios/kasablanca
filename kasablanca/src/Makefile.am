# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# these are the headers for your project
noinst_HEADERS = kasablanca.h bookmarkdialog.h customconnectdialog.h fileexistsdialog.h kbitem.h siteinfo.h bookmarks.h ftplib.h ftpsession.h kbfileinfo.h kbdirinfo.h kbtransferitem.h kbtransferdir.h kbtransferfile.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kasablanca.pot

KDE_ICON = kasablanca

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = kasablanca

# the application source, library search path, and link libraries
kasablanca_SOURCES = main.cpp kasablanca.cpp bookmarkdialog.cpp customconnectdialog.cpp fileexistsdialog.cpp kbitem.cpp siteinfo.cpp Q_bookmarkdialog.ui Q_customconnectdialog.ui Q_fileexistsdialog.ui Q_mainwindow.ui bookmarks.cpp eventhandler.cpp ftpthread.cpp ftplib.cpp ftpsession.cpp kbfileinfo.cpp kbdirinfo.cpp kbdir.cpp kbfile.cpp kbtransferitem.cpp kbtransferdir.cpp kbtransferfile.cpp
kasablanca_LDFLAGS = $(KDE_RPATH) $(all_libraries)
kasablanca_LDADD = -lssl $(LIB_KDEUI)

# this is where the desktop file will go 
shelldesktopdir = $(kde_appsdir)/Utilities
shelldesktop_DATA = kasablanca.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir = $(kde_datadir)/kasablanca
shellrc_DATA = kasablancaui.rc about.png


