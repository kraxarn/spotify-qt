#pragma once

#include <QString>

namespace spt
{
	class WebPlayer
	{
	public:
		explicit WebPlayer(QString token);
		QString html();
	private:
		QString token;
	};
}