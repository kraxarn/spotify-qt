#pragma once

#include "dialog/base.hpp"

namespace Dialog
{
	class About: public Base
	{
	Q_OBJECT

	public:
		explicit About(QWidget *parent);
	};
}
