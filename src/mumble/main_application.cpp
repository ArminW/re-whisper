/* Copyright (C) 2005-2010, Thorvald Natvig <thorvald@natvig.com>

   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.
   - Neither the name of the Mumble Developers nor the names of its
     contributors may be used to endorse or promote products derived from this
     software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "Overlay.h"
#include "MainWindow.h"
#include "ServerHandler.h"
#include "AudioInput.h"
#include "AudioOutput.h"
#include "AudioWizard.h"
#include "Cert.h"
#include "Database.h"
#include "Log.h"
#include "Plugins.h"
#include "Global.h"
#include "LCD.h"
#ifdef USE_BONJOUR
#include "BonjourClient.h"
#endif
#ifdef USE_DBUS
#include "DBus.h"
#endif
#ifdef USE_VLD
#include "vld.h"
#endif
#include "VersionCheck.h"
#include "NetworkConfig.h"
#include "CrashReporter.h"
#include "FileEngine.h"
#include "SocketRPC.h"

//Beginning of Whisper integration includes
#include "wError.h"
#include "wViewerHandler.h"
#include "wGameHandler.h"
#include "wConfigFile.h"
using namespace whisper;
//End of Whisper integration includes


#ifdef BOOST_NO_EXCEPTIONS
namespace boost {
	void throw_exception(std::exception const &) {
		qFatal("Boost exception caught!");
	}
}
#endif

extern void os_init();
extern char *os_lang;

class QAppMumble : public QApplication {
	public:
		QUrl quLaunchURL;
		QAppMumble(int &pargc, char **pargv) : QApplication(pargc, pargv) {}
		void commitData(QSessionManager&);
		bool event(QEvent *e);
#ifdef Q_OS_WIN
		bool winEventFilter(MSG *msg, long *result);
#endif
};

void QAppMumble::commitData(QSessionManager &) {
	// Make sure the config is saved and supress the ask on quite message
	if (g.mw) {
		g.s.save();
		g.mw->bSuppressAskOnQuit = true;
	}
}

bool QAppMumble::event(QEvent *e) {
#if QT_VERSION >= 0x040600
	if (e->type() == QEvent::FileOpen) {
		QFileOpenEvent *foe = static_cast<QFileOpenEvent *>(e);
		if (! g.mw) {
			this->quLaunchURL = foe->url();
		} else {
			g.mw->openUrl(foe->url());
		}
		return true;
	}
#endif
	return QApplication::event(e);
}

#ifdef Q_OS_WIN
bool QAppMumble::winEventFilter(MSG *msg, long *result) {
	if (QThread::currentThread() == thread()) {
		if (Global::g_global_struct && g.ocIntercept) {
			switch (msg->message) {
				case WM_MOUSELEAVE:
					*result = 0;
					return true;
				case WM_KEYDOWN:
				case WM_KEYUP:
				case WM_SYSKEYDOWN:
				case WM_SYSKEYUP:
					GlobalShortcutEngine::engine->prepareInput();
				default:
					break;
			}
		}
	}
	return QApplication::winEventFilter(msg, result);
}
#endif

int main_application(int argc, char **argv, GameHandler *pGh) {
	int res = 0;

	QT_REQUIRE_VERSION(argc, argv, "4.4.0");

#if defined(Q_OS_WIN)
	SetDllDirectory(L"");
#endif

	// Initialize application object.
	QAppMumble a(argc, argv);
	a.setApplicationName(QLatin1String("Mumble"));
	a.setOrganizationName(QLatin1String("Mumble"));
	a.setOrganizationDomain(QLatin1String("mumble.sourceforge.net"));
	a.setQuitOnLastWindowClosed(false);

	// Whisper integration
	QString sAppDirPath = a.applicationDirPath();
	WDEBUG2("Executable base dir is: %s", sAppDirPath.toAscii().data());
	a.addLibraryPath("whisper/plugins");
	/* -> is done with qt.conf
	QString sPluginBaseDirPath = sAppDirPath + "/whisper/plugins";
	a.addLibraryPath(sPluginBaseDirPath);
	WDEBUG2("Plugin base dir added: %s", sPluginBaseDirPath.toAscii().data());
	*/
	foreach (QString sPath, a.libraryPaths()) {
		WDEBUG2("- library path: %s ", sPath.toAscii().data());
	}
	//WDEBUG2("QLibraryInfo::location(QLibraryInfo::PluginsPath): %s", QLibraryInfo::location(QLibraryInfo::PluginsPath).toAscii().data());

	Global::g_global_struct = new Global();

	qsrand(QDateTime::currentDateTime().toTime_t());

