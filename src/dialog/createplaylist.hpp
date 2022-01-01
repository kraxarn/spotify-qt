#pragma once

#include "dialog/base.hpp"

#include <QLineEdit>

namespace Dialog
{
	class CreatePlaylist: public Base
	{
	Q_OBJECT

	public:
		explicit CreatePlaylist(QWidget *parent);

	protected:
		void showEvent(QShowEvent *event) override;

	private:
		QLineEdit *playlistName;
	};
}
