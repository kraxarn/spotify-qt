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
		CreatePlaylist(std::string trackId, lib::spt::api &spotify, QWidget *parent);

	protected:
		void showEvent(QShowEvent *event) override;
		void onOk(bool checked) override;

	private:
		std::string trackId;
		QLineEdit *playlistNameEdit;
		QString defaultName;

		lib::spt::api &spotify;

		auto getPlaylistName() const -> QString;
		void showError(const QString &message);
	};
}