#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
	os_init();
#endif

	bool bAllowMultiple = false;
	QUrl url;

	if (a.arguments().count() > 1) {
		QStringList args = a.arguments();
		for (int i = 1; i < args.count(); ++i) {
			if (args.at(i) == QLatin1String("-m")) {
				bAllowMultiple = true;
			} else if (args.at(i) == QLatin1String("-n")) {
				g.s.bSuppressIdentity = true;
			} else {
				QUrl u = QUrl::fromEncoded(args.at(i).toUtf8());
				if (u.isValid() && (u.scheme() == QLatin1String("mumble"))) {
					url = u;
				} else {
					QFile f(args.at(i));
					if (f.exists()) {
						url = QUrl::fromLocalFile(f.fileName());
					}
				}
			}
		}
	}

#ifdef USE_DBUS
#ifdef Q_OS_WIN
	// By default, windbus expects the path to dbus-daemon to be in PATH, and the path
	// should contain bin\\, and the path to the config is hardcoded as ..\etc

	{
		size_t reqSize;
		_wgetenv_s(&reqSize, NULL, 0, L"PATH");
		if (reqSize > 0) {
			STACKVAR(wchar_t, buff, reqSize+1);
			_wgetenv_s(&reqSize, buff, reqSize, L"PATH");
			QString path = QString::fromLatin1("%1;%2").arg(QDir::toNativeSeparators(a.applicationDirPath())).arg(QString::fromWCharArray(buff));
			STACKVAR(wchar_t, buffout, path.length() + 1);
			path.toWCharArray(buffout);
			_wputenv_s(L"PATH", buffout);
		}
	}
#endif
#endif

	if (! bAllowMultiple) {
		if (url.isValid()) {
			int major, minor, patch;
			major = 1;
			minor = 1;
			patch = 0;

			QString version = url.queryItemValue(QLatin1String("version"));
#ifndef USE_DBUS
			QMap<QString, QVariant> param;
			param.insert(QLatin1String("href"), url);
#endif
			MumbleVersion::get(&major, &minor, &patch, version);

			if ((major == 1) && (minor == 1)) {
				bool sent = false;
#ifdef USE_DBUS
				QDBusInterface qdbi(QLatin1String("net.sourceforge.mumble.mumble11x"), QLatin1String("/"), QLatin1String("net.sourceforge.mumble.Mumble"));

				QDBusMessage reply=qdbi.call(QLatin1String("openUrl"), QLatin1String(url.toEncoded()));
				sent = (reply.type() == QDBusMessage::ReplyMessage);
#else
				sent = SocketRPC::send(QLatin1String("Mumble11x"), QLatin1String("url"), param);
#endif
				if (sent) {
					return 0;
				} else {
					QString executable = a.applicationFilePath();
					int idx = executable.lastIndexOf(QLatin1String("mumble"));
					if (idx >= 0) {
						QStringList args;
						args << url.toString();

						executable.replace(idx, 6, QLatin1String("mumble11x"));
						if (QProcess::startDetached(executable, args))
							return 0;
					}
				}
			} else {
				bool sent = false;
#ifdef USE_DBUS
				QDBusInterface qdbi(QLatin1String("net.sourceforge.mumble.mumble"), QLatin1String("/"), QLatin1String("net.sourceforge.mumble.Mumble"));

				QDBusMessage reply=qdbi.call(QLatin1String("openUrl"), QLatin1String(url.toEncoded()));
				sent = (reply.type() == QDBusMessage::ReplyMessage);
#else
				sent = SocketRPC::send(QLatin1String("Mumble"), QLatin1String("url"), param);
#endif
				if (sent)
					return 0;
			}
		} else {
			bool sent = false;
#ifdef USE_DBUS
			QDBusInterface qdbi(QLatin1String("net.sourceforge.mumble.mumble"), QLatin1String("/"), QLatin1String("net.sourceforge.mumble.Mumble"));

			QDBusMessage reply=qdbi.call(QLatin1String("focus"));
			sent = (reply.type() == QDBusMessage::ReplyMessage);
#else
			sent = SocketRPC::send(QLatin1String("Mumble"), QLatin1String("focus"));
#endif
			if (sent)
				return 0;

		}
	}

	// Load preferences
	g.s.load();

	// Check wheter we need to enable accessibility features
