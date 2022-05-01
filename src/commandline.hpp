#pragma once

#include <QCommandLineParser>

class CommandLine: public QCommandLineParser
{
public:
	explicit CommandLine(const QCoreApplication &app);

private:
	static auto options() -> QList<QCommandLineOption>;
};
