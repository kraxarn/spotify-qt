#pragma once

#include "dialog/base.hpp"

#include <QListWidget>

namespace Dialog
{
	class Disallows: public Base
	{
	Q_OBJECT

	public:
		explicit Disallows(QWidget *parent);

	protected:
		void showEvent(QShowEvent *event) override;
		void onOk(bool checked) override;

	private:
		static constexpr int actionRole = 0x100;

		QListWidget *list;
	};
}
