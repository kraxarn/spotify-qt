#pragma once

#include "view/log/base.hpp"

namespace Log
{
	class Application: public Base
	{
	public:
		Application(QWidget *parent);

	protected:
		auto getMessages() -> const std::vector<lib::log_message> & override;
	};
}
