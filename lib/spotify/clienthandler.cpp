#include "clienthandler.hpp"

using namespace lib::spt;

ClientHandler::ClientHandler(const lib::Settings &settings)
	: settings(settings)
{
}

std::string ClientHandler::start()
{
	// Don't start if already running
	if (isRunning())
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
	if (backend.empty() && supportsPulse())
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

std::string ClientHandler::exec(const std::vector<std::string> &arguments)
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

std::vector<std::string> ClientHandler::available_backends()
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
			items.push_back(line.substr(line.length() - 2)
				.remove("(default)")
				.trimmed());
		}
	}
	else if (clientType == lib::ClientType::Spotifyd)
	{
		auto result = clientExec(path, QStringList({
			"--help",
		}));

		for (auto &line : result.split('\n'))
		{
			if (!line.contains("audio backend"))
				continue;

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

bool ClientHandler::supportsPulse()
{
	return clientExec(path, clientType == lib::ClientType::Librespot
		? QStringList({
			"--name", "",
			"--backend", "?"
		}) : QStringList({
			"--help"
		}))
		.contains("pulseaudio");
}

QString ClientHandler::version(const QString &path)
{
	auto clientType = getClientType(path);

	return clientType == lib::ClientType::Spotifyd
		? clientExec(path, {
			"--version"
		}) : clientType == lib::ClientType::Librespot
			? "librespot"
			: QString();
}

bool ClientHandler::isRunning()
{
	if (path.isEmpty() || !QFile("/usr/bin/ps").exists())
		return false;

	QProcess ps;
	ps.start("/usr/bin/ps", {"aux"});
	ps.waitForFinished();
	auto out = ps.readAllStandardOutput();
	return QString(out).contains(path);
}

std::string ClientHandler::join_args(const std::vector<std::string> &args)
{
	return strings::join(args, " ");
}

QString ClientHandler::getSinkInfo()
{
	if (!QFileInfo::exists("/usr/bin/pactl"))
		return QString();
	QProcess process;

	// Find what sink to use
	process.start("/usr/bin/pactl", {
		"list", "sink-inputs"
	});
	process.waitForFinished();
	auto sinks = QString(process.readAllStandardOutput()).split("Sink Input #");
	for (auto &sink : sinks)
		if (sink.contains("Spotify"))
			return sink;

	return QString();
}

float ClientHandler::getVolume()
{
	auto sink = getSinkInfo();
	if (sink.isEmpty())
		return 1.f;
	auto i = sink.indexOf("Volume:");
	if (i < 0)
		return 1.f;

	bool ok;
	for (auto &p : sink.right(sink.length() - i).split(' '))
	{
		if (!p.endsWith('%'))
			continue;
		auto v = p.left(p.length() - 1).toInt(&ok);
		if (!ok)
			continue;
		return v / 100.f;
	}

	return 1.f;
}

void ClientHandler::setVolume(float value)
{
	auto sink = getSinkInfo();
	if (sink.isEmpty())
		return;

	// Sink was found, get id
	auto left = sink.left(sink.indexOf('\n'));
	auto sinkId = left.right(left.length() - left.lastIndexOf('#') - 1);
	QProcess process;
	process.start("/usr/bin/pactl", {
		"set-sink-input-volume", sinkId, QString::number(value, 'f', 2)
	});

	process.waitForFinished();
}

void ClientHandler::logOutput(const QByteArray &output) const
{
	for (auto &line : QString(output).split('\n'))
	{
		if (line.isEmpty())
			continue;
		log << QPair<QDateTime, QString>(QDateTime::currentDateTime(), line);
	}
}

void ClientHandler::readyRead() const
{
	logOutput(process->readAllStandardOutput());
}

void ClientHandler::readyError() const
{
	logOutput(process->readAllStandardError());
}

QList<QPair<QDateTime, QString>> ClientHandler::getLog()
{
	return log;
}

lib::ClientType ClientHandler::getClientType(const QString &path)
{
	auto baseName = QFileInfo(path).baseName().toLower();

	return baseName == "spotifyd"
		? lib::ClientType::Spotifyd
		: baseName == "librespot"
			? lib::ClientType::Librespot
			: lib::ClientType::None;
}
