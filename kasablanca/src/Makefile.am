# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# these are the headers for your project
noinst_HEADERS = kasablanca.h bookmarkdialog.h customconnectdialog.h diritem.h fileexistsdialog.h fileitem.h kbitem.h remotefileinfo.h siteinfo.h transferdir.h transferfile.h transferitem.h kbprocess.h bookmarks.h ftplib.h

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
kasablanca_SOURCES = main.cpp kasablanca.cpp bookmarkdialog.cpp customconnectdialog.cpp diritem.cpp fileexistsdialog.cpp fileitem.cpp kbitem.cpp remotefileinfo.cpp siteinfo.cpp transferdir.cpp transferfile.cpp transferitem.cpp kbprocess.cpp Q_bookmarkdialog.ui Q_customconnectdialog.ui Q_fileexistsdialog.ui Q_mainwindow.ui bookmarks.cpp eventhandler.cpp ftpthread.cpp kbdirectory.cpp ftplib.cpp
kasablanca_LDFLAGS = $(KDE_RPATH) $(all_libraries)
kasablanca_LDADD = -lssl $(LIB_KDEUI)

# this is where the desktop file will go 
shelldesktopdir = $(kde_appsdir)/Utilities
shelldesktop_DATA = kasablanca.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir = $(kde_datadir)/kasablanca
shellrc_DATA = kasablancaui.rc about.png

SUBDIRS = kbftp
