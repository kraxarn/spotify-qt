#pragma once

#include "dialog/base.hpp"
#include "lib/spotify/api.hpp"

#include <QLineEdit>

namespace Dialog
{
	class CreatePlaylist: public Base
	{
	Q_OBJECT

	public:
		CreatePlaylist(std::vector<std::string> trackIds,
			lib::spt::api &spotify, QWidget *parent);

	protected:
		void showEvent(QShowEvent *event) override;
		void onOk(bool checked) override;

	private:
		std::vector<std::string> trackIds;
		QLineEdit *playlistNameEdit;
		QString defaultName;

		lib::spt::api &spotify;

		auto getPlaylistName() const -> QString;
		auto getTrackUris() const -> std::vector<std::string>;
		void showError(const QString &message);
	};
}
