#pragma once

#include "dialog/base.hpp"
#include "lib/spotify/api.hpp"

#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>

namespace Dialog
{
	class EditPlaylist: public Base
	{
	Q_OBJECT

	public:
		EditPlaylist(lib::spt::api &spotify, const lib::spt::playlist &playlist,
			int selectedIndex, QWidget *parent);

	private:
		QLineEdit *name;
		QTextEdit *description;
		QCheckBox *isPublic;
		QCheckBox *isCollaborative;

		lib::spt::api &spotify;
		const lib::spt::playlist &playlist;

	protected:
		void onOk(bool checked) override;
	};
}
