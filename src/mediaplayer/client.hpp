#pragma once

#ifdef USE_DBUS

#include <QDBusInterface>

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
		auto openUri(const QString &trackUrl) -> bool;

	private:
		QDBusInterface interface;

		auto callMethod(const QString &name, const QVariant &arg = {}) -> bool;
		static auto invalidMessage() -> QString;
	};
}

#endif
