#include "spotifyclient/helper.hpp"

auto SpotifyClient::Helper::clientExec(const QString &path, const QStringList &arguments) -> QString
{
	// Check if it exists
	QFileInfo file(path);
	if (!file.exists())
	{
		return {};
	}

	// Check if either client
	if (clientType(path) == lib::client_type::none)
	{
		return {};
	}

	// Prepare process
	QProcess process;

	// Get version info
	process.start(file.absoluteFilePath(), arguments);
	process.waitForFinished();

	// Entire stdout is version
	return process.readAllStandardOutput().trimmed();
}

auto SpotifyClient::Helper::availableBackends(const QString &path) -> QStringList
{
	QStringList items;
	auto type = clientType(path);

	if (type == lib::client_type::librespot)
	{
		auto result = clientExec(path, QStringList({
			"--name", "",
			"--backend", "?"
		}));

		for (auto &line: result.split('\n'))
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
	else if (type == lib::client_type::spotifyd)
	{
		auto result = clientExec(path, QStringList({
			"--help",
		}));

		for (auto &line: result.split('\n'))
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

auto SpotifyClient::Helper::clientType(const QString &path) -> lib::client_type
{
	auto baseName = QFileInfo(path).baseName().toLower();

	if (baseName == "spotifyd")
	{
		return lib::client_type::spotifyd;
	}

	if (baseName == "librespot")
	{
		return lib::client_type::librespot;
	}

	return lib::client_type::none;
}

auto SpotifyClient::Helper::supportsPulse(const QString &path) -> bool
{
	auto type = clientType(path);
	QStringList args;

	if (type == lib::client_type::librespot)
	{
		args = QStringList{
			"--name", "",
			"--backend", "?"
		};
	}
	else if (type == lib::client_type::spotifyd)
	{
		args = QStringList{
			"--help"
		};
	}

	return clientExec(path, args)
		.contains("pulseaudio");
}

auto SpotifyClient::Helper::version(const QString &path) -> QString
{
	auto type = clientType(path);

	if (type == lib::client_type::spotifyd)
	{
		return clientExec(path, {
			"--version"
		});
	}

	if (type == lib::client_type::librespot)
	{
		// librespot doesn't provide version information
		return "librespot";
	}

	return {};
}

auto SpotifyClient::Helper::running(const QString &path) -> bool
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

auto SpotifyClient::Helper::sinkInfo() -> QString
{
	if (!QFileInfo::exists("/usr/bin/pactl"))
	{
		return {};
	}
	QProcess process;

	// Find what sink to use
	process.start("/usr/bin/pactl", {
		"list", "sink-inputs"
	});
	process.waitForFinished();
	auto sinks = QString(process.readAllStandardOutput()).split("Sink Input #");
	for (auto &sink: sinks)
	{
		if (sink.contains("Spotify"))
		{
			return sink;
		}
	}

	return {};
}

auto SpotifyClient::Helper::getVolume() -> float
{
	auto sink = sinkInfo();
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
	for (auto &p: sink.right(sink.length() - i).split(' '))
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
		return static_cast<float>(v) / 100.F;
	}

	return 1.F;
}

void SpotifyClient::Helper::setVolume(float value)
{
	auto sink = sinkInfo();
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
