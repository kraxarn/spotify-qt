#pragma once

#ifdef USE_DBUS

#include <QDBusInterface>
#include <QDBusReply>

namespace MediaPlayer
{
	class Client
	{
	public:
		Client();

		auto playPause() -> bool;
		auto previousTrack() -> bool;
		auto nextTrack() -> bool;
		auto metadata() -> QString;

	private:
		QDBusInterface interface;

		auto callMethod(const QString &name) -> bool;
		static auto invalidMessage() -> QString;
	};
}

#endif
