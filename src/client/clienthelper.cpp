#include "clienthelper.hpp"

auto spt::ClientHelper::clientExec(const QString &path, const QStringList &arguments) -> QString
{
	// Check if it exists
	QFileInfo file(path);
	if (!file.exists())
	{
		return QString();
	}

	// Check if either client
	if (getClientType(path) == lib::client_type::none)
	{
		return QString();
	}

	// Prepare process
	QProcess process;

	// Get version info
	process.start(file.absoluteFilePath(), arguments);
	process.waitForFinished();

	// Entire stdout is version
	return process.readAllStandardOutput().trimmed();
}

auto spt::ClientHelper::availableBackends(const QString &path) -> QStringList
{
	QStringList items;
	auto clientType = getClientType(path);

	if (clientType == lib::client_type::librespot)
	{
		auto result = clientExec(path, QStringList({
			"--name", "",
			"--backend", "?"
		}));

		for (auto &line : result.split('\n'))
		{
			if (!line.startsWith("-"))
			{
				continue;
			}
			items.append(line.right(line.length() - 2)
				.remove("(default)")
				.trimmed());
		}
	}
	else if (clientType == lib::client_type::spotifyd)
	{
		auto result = clientExec(path, QStringList({
			"--help",
		}));

		for (auto &line : result.split('\n'))
		{
			if (!line.contains("audio backend"))
			{
				continue;
			}

			items.append(line.right(line.length() - line.indexOf('[') - 1)
				.remove("possible values: ")
				.remove(']')
				.trimmed()
				.split(", "));
			break;
		}
	}

	return items;
}

auto spt::ClientHelper::getClientType(const QString &path) -> lib::client_type
{
	auto baseName = QFileInfo(path).baseName().toLower();

	return baseName == "spotifyd"
		? lib::client_type::spotifyd
		: baseName == "librespot"
			? lib::client_type::librespot
			: lib::client_type::none;
}

auto spt::ClientHelper::supportsPulse(const QString &path) -> bool
{
	return clientExec(path, getClientType(path) == lib::client_type::librespot
		? QStringList({
			"--name", "",
			"--backend", "?"
		}) : QStringList({
			"--help"
		}))
		.contains("pulseaudio");
}

auto spt::ClientHelper::version(const QString &path) -> QString
{
	auto clientType = getClientType(path);

	return clientType == lib::client_type::spotifyd
		? clientExec(path, {
			"--version"
		}) : clientType == lib::client_type::librespot
			? "librespot"
			: QString();
}

auto spt::ClientHelper::isRunning(const QString &path) -> bool
{
	if (path.isEmpty() || !QFile("/usr/bin/ps").exists())
	{
		return false;
	}

	QProcess ps;
	ps.start("/usr/bin/ps", {"aux"});
	ps.waitForFinished();
	auto out = ps.readAllStandardOutput();
	return QString(out).contains(path);
}

auto spt::ClientHelper::getSinkInfo() -> QString
{
	if (!QFileInfo::exists("/usr/bin/pactl"))
	{
		return QString();
	}
	QProcess process;

	// Find what sink to use
	process.start("/usr/bin/pactl", {
		"list", "sink-inputs"
	});
	process.waitForFinished();
	auto sinks = QString(process.readAllStandardOutput()).split("Sink Input #");
	for (auto &sink : sinks)
	{
		if (sink.contains("Spotify"))
		{
			return sink;
		}
	}

	return QString();
}

auto spt::ClientHelper::getVolume() -> float
{
	auto sink = getSinkInfo();
	if (sink.isEmpty())
	{
		return 1.F;
	}

	auto i = sink.indexOf("Volume:");
	if (i < 0)
	{
		return 1.F;
	}

	bool ok;
	for (auto &p : sink.right(sink.length() - i).split(' '))
	{
		if (!p.endsWith('%'))
		{
			continue;
		}
		auto v = p.left(p.length() - 1).toInt(&ok);
		if (!ok)
		{
			continue;
		}
		return v / 100.F;
	}

	return 1.F;
}

void spt::ClientHelper::setVolume(float value)
{
	auto sink = getSinkInfo();
	if (sink.isEmpty())
	{
		return;
	}

	// Sink was found, get id
	auto left = sink.left(sink.indexOf('\n'));
	auto sinkId = left.right(left.length() - left.lastIndexOf('#') - 1);
	QProcess process;
	process.start("/usr/bin/pactl", {
		"set-sink-input-volume", sinkId, QString::number(value, 'f', 2)
	});

	process.waitForFinished();
}