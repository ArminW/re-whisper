include(../mumble.pri)
DEFINES *= MUMBLE
TEMPLATE = app
#TEMPLATE = lib
#CONFIG += dll
QT *= network \
    sql \
    opengl \
    xml \
    svg
TARGET = mumble
HEADERS *= BanEditor.h \
    ACLEditor.h \
    ConfigWidget.h \
    Log.h \
    AudioConfigDialog.h \
    AudioStats.h \
    AudioInput.h \
    AudioOutput.h \
    CustomElements.h \
    MainWindow.h \
    ServerHandler.h \
    About.h \
    ConnectDialog.h \
    GlobalShortcut.h \
    TextToSpeech.h \
    Settings.h \
    Database.h \
    VersionCheck.h \
    Global.h \
    UserModel.h \
    Audio.h \
    ConfigDialog.h \
    Plugins.h \
    LookConfig.h \
    Overlay.h \
    AudioWizard.h \
    ViewCert.h \
    TextMessage.h \
    NetworkConfig.h \
    LCD.h \
    Usage.h \
    Cert.h \
    ClientUser.h \
    UserEdit.h \
    Tokens.h \
    UserView.h \
    RichTextEditor.h \
    wGameHandler.h \
    wConnection.h \
    wVoiceClient.h \
    wViewerHandler.h \
    wResponse.h \
    wRequestQueue.h \
    wRequest.h \
    wMessage.h \
    wGlobal.h \
    wEvent.h \
    wError.h \
    wXmlWriter.h \
    wTalkingThread.h \
    wConfigFile.h
SOURCES *= BanEditor.cpp \
    ACLEditor.cpp \
    ConfigWidget.cpp \
    Log.cpp \
    AudioConfigDialog.cpp \
    AudioStats.cpp \
    AudioInput.cpp \
    AudioOutput.cpp \
    main.cpp \
    main_application.cpp \
    CustomElements.cpp \
    MainWindow.cpp \
    ServerHandler.cpp \
    About.cpp \
    ConnectDialog.cpp \
    Settings.cpp \
    Database.cpp \
    VersionCheck.cpp \
    Global.cpp \
    UserModel.cpp \
    Audio.cpp \
    ConfigDialog.cpp \
    Plugins.cpp \
    LookConfig.cpp \
    Overlay.cpp \
    AudioWizard.cpp \
    ViewCert.cpp \
    Messages.cpp \
    TextMessage.cpp \
    GlobalShortcut.cpp \
    NetworkConfig.cpp \
    LCD.cpp \
    Usage.cpp \
    Cert.cpp \
    ClientUser.cpp \
    UserEdit.cpp \
    Tokens.cpp \
    UserView.cpp \
    RichTextEditor.cpp \
    wConnection.cpp \
    wVoiceClient.cpp \
    wViewerHandler.cpp \
    wResponse.cpp \
    wRequestQueue.cpp \
    wRequest.cpp \
    wMessage.cpp \
    wEvent.cpp \
    wError.cpp \
    wXmlWriter.cpp \
    wTalkingThread.cpp \
    wConfigFile.cpp
SOURCES *= smallft.cpp
DIST *= ../../icons/mumble.ico \
    licenses.h \
    smallft.h \
    ../../icons/mumble.xpm \
    murmur_pch.h \
    mumble.plist
RESOURCES *= mumble.qrc \
    mumble_flags.qrc
FORMS *= ConfigDialog.ui \
    MainWindow.ui \
    ConnectDialog.ui \
    ConnectDialogEdit.ui \
    BanEditor.ui \
    ACLEditor.ui \
    Plugins.ui \
    Overlay.ui \
    LookConfig.ui \
    AudioInput.ui \
    AudioOutput.ui \
    Log.ui \
    TextMessage.ui \
    AudioStats.ui \
    NetworkConfig.ui \
    LCD.ui \
    GlobalShortcut.ui \
    GlobalShortcutTarget.ui \
    Cert.ui \
    UserEdit.ui \
    AudioWizard.ui \
    Tokens.ui \
    RichTextEditor.ui \
    RichTextEditorLink.ui
TRANSLATIONS = mumble_en.ts \
    mumble_es.ts \
    mumble_de.ts \
    mumble_fr.ts \
    mumble_pl.ts \
    mumble_ru.ts \
    mumble_cs.ts \
    mumble_it.ts \
    mumble_ja.ts \
    mumble_zh_CN.ts \
    mumble_zh_TW.ts \
    mumble_da.ts