#ifdef Q_OS_WIN
	// Only windows for now. Could not find any information on how to query this for osx or linux
	{
		HIGHCONTRAST hc;
		hc.cbSize = sizeof(HIGHCONTRAST);
		SystemParametersInfo(SPI_GETHIGHCONTRAST, sizeof(HIGHCONTRAST), &hc, 0);

		if (hc.dwFlags & HCF_HIGHCONTRASTON)
			g.s.bHighContrast = true;

	}
#endif

	DeferInit::run_initializers();

	if (! g.s.qsStyle.isEmpty()) {
		a.setStyle(g.s.qsStyle);
		g.qsCurrentStyle = g.s.qsStyle;
	}

	if (! g.s.qsSkin.isEmpty()) {
		QFile file(g.s.qsSkin);
		file.open(QFile::ReadOnly);
		QString styleSheet=QLatin1String(file.readAll());
		if (! styleSheet.isEmpty()) {
			QFileInfo fi(g.s.qsSkin);
			QDir::addSearchPath(QLatin1String("skin"), fi.path());
			a.setStyleSheet(styleSheet);
		}
	} else {
		a.setStyleSheet(MainWindow::defaultStyleSheet);
	}

	QDir::addSearchPath(QLatin1String("skin"),QLatin1String(":/"));
	QDir::addSearchPath(QLatin1String("translation"), QLatin1String(":/"));

	QString qsSystemLocale = QLocale::system().name();

#ifdef Q_OS_MAC
	if (os_lang) {
		qWarning("Using Mac OS X system langauge as locale name");
		qsSystemLocale = QString(os_lang);
	}
#endif

	qWarning("Locale is %s", qPrintable(qsSystemLocale));

	QString locale = g.s.qsLanguage.isEmpty() ? qsSystemLocale : g.s.qsLanguage;

	QTranslator translator;
	if (translator.load(QLatin1String("translation:mumble_") + locale))
		a.installTranslator(&translator);

	QTranslator loctranslator;
	if (loctranslator.load(QLatin1String("mumble_") + locale, a.applicationDirPath()))
		a.installTranslator(&loctranslator);

	QTranslator qttranslator;
	if (qttranslator.load(QLibraryInfo::location(QLibraryInfo::TranslationsPath) + QLatin1String("/qt_") + locale))
		a.installTranslator(&qttranslator);
	else if (qttranslator.load(QLatin1String("translation:qt_") + locale))
		a.installTranslator(&qttranslator);

	g.qsRegionalHost = qsSystemLocale;
	g.qsRegionalHost = g.qsRegionalHost.remove(QRegExp(QLatin1String("^.+_"))).toLower() + QLatin1String(".mumble.info");

	// Initialize proxy settings
	NetworkConfig::SetupProxy();

	g.nam = new QNetworkAccessManager();

