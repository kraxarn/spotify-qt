#pragma once

#include "lib/settings.hpp"
#include "lib/enums.hpp"

#include "enum/dialogaction.hpp"
#include "util/icon.hpp"
#include "widget/dragarea.hpp"

#include <QDialog>
#include <QVBoxLayout>
#include <QToolBar>
#include <QLabel>

namespace Dialog
{
	class Base: public QDialog
	{
	protected:
		Base(lib::settings &settings, QWidget *parent);

		void addLayout(QLayout *layout);

		void addActions(DialogAction dialogAction);

		void setTitle(const QString &text);

		virtual void onOk(bool checked);
		virtual void onApply(bool checked);
		virtual void onCancel(bool checked);

	private:
		static constexpr int contentMargins = 4;

		lib::settings &settings;

		QToolBar *toolBar;
		QVBoxLayout *layout;
		QLabel *title;

		QAction *ok = nullptr;
		QAction *apply = nullptr;
		QAction *cancel = nullptr;
	};
}
