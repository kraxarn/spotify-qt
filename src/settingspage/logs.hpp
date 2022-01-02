#pragma once

#include "settingspage/base.hpp"

namespace SettingsPage
{
	class Logs: public Base
	{
	public:
		Logs(lib::settings &settings, QWidget *parent);

		auto icon() -> QIcon override;
		auto title() -> QString override;
		auto save() -> bool override;

	private:
		auto appLogs() -> QWidget *;
		auto spotifyLogs() -> QWidget *;
	};
}
