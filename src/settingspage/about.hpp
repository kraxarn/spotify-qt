#pragma once

#include "util/icon.hpp"
#include "settingspage/base.hpp"
#include "list/contributors.hpp"

namespace SettingsPage
{
	class About: public Base
	{
	public:
		About(lib::settings &settings, const lib::http_client &httpClient, QWidget *parent);

		auto icon() -> QIcon override;
		auto title() -> QString override;
		auto save() -> bool override;

	private:
		auto about() -> QWidget *;
		auto contributors(const lib::http_client &httpClient) -> QWidget *;
	};
}
