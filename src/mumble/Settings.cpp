/* Copyright (C) 2005-2010, Thorvald Natvig <thorvald@natvig.com>
   Copyright (C) 2009, Stefan Hacker <dd0t@users.sourceforge.net>

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



#include "Settings.h"
#include "Log.h"
#include "Global.h"
#include "AudioInput.h"
#include "Cert.h"

#include "wError.h" // VGDEBUG
using namespace whisper;

bool Shortcut::isServerSpecific() const {
	if (qvData.canConvert<ShortcutTarget>()) {
		const ShortcutTarget &sc = qvariant_cast<ShortcutTarget>(qvData);
		return sc.isServerSpecific();
	}
	return false;
}

bool Shortcut::operator <(const Shortcut &other) const {
	return (iIndex < other.iIndex);
}

bool Shortcut::operator ==(const Shortcut &other) const {
	return (iIndex == other.iIndex) && (qlButtons == other.qlButtons) && (qvData == other.qvData) && (bSuppress == other.bSuppress);
}

ShortcutTarget::ShortcutTarget() {
	bUsers = true;
	iChannel = -3;
	bLinks = bChildren = bForceCenter = false;
}

bool ShortcutTarget::isServerSpecific() const {
	return (! bUsers && (iChannel >= 0));
}

bool ShortcutTarget::operator ==(const ShortcutTarget &o) const {
	if ((bUsers != o.bUsers) || (bForceCenter != o.bForceCenter))
		return false;
	if (bUsers)
		return (qlUsers == o.qlUsers) && (qlSessions == o.qlSessions);
	else
		return (iChannel == o.iChannel) && (bLinks == o.bLinks) && (bChildren == o.bChildren) && (qsGroup == o.qsGroup);
}

quint32 qHash(const ShortcutTarget &t) {
	quint32 h = t.bForceCenter ? 0x55555555 : 0xaaaaaaaa;
	if (t.bUsers) {
		foreach(unsigned int u, t.qlSessions)
			h ^= u;
	} else {
		h ^= t.iChannel;
		if (t.bLinks)
			h ^= 0x80000000;
		if (t.bChildren)
			h ^= 0x40000000;
		h ^= qHash(t.qsGroup);
		h = ~h;
	}
	return h;
}

quint32 qHash(const QList<ShortcutTarget> &l) {
	quint32 h = l.count();
	foreach(const ShortcutTarget &st, l)
		h ^= qHash(st);
	return h;
}

QDataStream &operator<<(QDataStream &qds, const ShortcutTarget &st) {
	qds << st.bUsers << st.bForceCenter;

	if (st.bUsers)
		return qds << st.qlUsers;
	else
		return qds << st.iChannel << st.qsGroup << st.bLinks << st.bChildren;
}

QDataStream &operator>>(QDataStream &qds, ShortcutTarget &st) {
	qds >> st.bUsers >> st.bForceCenter;
	if (st.bUsers)
		return qds >> st.qlUsers;
	else
		return qds >> st.iChannel >> st.qsGroup >> st.bLinks >> st.bChildren;
}

const QString Settings::cqsDefaultPushClickOn = QLatin1String(":/on.ogg");
const QString Settings::cqsDefaultPushClickOff = QLatin1String(":/off.ogg");

Settings::Settings() {
	qRegisterMetaType<ShortcutTarget>("ShortcutTarget");
	qRegisterMetaTypeStreamOperators<ShortcutTarget>("ShortcutTarget");
	qRegisterMetaType<QVariant>("QVariant");

	atTransmit = VAD;
	bTransmitPosition = false;
	bMute = bDeaf = false;
        //Whisper integration
        bTTS = false;
	iTTSVolume = 75;
	iTTSThreshold = 250;
	iQuality = 40000;
	fVolume = 1.0f;
	fOtherVolume = 0.5f;
	bAttenuateOthersOnTalk = false;
	bAttenuateOthers = true;
	iMinLoudness = 1000;
	iVoiceHold = 50;
	iJitterBufferSize = 1;
	iFramesPerPacket = 2;
	iNoiseSuppress = -30;
	iIdleTime = 0;
	vsVAD = SignalToNoise;
	fVADmin = 0.80f;
	fVADmax = 0.98f;

	bPushClick = false;
	qsPushClickOn = cqsDefaultPushClickOn;
	qsPushClickOff = cqsDefaultPushClickOff;

	bUserTop = false;

	bWhisperFriends = false;

	uiDoublePush = 0;
	bExpert = false;

#ifdef NO_UPDATE_CHECK
	bUpdateCheck = false;
	bPluginCheck = false;
#else
	bUpdateCheck = true;
	bPluginCheck = true;
#endif

	qsImagePath = QDesktopServices::storageLocation(QDesktopServices::PicturesLocation);

	bFirstTime = true;
	ceExpand = ChannelsWithUsers;
	ceChannelDrag = Ask;
	bShowChatbar = true;
	bMinimalView = false;
	bHideFrame = false;
	aotbAlwaysOnTop = OnTopNever;
	bAskOnQuit = true;
	bHideTray = true;
	bStateInTray = true;
	bUsage = true;
	bShowUserCount = false;
	wlWindowLayout = LayoutClassic;

	ssFilter = ShowReachable;

	iOutputDelay = 5;

	qsALSAInput=QLatin1String("default");
	qsALSAOutput=QLatin1String("default");

	bEcho = false;
	bEchoMulti = true;

	iPortAudioInput = -1; // default device
	iPortAudioOutput = -1; // default device

	bPositionalAudio = false;
	bPositionalHeadphone = false;
	fAudioMinDistance = 1.0f;
	fAudioMaxDistance = 15.0f;
	fAudioMaxDistVolume = 0.80f;
	fAudioBloom = 0.5f;

        //Whisper integration
        bOverlayEnable = false;
	bOverlayUserTextures=true;
	osOverlay = All;
	bOverlayAlwaysSelf = true;
	fOverlayX = 0.8f;
	fOverlayY = 0.0f;
	bOverlayTop = false;
	bOverlayBottom = true;
	bOverlayLeft = true;
	bOverlayRight = false;
#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
	qfOverlayFont = QFont(QLatin1String("Verdana"), 20);
#else
	qfOverlayFont = QFont(QLatin1String("Arial"), 20);
#endif
	fOverlayHeight = .03f;
	qcOverlayUser = QColor(255,255,255,128);
	qcOverlayTalking = QColor(255,255,196,255);
	qcOverlayWhisper = QColor(255,128,128,255);
	qcOverlayChannel = QColor(192,192,255,192);
	qcOverlayChannelTalking = QColor(224,224,255,255);

	iLCDUserViewMinColWidth = 50;
	iLCDUserViewSplitterWidth = 2;

	// Network settings
	bTCPCompat = false;
	bQoS = true;
	bReconnect = true;
	ptProxyType = NoProxy;
	usProxyPort = 0;

	iMaxImageSize = ciDefaultMaxImageSize;
	iMaxImageWidth = 1024; // Allow 1024x1024 resolution
	iMaxImageHeight = 1024;
	bSuppressIdentity = false;

#if defined(AUDIO_TEST)
	lmLoopMode = Server;
#else
	lmLoopMode = None;
#endif
	dPacketLoss = 0;
	dMaxPacketDelay = 0.0f;

	for (int i=Log::firstMsgType;i<=Log::lastMsgType;++i)
		qmMessages.insert(i, Settings::LogConsole | Settings::LogBalloon | Settings::LogTTS);

	for (int i=Log::firstMsgType;i<=Log::lastMsgType;++i)
		qmMessageSounds.insert(i, QString());

	qmMessageSounds[Log::CriticalError] = QLatin1String(":/Critical.ogg");
	qmMessageSounds[Log::PermissionDenied] = QLatin1String(":/PermissionDenied.ogg");
	qmMessageSounds[Log::SelfMute] = QLatin1String(":/SelfMutedDeafened.ogg");
	qmMessageSounds[Log::ServerConnected] = QLatin1String(":/ServerConnected.ogg");
	qmMessageSounds[Log::ServerDisconnected] = QLatin1String(":/ServerDisconnected.ogg");
	qmMessageSounds[Log::TextMessage] = QLatin1String(":/TextMessage.ogg");
	qmMessageSounds[Log::ChannelJoin] = QLatin1String(":/UserJoinedChannel.ogg");
	qmMessageSounds[Log::ChannelLeave] = QLatin1String(":/UserLeftChannel.ogg");
	qmMessageSounds[Log::YouMutedOther] = QLatin1String(":/UserMutedYouOrByYou.ogg");
	qmMessageSounds[Log::YouMuted] = QLatin1String(":/UserMutedYouOrByYou.ogg");
	qmMessageSounds[Log::YouKicked] = QLatin1String(":/UserKickedYouOrByYou.ogg");

	qmMessages[Log::DebugInfo] = Settings::LogConsole;
	qmMessages[Log::Warning] = Settings::LogConsole | Settings::LogBalloon;
	qmMessages[Log::Information] = Settings::LogConsole;
	qmMessages[Log::UserJoin] = Settings::LogConsole;
	qmMessages[Log::UserLeave] = Settings::LogConsole;
	qmMessages[Log::UserKicked] = Settings::LogConsole;
	qmMessages[Log::OtherSelfMute] = Settings::LogConsole;
	qmMessages[Log::OtherMutedOther] = Settings::LogConsole;
}

bool Settings::doEcho() const {
	if (! bEcho)
		return false;

	if (AudioInputRegistrar::qmNew) {
		AudioInputRegistrar *air = AudioInputRegistrar::qmNew->value(qsAudioInput);
		if (air) {
			if (air->canEcho(qsAudioOutput))
				return true;
		}
	}
	return false;
}

bool Settings::doPositionalAudio() const {
	return bPositionalAudio;
}

#include BOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()


BOOST_TYPEOF_REGISTER_TYPE(Settings::AudioTransmit);
BOOST_TYPEOF_REGISTER_TYPE(Settings::VADSource);
BOOST_TYPEOF_REGISTER_TYPE(Settings::LoopMode)
BOOST_TYPEOF_REGISTER_TYPE(Settings::OverlayShow)
BOOST_TYPEOF_REGISTER_TYPE(Settings::ProxyType)
BOOST_TYPEOF_REGISTER_TYPE(Settings::ChannelExpand)
BOOST_TYPEOF_REGISTER_TYPE(Settings::ChannelDrag)
BOOST_TYPEOF_REGISTER_TYPE(Settings::ServerShow)
BOOST_TYPEOF_REGISTER_TYPE(Settings::WindowLayout)
BOOST_TYPEOF_REGISTER_TYPE(Settings::AlwaysOnTopBehaviour)
BOOST_TYPEOF_REGISTER_TYPE(QString)
BOOST_TYPEOF_REGISTER_TYPE(QByteArray)
BOOST_TYPEOF_REGISTER_TYPE(QColor)
BOOST_TYPEOF_REGISTER_TYPE(QVariant)
BOOST_TYPEOF_REGISTER_TYPE(QFont)
BOOST_TYPEOF_REGISTER_TEMPLATE(QList, 1)

#define SAVELOAD(var,name) var = qvariant_cast<BOOST_TYPEOF(var)>(g.qs->value(QLatin1String(name), var))
#define LOADENUM(var, name) var = static_cast<BOOST_TYPEOF(var)>(g.qs->value(QLatin1String(name), var).toInt())

void Settings::load() {
	SAVELOAD(bMute, "audio/mute");
	SAVELOAD(bDeaf, "audio/deaf");
	LOADENUM(atTransmit, "audio/transmit");
	SAVELOAD(uiDoublePush, "audio/doublepush");
	SAVELOAD(bPushClick, "audio/pushclick");
	SAVELOAD(qsPushClickOn, "audio/pushclickon");
	SAVELOAD(qsPushClickOff, "audio/pushclickoff");
	SAVELOAD(iQuality, "audio/quality");
	SAVELOAD(iMinLoudness, "audio/loudness");
	SAVELOAD(fVolume, "audio/volume");
	SAVELOAD(fOtherVolume, "audio/othervolume");
	SAVELOAD(bAttenuateOthers, "audio/attenuateothers");
	SAVELOAD(bAttenuateOthersOnTalk, "audio/attenuateothersontalk");
	LOADENUM(vsVAD, "audio/vadsource");
	SAVELOAD(fVADmin, "audio/vadmin");
	SAVELOAD(fVADmax, "audio/vadmax");
	SAVELOAD(iNoiseSuppress, "audio/noisesupress");
	SAVELOAD(iVoiceHold, "audio/voicehold");
	SAVELOAD(iOutputDelay, "audio/outputdelay");
	SAVELOAD(iIdleTime, "audio/idletime");
	SAVELOAD(fAudioMinDistance, "audio/mindistance");
	SAVELOAD(fAudioMaxDistance, "audio/maxdistance");
	SAVELOAD(fAudioMaxDistVolume, "audio/maxdistancevolume");
	SAVELOAD(fAudioBloom, "audio/bloom");
	SAVELOAD(bEcho, "audio/echo");
	SAVELOAD(bEchoMulti, "audio/echomulti");
	SAVELOAD(bPositionalAudio, "audio/positional");
	SAVELOAD(bPositionalHeadphone, "audio/headphone");
	SAVELOAD(qsAudioInput, "audio/input");
	SAVELOAD(qsAudioOutput, "audio/output");
	SAVELOAD(bWhisperFriends, "audio/whisperfriends");

	SAVELOAD(iJitterBufferSize, "net/jitterbuffer");
	SAVELOAD(iFramesPerPacket, "net/framesperpacket");

	SAVELOAD(qsASIOclass, "asio/class");
	SAVELOAD(qlASIOmic, "asio/mic");
	SAVELOAD(qlASIOspeaker, "asio/speaker");

	SAVELOAD(qsWASAPIInput, "wasapi/input");
	SAVELOAD(qsWASAPIOutput, "wasapi/output");

	SAVELOAD(qsALSAInput, "alsa/input");
	SAVELOAD(qsALSAOutput, "alsa/output");

	SAVELOAD(qsPulseAudioInput, "pulseaudio/input");
	SAVELOAD(qsPulseAudioOutput, "pulseaudio/output");

	SAVELOAD(qsOSSInput, "oss/input");
	SAVELOAD(qsOSSOutput, "oss/output");

	SAVELOAD(qsCoreAudioInput, "coreaudio/input");
	SAVELOAD(qsCoreAudioOutput, "coreaudio/output");

	SAVELOAD(iPortAudioInput, "portaudio/input");
	SAVELOAD(iPortAudioOutput, "portaudio/output");

	SAVELOAD(qbaDXInput, "directsound/input");
	SAVELOAD(qbaDXOutput, "directsound/output");

	SAVELOAD(bTTS, "tts/enable");
	SAVELOAD(iTTSVolume, "tts/volume");
	SAVELOAD(iTTSThreshold, "tts/threshold");

	SAVELOAD(bOverlayEnable, "overlay/enable");
	LOADENUM(osOverlay, "overlay/show");
	SAVELOAD(bOverlayUserTextures, "overlay/usertextures");
	SAVELOAD(bOverlayAlwaysSelf, "overlay/alwaysself");
	SAVELOAD(fOverlayX, "overlay/x");
	SAVELOAD(fOverlayY, "overlay/y");
	SAVELOAD(bTransmitPosition, "audio/postransmit");
	SAVELOAD(bOverlayTop, "overlay/top");
	SAVELOAD(bOverlayBottom, "overlay/bottom");
	SAVELOAD(bOverlayLeft, "overlay/left");
	SAVELOAD(bOverlayRight, "overlay/right");
	SAVELOAD(qfOverlayFont, "overlay/font");
	SAVELOAD(fOverlayHeight, "overlay/height");
	SAVELOAD(qcOverlayUser, "overlay/user");
	SAVELOAD(qcOverlayTalking, "overlay/talking");
	SAVELOAD(qcOverlayChannel, "overlay/channel");
	SAVELOAD(qcOverlayChannelTalking, "overlay/channeltalking");

	// Network settings
	SAVELOAD(bTCPCompat, "net/tcponly");
	SAVELOAD(bQoS, "net/qos");
	SAVELOAD(bReconnect, "net/reconnect");
	SAVELOAD(bSuppressIdentity, "net/suppress");
	LOADENUM(ptProxyType, "net/proxytype");
	SAVELOAD(qsProxyHost, "net/proxyhost");
	SAVELOAD(usProxyPort, "net/proxyport");
	SAVELOAD(qsProxyUsername, "net/proxyusername");
	SAVELOAD(qsProxyPassword, "net/proxypassword");
	SAVELOAD(iMaxImageSize, "net/maximagesize");
	SAVELOAD(iMaxImageWidth, "net/maximagewidth");
	SAVELOAD(iMaxImageHeight, "net/maximageheight");

	SAVELOAD(bExpert, "ui/expert");
	SAVELOAD(qsLanguage, "ui/language");
	SAVELOAD(qsStyle, "ui/style");
	SAVELOAD(qsSkin, "ui/skin");
	LOADENUM(ceExpand, "ui/expand");
	LOADENUM(ceChannelDrag, "ui/drag");
	LOADENUM(aotbAlwaysOnTop, "ui/alwaysontop");
	SAVELOAD(bAskOnQuit, "ui/askonquit");
	SAVELOAD(bShowChatbar, "ui/shotchatbar");
	SAVELOAD(bMinimalView, "ui/minimalview");
	SAVELOAD(bHideFrame, "ui/hideframe");
	SAVELOAD(bUserTop, "ui/usertop");
	SAVELOAD(bFirstTime, "ui/firsttime120");
	SAVELOAD(qbaMainWindowGeometry, "ui/geometry");
	SAVELOAD(qbaMainWindowState, "ui/state");
	SAVELOAD(qbaMinimalViewGeometry, "ui/minimalviewgeometry");
	SAVELOAD(qbaConfigGeometry, "ui/ConfigGeometry");
	LOADENUM(wlWindowLayout, "ui/WindowLayout");
	SAVELOAD(qbaSplitterState, "ui/splitter");
	SAVELOAD(qbaHeaderState, "ui/header");
	SAVELOAD(qsUsername, "ui/username");
	SAVELOAD(qsLastServer, "ui/server");
	LOADENUM(ssFilter, "ui/serverfilter");
#ifndef NO_UPDATE_CHECK
	SAVELOAD(bUpdateCheck, "ui/updatecheck");
	SAVELOAD(bPluginCheck, "ui/plugincheck");
#endif
	SAVELOAD(bHideTray, "ui/hidetray");
	SAVELOAD(bStateInTray, "ui/stateintray");
	SAVELOAD(bUsage, "ui/usage");
	SAVELOAD(bShowUserCount, "ui/showusercount");
	SAVELOAD(qsImagePath, "ui/imagepath");

	SAVELOAD(iLCDUserViewMinColWidth, "lcd/userview/mincolwidth");
	SAVELOAD(iLCDUserViewSplitterWidth, "lcd/userview/splitterwidth");

	QByteArray qba = qvariant_cast<QByteArray>(g.qs->value(QLatin1String("net/certificate")));
	if (! qba.isEmpty())
		kpCertificate = CertWizard::importCert(qba);

	int nshorts = g.qs->beginReadArray(QLatin1String("shortcuts"));
	for (int i=0;i<nshorts;i++) {
		g.qs->setArrayIndex(i);
		Shortcut s;

		s.iIndex = -2;

		SAVELOAD(s.iIndex, "index");
		SAVELOAD(s.qlButtons, "keys");
		SAVELOAD(s.bSuppress, "suppress");
		s.qvData = g.qs->value(QLatin1String("data"));
		if (s.iIndex >= -1)
			qlShortcuts << s;
	}
	g.qs->endArray();

	g.qs->beginReadArray(QLatin1String("messages"));
	for (QMap<int, quint32>::const_iterator it = qmMessages.constBegin(); it != qmMessages.constEnd(); ++it) {
		g.qs->setArrayIndex(it.key());
		SAVELOAD(qmMessages[it.key()], "log");
	}
	g.qs->endArray();

	g.qs->beginReadArray(QLatin1String("messagesounds"));
	for (QMap<int, QString>::const_iterator it = qmMessageSounds.constBegin(); it != qmMessageSounds.constEnd(); ++it) {
		g.qs->setArrayIndex(it.key());
		SAVELOAD(qmMessageSounds[it.key()], "logsound");
	}
	g.qs->endArray();

	g.qs->beginGroup(QLatin1String("lcd/devices"));
	foreach(const QString &d, g.qs->childKeys()) {
		qmLCDDevices.insert(d, g.qs->value(d, true).toBool());
	}
	g.qs->endGroup();

	g.qs->beginGroup(QLatin1String("audio/plugins"));
	foreach(const QString &d, g.qs->childKeys()) {
		qmPositionalAudioPlugins.insert(d, g.qs->value(d, true).toBool());
	}
	g.qs->endGroup();
}

#undef SAVELOAD
#define SAVELOAD(var,name) if (var != def.var) g.qs->setValue(QLatin1String(name), var); else g.qs->remove(QLatin1String(name))

void Settings::save() {
	Settings def;

	SAVELOAD(bMute, "audio/mute");
	SAVELOAD(bDeaf, "audio/deaf");
	SAVELOAD(atTransmit, "audio/transmit");
	SAVELOAD(uiDoublePush, "audio/doublepush");
	SAVELOAD(bPushClick, "audio/pushclick");
	SAVELOAD(qsPushClickOn, "audio/pushclickon");
	SAVELOAD(qsPushClickOff, "audio/pushclickoff");
	SAVELOAD(iQuality, "audio/quality");
	SAVELOAD(iMinLoudness, "audio/loudness");
	SAVELOAD(fVolume, "audio/volume");
	SAVELOAD(fOtherVolume, "audio/othervolume");
	SAVELOAD(bAttenuateOthers, "audio/attenuateothers");
	SAVELOAD(bAttenuateOthersOnTalk, "audio/attenuateothersontalk");
	SAVELOAD(vsVAD, "audio/vadsource");
	SAVELOAD(fVADmin, "audio/vadmin");
	SAVELOAD(fVADmax, "audio/vadmax");
	SAVELOAD(iNoiseSuppress, "audio/noisesupress");
	SAVELOAD(iVoiceHold, "audio/voicehold");
	SAVELOAD(iOutputDelay, "audio/outputdelay");
	SAVELOAD(iIdleTime, "audio/idletime");
	SAVELOAD(fAudioMinDistance, "audio/mindistance");
	SAVELOAD(fAudioMaxDistance, "audio/maxdistance");
	SAVELOAD(fAudioMaxDistVolume, "audio/maxdistancevolume");
	SAVELOAD(fAudioBloom, "audio/bloom");
	SAVELOAD(bEcho, "audio/echo");
	SAVELOAD(bEchoMulti, "audio/echomulti");
	SAVELOAD(bPositionalAudio, "audio/positional");
	SAVELOAD(bPositionalHeadphone, "audio/headphone");
	SAVELOAD(qsAudioInput, "audio/input");
	SAVELOAD(qsAudioOutput, "audio/output");
	SAVELOAD(bWhisperFriends, "audio/whisperfriends");

	SAVELOAD(iJitterBufferSize, "net/jitterbuffer");
	SAVELOAD(iFramesPerPacket, "net/framesperpacket");

	SAVELOAD(qsASIOclass, "asio/class");
	SAVELOAD(qlASIOmic, "asio/mic");
	SAVELOAD(qlASIOspeaker, "asio/speaker");

	SAVELOAD(qsWASAPIInput, "wasapi/input");
	SAVELOAD(qsWASAPIOutput, "wasapi/output");

	SAVELOAD(qsALSAInput, "alsa/input");
	SAVELOAD(qsALSAOutput, "alsa/output");

	SAVELOAD(qsPulseAudioInput, "pulseaudio/input");
	SAVELOAD(qsPulseAudioOutput, "pulseaudio/output");

	SAVELOAD(qsOSSInput, "oss/input");
	SAVELOAD(qsOSSOutput, "oss/output");

	SAVELOAD(qsCoreAudioInput, "coreaudio/input");
	SAVELOAD(qsCoreAudioOutput, "coreaudio/output");

	SAVELOAD(iPortAudioInput, "portaudio/input");
	SAVELOAD(iPortAudioOutput, "portaudio/output");

	SAVELOAD(qbaDXInput, "directsound/input");
	SAVELOAD(qbaDXOutput, "directsound/output");

	SAVELOAD(bTTS, "tts/enable");
	SAVELOAD(iTTSVolume, "tts/volume");
	SAVELOAD(iTTSThreshold, "tts/threshold");

	SAVELOAD(bOverlayEnable, "overlay/enable");
	SAVELOAD(osOverlay, "overlay/show");
	SAVELOAD(bOverlayUserTextures, "overlay/usertextures");
	SAVELOAD(bOverlayAlwaysSelf, "overlay/alwaysself");
	SAVELOAD(fOverlayX, "overlay/x");
	SAVELOAD(fOverlayY, "overlay/y");
	SAVELOAD(bTransmitPosition, "audio/postransmit");
	SAVELOAD(bOverlayTop, "overlay/top");
	SAVELOAD(bOverlayBottom, "overlay/bottom");
	SAVELOAD(bOverlayLeft, "overlay/left");
	SAVELOAD(bOverlayRight, "overlay/right");
	SAVELOAD(qfOverlayFont, "overlay/font");
	SAVELOAD(fOverlayHeight, "overlay/height");
	SAVELOAD(qcOverlayUser, "overlay/user");
	SAVELOAD(qcOverlayTalking, "overlay/talking");
	SAVELOAD(qcOverlayChannel, "overlay/channel");
	SAVELOAD(qcOverlayChannelTalking, "overlay/channeltalking");

	// Network settings
	SAVELOAD(bTCPCompat, "net/tcponly");
	SAVELOAD(bQoS, "net/qos");
	SAVELOAD(bReconnect, "net/reconnect");
	SAVELOAD(ptProxyType, "net/proxytype");
	SAVELOAD(qsProxyHost, "net/proxyhost");
	SAVELOAD(usProxyPort, "net/proxyport");
	SAVELOAD(qsProxyUsername, "net/proxyusername");
	SAVELOAD(qsProxyPassword, "net/proxypassword");
	SAVELOAD(iMaxImageSize, "net/maximagesize");
	SAVELOAD(iMaxImageWidth, "net/maximagewidth");
	SAVELOAD(iMaxImageHeight, "net/maximageheight");

	SAVELOAD(bExpert, "ui/expert");
	SAVELOAD(qsLanguage, "ui/language");
	SAVELOAD(qsStyle, "ui/style");
	SAVELOAD(qsSkin, "ui/skin");
	SAVELOAD(ceExpand, "ui/expand");
	SAVELOAD(ceChannelDrag, "ui/drag");
	SAVELOAD(aotbAlwaysOnTop, "ui/alwaysontop");
	SAVELOAD(bAskOnQuit, "ui/askonquit");
	SAVELOAD(bShowChatbar, "ui/showchatbar");
	SAVELOAD(bMinimalView, "ui/minimalview");
	SAVELOAD(bHideFrame, "ui/hideframe");
	SAVELOAD(bUserTop, "ui/usertop");
	SAVELOAD(bFirstTime, "ui/firsttime120");
	SAVELOAD(qbaMainWindowGeometry, "ui/geometry");
	SAVELOAD(qbaMainWindowState, "ui/state");
	SAVELOAD(qbaMinimalViewGeometry, "ui/minimalviewgeometry");
	SAVELOAD(qbaConfigGeometry, "ui/ConfigGeometry");
	SAVELOAD(wlWindowLayout, "ui/WindowLayout");
	SAVELOAD(qbaSplitterState, "ui/splitter");
	SAVELOAD(qbaHeaderState, "ui/header");
	SAVELOAD(qsUsername, "ui/username");
	SAVELOAD(qsLastServer, "ui/server");
	SAVELOAD(ssFilter, "ui/serverfilter");
	SAVELOAD(bUpdateCheck, "ui/updatecheck");
	SAVELOAD(bPluginCheck, "ui/plugincheck");
	SAVELOAD(bHideTray, "ui/hidetray");
	SAVELOAD(bStateInTray, "ui/stateintray");
	SAVELOAD(bUsage, "ui/usage");
	SAVELOAD(bShowUserCount, "ui/showusercount");
	SAVELOAD(qsImagePath, "ui/imagepath");

	SAVELOAD(iLCDUserViewMinColWidth, "lcd/userview/mincolwidth");
	SAVELOAD(iLCDUserViewSplitterWidth, "lcd/userview/splitterwidth");

	WDEBUG1 ("VGDEBUG Settings: before certificate check");
	QByteArray qba = CertWizard::exportCert(kpCertificate);
	WDEBUG1 ("VGDEBUG Settings: after certificate check");

	g.qs->setValue(QLatin1String("net/certificate"), qba);

	g.qs->beginWriteArray(QLatin1String("shortcuts"));
	int idx = 0;
	foreach(const Shortcut &s, qlShortcuts) {
		if (! s.isServerSpecific()) {
			g.qs->setArrayIndex(idx++);
			g.qs->setValue(QLatin1String("index"), s.iIndex);
			g.qs->setValue(QLatin1String("keys"), s.qlButtons);
			g.qs->setValue(QLatin1String("suppress"), s.bSuppress);
			g.qs->setValue(QLatin1String("data"), s.qvData);
		}
	}
	g.qs->endArray();

	WDEBUG1("VGDEBUG Settings: before messages");

	g.qs->beginWriteArray(QLatin1String("messages"));
	for (QMap<int, quint32>::const_iterator it = qmMessages.constBegin(); it != qmMessages.constEnd(); ++it) {
		g.qs->setArrayIndex(it.key());
		SAVELOAD(qmMessages[it.key()], "log");
	}
	g.qs->endArray();

	WDEBUG1("VDEBUG Settings: before messagesounds");

	g.qs->beginWriteArray(QLatin1String("messagesounds"));
	for (QMap<int, QString>::const_iterator it = qmMessageSounds.constBegin(); it != qmMessageSounds.constEnd(); ++it) {
		g.qs->setArrayIndex(it.key());
		SAVELOAD(qmMessageSounds[it.key()], "logsound");
	}
	g.qs->endArray();

	WDEBUG1("VDEBUG Settings: before devices");

	g.qs->beginGroup(QLatin1String("lcd/devices"));
	foreach(const QString &d, qmLCDDevices.keys()) {
		bool v = qmLCDDevices.value(d);
		if (!v)
			g.qs->setValue(d, v);
		else
			g.qs->remove(d);
	}
	g.qs->endGroup();

	WDEBUG1("VDEBUG Settings: before audio");

	g.qs->beginGroup(QLatin1String("audio/plugins"));
	foreach(const QString &d, qmPositionalAudioPlugins.keys()) {
		bool v = qmPositionalAudioPlugins.value(d);
		if (!v)
			g.qs->setValue(d, v);
		else
			g.qs->remove(d);
	}
	g.qs->endGroup();
}
