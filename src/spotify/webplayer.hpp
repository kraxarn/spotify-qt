#pragma once

#include <QString>

namespace spt
{
	class WebPlayer
	{
	public:
		explicit WebPlayer(const QString &token);
		QString html();

	private:
		const QString &token;
	};
}