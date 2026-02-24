#pragma once

#include "dialog/base.hpp"

#include <QListWidget>

namespace Dialog
{
	class Icons final : public Base
	{
		Q_OBJECT

	public:
		explicit Icons(QWidget *parent);

	protected:
		void showEvent(QShowEvent *event) override;

	protected:
		QListWidget *list;
	};
}