/* whisper integration
#ifndef NO_CRASH_REPORT
	CrashReporter *cr = new CrashReporter();
	cr->run();
	delete cr;
#endif
*/
	// Initialize logger
	g.l = new Log();

	// Initialize database
	g.db = new Database();

#ifdef USE_BONJOUR
	// Initialize bonjour
	g.bc = new BonjourClient();
#endif

	g.o = new Overlay();
	g.o->setActive(g.s.os.bEnable);

	g.lcd = new LCD();

	// Process any waiting events before initializing our MainWindow.
	// The mumble:// URL support for Mac OS X happens through AppleEvents,
	// so we need to loop a little before we begin.
	a.processEvents();

	// Main Window
	g.mw=new MainWindow(NULL);

    //Whisper Integration
    ConfigFile& cf(ConfigFile::getInstance());
    if(cf.isValid()) {
        if(cf.getValue("debug", "debug") == "true") {
            g.mw->setVisible(true);
            g.mw->qstiIcon->setVisible(true);
        }
		else {
			g.mw->setVisible(false);
			g.mw->qstiIcon->setVisible(false);
		}
    }
	else {
	    //g.mw->show();
		g.mw->setVisible(false);
		g.mw->qstiIcon->setVisible(false);
	}


#ifdef USE_DBUS
	new MumbleDBus(g.mw);
	QDBusConnection::sessionBus().registerObject(QLatin1String("/"), g.mw);
	QDBusConnection::sessionBus().registerService(QLatin1String("net.sourceforge.mumble.mumble"));
#endif

	SocketRPC *srpc = new SocketRPC(QLatin1String("Mumble"));

	g.l->log(Log::Information, MainWindow::tr("Welcome to Mumble."));

	// Plugins
	g.p = new Plugins(NULL);
	g.p->rescanPlugins();

	MumbleFileEngineHandler *mfeh = new MumbleFileEngineHandler();

	Audio::start();

	a.setQuitOnLastWindowClosed(false);

    //Beginning of Whisper integration
	//g.gh = new ViewerHandler(0);
	g.gh = pGh;
	//g.gh = new whisper::DefaultGameHandler;
	g.gh->start();
    //ViewerHandler& rViewer(ViewerHandler::instance());
    //rViewer.start();
    //End of Whisper Integration

	// Configuration updates
	bool runaudiowizard = false;
       
//Whisper integration  TODO: run whatever kind of audio setting wizard
/*
	if (g.s.uiUpdateCounter == 0) {
		// Previous version was an pre 1.2.3 release or this is the first run
		runaudiowizard = true;
	}
*///End of whisper integration

	if (runaudiowizard) {
		AudioWizard *aw = new AudioWizard(g.mw);
		aw->exec();
		delete aw;
	}

	g.s.uiUpdateCounter = 1;

        //Whisper integration
        /*
	if (! CertWizard::validateCert(g.s.kpCertificate)) {
		QDir qd(QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation));
		QFile qf(qd.absoluteFilePath(QLatin1String("MumbleAutomaticCertificateBackup.p12")));
		if (qf.open(QIODevice::ReadOnly | QIODevice::Unbuffered)) {
			Settings::KeyPair kp = CertWizard::importCert(qf.readAll());
			qf.close();
			if (CertWizard::validateCert(kp))
				g.s.kpCertificate = kp;
		}
		if (! CertWizard::validateCert(g.s.kpCertificate)) {
			CertWizard *cw = new CertWizard(g.mw);
			cw->exec();
			delete cw;

			if (! CertWizard::validateCert(g.s.kpCertificate)) {
				g.s.kpCertificate = CertWizard::generateNewCert();
				if (qf.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered)) {
					qf.write(CertWizard::exportCert(g.s.kpCertificate));
					qf.close();
				}
			}
		}
	}
        */
        //End of whisper integration

	if (QDateTime::currentDateTime().daysTo(g.s.kpCertificate.first.first().expiryDate()) < 14)
		g.l->log(Log::Warning, CertWizard::tr("<b>Certificate Expiry:</b> Your certificate is about to expire. You need to renew it, or you will no longer be able to connect to servers you are registered on."));

