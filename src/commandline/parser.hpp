#pragma once

#include <QCommandLineParser>

namespace CommandLine
{
	class Parser: public QCommandLineParser
	{
	public:
		explicit Parser(const QCoreApplication &app);

	private:
		static auto options() -> QList<QCommandLineOption>;
	};
}