PRECOMPILED_HEADER = mumble_pch.hpp
INCLUDEPATH *= ../bonjour
isEmpty(QMAKE_LRELEASE) { 
    QMAKE_QMAKE_BASE = $$basename(QMAKE_QMAKE)
    QMAKE_LRELEASE = $$dirname(QMAKE_QMAKE)/$$replace(QMAKE_QMAKE_BASE,qmake,lrelease)
}
unix:!CONFIG(bundled-speex):system(pkg-config --atleast-version=1.2 speexdsp):CONFIG *= no-bundled-speex
CONFIG(no-crash-report):DEFINES *= NO_CRASH_REPORT
else { 
    HEADERS *= CrashReporter.h
    SOURCES *= CrashReporter.cpp
}
CONFIG(no-bundled-speex):PKGCONFIG *= speex \
    speexdsp
!CONFIG(no-bundled-speex) { 
    INCLUDEPATH *= ../../speex/include \
        ../../speex/libspeex \
        ../../speexbuild
    LIBS *= -lspeex
}
unix:!CONFIG(bundled-celt):system(pkg-config --atleast-version=0.7.0 celt):CONFIG *= no-bundled-celt
CONFIG(no-bundled-celt):INCLUDEPATH *= /usr/include/celt
!CONFIG(no-bundled-celt):INCLUDEPATH *= ../../celt-0.7.0-src/libcelt
!win32:QMAKE_CXXFLAGS *= -Wall \
    -Wextra