#ifdef QT_NO_DEBUG
#ifdef RELEASE_BUILD
	if (g.s.bUpdateCheck)
#endif
		new VersionCheck(true, g.mw);

#ifdef SNAPSHOT_BUILD
	new VersionCheck(false, g.mw, true);
#endif
#else
	g.mw->msgBox(g.mw->tr("Skipping version check in debug mode."));
#endif
	if (g.s.bPluginOverlayCheck) {
		g.p->checkUpdates();
		g.o->checkUpdates();
	}

	if (url.isValid()) {
		OpenURLEvent *oue = new OpenURLEvent(url);
		qApp->postEvent(g.mw, oue);
#ifdef Q_OS_MAC
	} else if (! a.quLaunchURL.isEmpty()) {
		OpenURLEvent *oue = new OpenURLEvent(a.quLaunchURL);
		qApp->postEvent(g.mw, oue);
#endif
	} else {
		// Whisper integration: Do not open ConnectDialog
		// g.mw->on_qaServerConnect_triggered(true);
	}

	if (! g.bQuit)
		res=a.exec();

	//Whisper integration - debugging
	WDEBUG1("Stopping viewerHandler execution...");
	g.gh->end();
	g.gh->wait();
	/*
	if(rViewer.isRunning()) {
		rViewer.end();
        	rViewer.wait();
	}
	*/
	//WDEBUG2("viewerHandler status: %s", rViewer.isFinished() ? "Finished" : "Running");
	delete g.gh;
	g.gh = 0;

	WDEBUG1("Out of execution thread - In main.");

	g.s.save();
	WDEBUG1("After g.s.save()"); 
	//End of whisper integration debugging.


	ServerHandlerPtr sh = g.sh;

	if (sh && sh->isRunning())
		Database::setShortcuts(g.sh->qbaDigest, g.s.qlShortcuts);

	Audio::stop();

	WDEBUG1("before sh Server Handler disconnect");
	if (sh)
		sh->disconnect();

	delete mfeh;

	delete srpc;

	WDEBUG1("VGDEBUG main: before sh reset");
	g.sh.reset();
	while (sh && ! sh.unique())
		QThread::yieldCurrentThread();
	sh.reset();

	WDEBUG1("VGDEBUG main: before mw delete");
	delete g.mw;

	WDEBUG1("VGDEBUG main: before nam delete");
	delete g.nam;
	WDEBUG1("VGDEBUG main: before lcd delete");
	delete g.lcd;

	WDEBUG1("VGDEBUG main: before db delete");
	delete g.db;
	WDEBUG1("VGDEBUG main: before p delete");
	delete g.p;
	WDEBUG1("VGDEBUG main: before l delete");
	delete g.l;

#ifdef USE_BONJOUR
	delete g.bc;
#endif

	WDEBUG1("VGDEBUG main: before o delete");
	delete g.o;

	WDEBUG1("VGDEBUG main: before run_destroyers");
	DeferInit::run_destroyers();

	WDEBUG1("VGDEBUG main: before delete g_global_struct");
	delete Global::g_global_struct;
	Global::g_global_struct = NULL;

#ifndef QT_NO_DEBUG
	// Hide Qt memory leak.
	WDEBUG1("VGDEBUG main: before setDefaultCertificates");
	QSslSocket::setDefaultCaCertificates(QList<QSslCertificate>());
	// Release global protobuf memory allocations.
	WDEBUG2("VGDEBUG main: before ShutdownProtobufLIvrary, version: %d", GOOGLE_PROTOBUF_VERSION);
#if (GOOGLE_PROTOBUF_VERSION >= 2001000)
	google::protobuf::ShutdownProtobufLibrary();
#endif
#endif
	WDEBUG1("VGDEBUG end:got to the end in main");
	return res;
}
