#pragma once

#include "view/log/base.hpp"

namespace Log
{
	class Spotify: public Base
	{
	public:
		Spotify(QWidget *parent);

	protected:
		auto getMessages() -> const std::vector<lib::log_message> & override;
	};
}