!CONFIG(no-dbus):CONFIG *= dbus
!CONFIG(no-g15):CONFIG *= g15
!CONFIG(no-bonjour):CONFIG *= bonjour
win32 { 
    RC_FILE = mumble.rc
    HEADERS *= GlobalShortcut_win.h \
        TaskList.h
    SOURCES *= GlobalShortcut_win.cpp \
        TextToSpeech_win.cpp \
        Overlay_win.cpp \
        os_win.cpp \
        TaskList.cpp
    LIBS *= -l"$$(DXSDK_DIR)Lib/x86/dxguid" \
        -l"$$(DXSDK_DIR)Lib/x86/dinput8" \
        -lsapi \
        -lole32 \
        -lws2_32 \
        -ladvapi32 \
        -lwintrust \
        -ldbghelp \
        -llibsndfile-1 \
        -lshell32 \
        -lshlwapi
    LIBS *= -ldelayimp \
        -delayload:speex.dll
    LIBPATH *= /dev/OpenSSL/lib \
        /dev/libsndfile
    DEFINES *= WIN32
    INCLUDEPATH *= /dev/OpenSSL/include \
        /dev/libsndfile/include
    !CONFIG(no-asio):CONFIG *= asio
    !CONFIG(no-directsound):CONFIG *= directsound
    !CONFIG(no-wasapi):CONFIG *= wasapi
}
unix { 
    HAVE_PULSEAUDIO = $$system(pkg-config --modversion --silence-errors libpulse)
    HAVE_PORTAUDIO = $$system(pkg-config --modversion --silence-errors portaudio-2.0)
    !isEmpty(HAVE_PORTAUDIO):!CONFIG(no-portaudio):CONFIG *= portaudio
    !isEmpty(HAVE_PULSEAUDIO):!CONFIG(no-pulseaudio) { 
        CONFIG -= portaudio
        CONFIG *= pulseaudio
    }
    !CONFIG(no-bundled-speex) { 
        QMAKE_CFLAGS *= -I../../speex/include \
            -I../../speexbuild
        QMAKE_CXXFLAGS *= -I../../speex/include \
            -I../../speexbuild
        QMAKE_CXXFLAGS_RELEASE *= -I../../speex/include \
            -I../../speexbuild
        QMAKE_CXXFLAGS_DEBUG *= -I../../speex/include \
            -I../../speexbuild
    }
    CONFIG *= link_pkgconfig
    PKGCONFIG *= openssl \
        sndfile
    contains(UNAME, Linux) { 
        !CONFIG(no-oss):CONFIG *= oss
        !CONFIG(no-alsa):CONFIG *= alsa
        !CONFIG(no-speechd):CONFIG *= speechd
        HEADERS *= GlobalShortcut_unix.h
        SOURCES *= GlobalShortcut_unix.cpp \
            TextToSpeech_unix.cpp \
            Overlay_unix.cpp
        LIBS *= -lrt
    }
    !macx { 
        PKGCONFIG *= x11
        LIBS *= -lXi
    }
    macx { 
        TARGET = Mumble
        ICON = ../../icons/mumble.icns
        QMAKE_INFO_PLIST = mumble.plist
        QMAKE_PKGINFO_TYPEINFO = MBLE
        LIBS += -framework \
            Security
        HEADERS *= GlobalShortcut_macx.h \
            ConfigDialogDelegate.h
        SOURCES *= TextToSpeech_macx.cpp \
            Overlay_unix.cpp \
            GlobalShortcut_macx.cpp \
            os_macx.cpp
        !CONFIG(no-cocoa) { 
            SOURCES -= ConfigDialog.cpp
            SOURCES += ConfigDialog.mm \
                ConfigDialogDelegate.mm
        }
        
        # CoreAudio
        LIBS += -framework \
            CoreAudio \
            -framework \
            AudioUnit \
            -framework \
            AudioToolbox
        SOURCES += CoreAudio.cpp
        HEADERS += CoreAudio.h
    }
}
alsa { 
    DEFINES *= USE_ALSA
    PKGCONFIG *= alsa
    HEADERS *= ALSAAudio.h
    SOURCES *= ALSAAudio.cpp
}
oss { 
    DEFINES *= USE_OSS
    HEADERS *= OSS.h
    SOURCES *= OSS.cpp
    INCLUDEPATH *= /usr/lib/oss/include
}
pulseaudio { 
    DEFINES *= USE_PULSEAUDIO
    PKGCONFIG *= libpulse
    HEADERS *= PulseAudio.h
    SOURCES *= PulseAudio.cpp
}
portaudio { 
    DEFINES *= USE_PORTAUDIO
    PKGCONFIG *= portaudio-2.0
    HEADERS *= PAAudio.h
    SOURCES *= PAAudio.cpp
}
asio { 
    DEFINES *= USE_ASIO
    HEADERS *= ASIOInput.h
    SOURCES *= ASIOInput.cpp
    FORMS *= ASIOInput.ui
    INCLUDEPATH *= ../../asio/common \
        ../../asio/host \
        ../../asio/host/pc
}
bonjour { 
    DEFINES *= USE_BONJOUR
    HEADERS *= ../bonjour/bonjourrecord.h \
        ../bonjour/bonjourserviceresolver.h \
        ../bonjour/bonjourservicebrowser.h \
        BonjourClient.h
    SOURCES *= ../bonjour/bonjourserviceresolver.cpp \
        ../bonjour/bonjourservicebrowser.cpp \
        BonjourClient.cpp
    win32 { 
        INCLUDEPATH *= /dev/Bonjour/include
        LIBPATH *= /dev/Bonjour/lib/win32
        LIBS *= -lDNSSD \
            -delayload:DNSSD.DLL
    }
    unix:!macx { 
        system(pkg-config --exists avahi-compat-libdns_sd avahi-client):PKGCONFIG *= avahi-compat-libdns_sd \
            avahi-client
        else:LIBS *= -ldns_sd
    }
}
dbus { 
    DEFINES *= USE_DBUS
    CONFIG *= qdbus
    HEADERS *= DBus.h
    SOURCES *= DBus.cpp
}
speechd { 
    DEFINES *= USE_SPEECHD
    LIBS *= -lspeechd
}
directsound { 
    DEFINES *= USE_DIRECTSOUND
    HEADERS *= DirectSound.h
    SOURCES *= DirectSound.cpp
    LIBS *= -ldsound
}
wasapi { 
    DEFINES *= USE_WASAPI
    HEADERS *= WASAPI.h
    SOURCES *= WASAPI.cpp
    LIBS *= -lAVRT \
        -delayload:AVRT.DLL
}
g15 { 
    DEFINES *= USE_G15
    unix:!macx { 
        SOURCES *= G15LCDEngine_unix.cpp
        HEADERS *= G15LCDEngine_unix.h
        LIBS *= -lg15daemon_client
    }
    else { 
        SOURCES *= G15LCDEngine_lglcd.cpp
        HEADERS *= G15LCDEngine_lglcd.h \
            ../../g15helper/g15helper.h
    }
}
CONFIG(no-update):DEFINES *= NO_UPDATE_CHECK
!CONFIG(no-embed-qt-translations) { 
    QT_TRANSDIR = $$[QT_INSTALL_TRANSLATIONS]/
    QT_TRANSDIR = $$replace(QT_TRANSDIR,/,$${DIR_SEPARATOR})
    QT_TRANSLATION_FILES *= qt_de.qm \
        qt_es.qm \
        qt_fr.qm \
        qt_ru.qm \
        qt_pl.qm \
        qt_ja_JP.qm \
        qt_zh_CN.qm \
        qt_zh_TW.qm
    copytrans.output = ${QMAKE_FILE_NAME}
    copytrans.commands = $$QMAKE_COPY \
        $${QT_TRANSDIR}${QMAKE_FILE_NAME} \
        ${QMAKE_FILE_OUT}
    copytrans.input = QT_TRANSLATION_FILES
    copytrans.CONFIG *= no_link \
        target_predeps
    QMAKE_EXTRA_COMPILERS *= copytrans
    RESOURCES *= mumble_qt.qrc
}
!CONFIG(no-embed-tango-icons):RESOURCES *= mumble_tango.qrc
lrel.output = ${QMAKE_FILE_BASE}.qm
lrel.commands = $${QMAKE_LRELEASE} \
    ${QMAKE_FILE_NAME}
lrel.input = TRANSLATIONS
lrel.CONFIG *= no_link \
    target_predeps
QMAKE_EXTRA_COMPILERS *= lrel
include(../../symbols.pri)
