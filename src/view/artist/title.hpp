#pragma once

#include "widget/loader.hpp"

#include <QLabel>

namespace View
{
	namespace Artist
	{
		class Title: public QLabel
		{
		Q_OBJECT

		public:
			explicit Title(QWidget *parent);

		private:
			static constexpr int pointSize = 24;
		};
	}
}
