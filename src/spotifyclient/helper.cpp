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
