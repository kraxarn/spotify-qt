#pragma once

#include "lib/spotify/api.hpp"

#include "util/icon.hpp"
#include "util/url.hpp"
#include "widget/statusmessage.hpp"

#include <QMenu>
#include <QClipboard>

namespace Menu
{
	class Show: public QMenu
	{
	Q_OBJECT

	public:
		Show(lib::spt::api &spotify, const lib::spt::show &show, QWidget *parent);

	private:
		lib::spt::show show;
		lib::spt::api &spotify;

		auto showUrl() const -> QString;
		auto shareMenu() -> QMenu *;

		void onCopyLink(bool checked) const;
		void onOpenInSpotify(bool checked) const;
	};
}
