#include "clienthandler.hpp"

using namespace lib::spt;

client_handler::client_handler(const lib::Settings &settings)
	: settings(settings)
{
}

std::string client_handler::start()
{
	// Don't start if already running
	if (is_running())
		return std::string();

	// Check if empty
	auto clientType = client_type();
	if (clientType == lib::ClientType::None)
		return "path is empty or invalid";

	// Check if path exists
	if (!file::exists(path()))
		return "file in path does not exist";

	// If using global config, just start
	if (settings.spotify.globalConfig
		&& clientType == lib::ClientType::Spotifyd)
	{
		std::system(path().c_str());
		return std::string();
	}

	// Check if username exists
	auto username = settings.spotify.username;
	if (username.empty())
		return "no username provided";

	// Get password from keyring if set
	std::string password;
#ifdef USE_DBUS
	KWallet keyring(username);
	if (settings.spotify.keyringPassword && keyring.unlock())
		password = keyring.readPassword();
#endif

	// Ask for password
	if (password.empty())
	{
		// TODO: D-bus is currently not supported (nor is password entry)
//		password = QInputDialog::getText(parentWidget,
//			"Enter password",
//			QString("Enter password for Spotify user \"%1\":").arg(username),
//			QLineEdit::Password);
//
//		if (password.isEmpty())
		return "no password provided";

#ifdef USE_DBUS
		if (settings.spotify.keyringPassword)
			keyring.writePassword(password);
#endif
	}

	// Common arguments
	std::vector<std::string> arguments = {
		"--bitrate", std::to_string(settings.spotify.bitrate),
		"--username", username,
		"--password", password
	};

	// librespot specific
	if (clientType == lib::ClientType::Librespot)
	{
		vector::append(arguments, {
			"--name", "spotify-qt (librespot)",
			"--initial-volume", "100",
			"--autoplay",
			"--cache", fmt::format("{}/librespot", path::cache())
		});
	}
	else if (clientType == lib::ClientType::Spotifyd)
	{
		vector::append(arguments, {
			"--no-daemon",
			"--device-name", "spotify-qt (spotifyd)",
		});
	}

	auto backend = settings.spotify.backend;
	if (backend.empty() && supports_pulse())
	{
		backend = "pulseaudio";
	}

	vector::append(arguments, {
		"--backend", backend
	});

	std::system(fmt::format("{} {} > {}", path(),
		join_args(arguments), log_path()).c_str());
	return std::string();
}

std::string client_handler::exec(const std::vector<std::string> &arguments)
{
	// Check if it exists
	if (!file::exists(path()))
		return std::string();

	// Check if either client
	if (client_type() == lib::ClientType::None)
		return std::string();

	// Start process and output to file
	auto filePath = fmt::format("{}.tmp", log_path());
	std::system(fmt::format("{} {} > {}", path(),
		join_args(arguments), filePath).c_str());

	// Read from file
	std::ifstream file(filePath);
	auto data = file::read_all(file);
	return strings::trim(data);
}

std::vector<std::string> client_handler::available_backends()
{
	auto clientType = client_type();
	std::vector<std::string> items;

	if (clientType == lib::ClientType::Librespot)
	{
		auto result = exec({
			"--name", "",
			"--backend", "?"
		});

		for (auto &line : strings::split(result, '\n'))
		{
			if (!strings::starts_with(line, "-"))
				continue;
			auto item = line.substr(line.length() - 2);
			strings::remove(item, "(default)");
			items.push_back(strings::trim(item));
		}
	}
	else if (clientType == lib::ClientType::Spotifyd)
	{
		auto result = exec({
			"--help",
		});

		for (auto &line : strings::split(result, '\n'))
		{
			if (!strings::contains(line, "audio backend"))
				continue;

			auto item = strings::right(line, line.length() - line.find('[') - 1);
			strings::remove(item, "possible values: ");
			strings::remove(item, "]");
			strings::trim(item);
			for (auto &i : strings::split(item, ','))
				items.push_back(strings::trim(i));
			break;
		}
	}

	return items;
}

bool client_handler::supports_pulse()
{
	return strings::contains(client_type() == ClientType::Librespot
		? exec({
			"--name", "",
			"--backend", "?"
		})
		: exec({
			"--help"
		}), "pulseaudio");
}

std::string client_handler::version()
{
	return client_type() == lib::ClientType::Spotifyd
		? exec({
			"--version"
		})
		: client_type() == lib::ClientType::Librespot
			? "librespot"
			: std::string();
}

bool client_handler::is_running()
{
	if (path().empty() || !file::exists("/usr/bin/ps"))
		return false;

	throw std::logic_error("not implemented");

//	QProcess ps;
//	ps.start("/usr/bin/ps", {"aux"});
//	ps.waitForFinished();
//	auto out = ps.readAllStandardOutput();
//	return QString(out).contains(path);
}

std::string client_handler::join_args(const std::vector<std::string> &args)
{
	return strings::join(args, " ");
}

std::string client_handler::sink_info()
{
	if (!file::exists(("/usr/bin/pactl")))
		return std::string();

	throw std::logic_error("not implemented");

//	QProcess process;
//
//	// Find what sink to use
//	process.start("/usr/bin/pactl", {
//		"list", "sink-inputs"
//	});
//	process.waitForFinished();
//	auto sinks = QString(process.readAllStandardOutput()).split("Sink Input #");
//	for (auto &sink : sinks)
//		if (sink.contains("Spotify"))
//			return sink;
//
//	return QString();
}

float client_handler::get_volume()
{
	auto sink = sink_info();
	if (sink.empty())
		return 1.f;
	auto i = sink.find("Volume:");
	if (i < 0)
		return 1.f;

	int v;
	for (auto &p : strings::split(strings::right(sink, sink.length() - i), ' '))
	{
		if (!strings::ends_with(p, "%"))
			continue;
		if (!strings::try_to_int(strings::left(p, p.length() - 1), v))
			continue;
		return (float)v / 100.f;
	}

	return 1.f;
}

void client_handler::set_volume(float value)
{
	auto sink = sink_info();
	if (sink.empty())
		return;

	throw std::logic_error("not implemented");

	// Sink was found, get id
//	auto left = strings::left(sink, sink.find('\n'));
//	auto sinkId = left.right(left.length() - left.lastIndexOf('#') - 1);
//	QProcess process;
//	process.start("/usr/bin/pactl", {
//		"set-sink-input-volume", sinkId, QString::number(value, 'f', 2)
//	});
//
//	process.waitForFinished();
}

std::vector<std::pair<lib::date, std::string>> log()
{
	return std::vector<std::pair<lib::date, std::string>>();
}

lib::ClientType client_handler::client_type()
{
	auto name = strings::right(path(), path().rfind('/'));
	name = strings::left(name, name.find('.'));
	strings::to_lower(name);

	return name == "spotifyd"
		? lib::ClientType::Spotifyd
		: name == "librespot"
			? lib::ClientType::Librespot
			: lib::ClientType::None;
}

std::string client_handler::path()
{
	return settings.spotify.path;
}

std::string client_handler::log_path()
{
	return std::string();
}
