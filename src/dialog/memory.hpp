#pragma once

#include "dialog/base.hpp"

#include <QLineEdit>
#include <QCheckBox>

namespace Dialog
{
	class Memory: public Base
	{
	Q_OBJECT

	public:
		explicit Memory(QWidget *parent);

	private:
		static constexpr int minAllocate = 1;
		static constexpr int maxAllocate = 1000;

		QLabel *status;
		QLineEdit *allocateMb;

		QByteArray data;

		void updateStatus();

		void onAllocateClicked(bool checked);
	};
}
