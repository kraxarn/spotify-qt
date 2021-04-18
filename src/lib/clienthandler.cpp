#include "../mainwindow.hpp"
#include "clienthandler.hpp"

using namespace spt;

QList<QPair<QDateTime, QString>> ClientHandler::log;

ClientHandler::ClientHandler(const lib::settings &settings, QWidget *parent)
	: settings(settings), parentWidget(parent), QObject(parent)
{
	path = QString::fromStdString(settings.spotify.path);
	process = new QProcess(parent);
	clientType = getClientType(path);
}

ClientHandler::~ClientHandler()
{
	if (process != nullptr)
		process->close();
}

QString ClientHandler::start()
{
	// Don't start if already running
	if (!settings.spotify.always_start && isRunning())
		return QString();

	// Check if empty
	if (clientType == ClientType::None)
		return "path is empty or invalid";

	// Check if path exists
	QFileInfo info(path);
	if (!info.exists())
		return "file in path does not exist";

	// If using global config, just start
	if (settings.spotify.global_config && clientType == ClientType::Spotifyd)
	{
		process->start(path, QStringList());
		return QString();
	}

	// Check if username exists
	auto username = QString::fromStdString(settings.spotify.username);
	if (username.isEmpty())
		return "no username provided";

	// Get password from keyring if set
	QString password;
#ifdef USE_DBUS
	KWallet keyring(username);
	if (settings.spotify.keyring_password && keyring.unlock())
		password = keyring.readPassword();
#endif

	// Ask for password
	if (password.isEmpty())
	{
		password = QInputDialog::getText(parentWidget,
			"Enter password",
			QString("Enter password for Spotify user \"%1\":").arg(username),
			QLineEdit::Password);

		if (password.isEmpty())
			return "no password provided";

#ifdef USE_DBUS
		if (settings.spotify.keyring_password)
			keyring.writePassword(password);
#endif
	}

	// Common arguments
	QStringList arguments({
		"--bitrate", QString::number(settings.spotify.bitrate),
		"--username", username,
		"--password", password
	});

	// librespot specific
	if (clientType == ClientType::Librespot)
	{
		arguments.append({
			"--name", "spotify-qt (librespot)",
			"--initial-volume", "100",
			"--autoplay",
			"--cache", QString("%1/librespot")
				.arg(((MainWindow *) parentWidget)->getCacheLocation())
		});
	}
	else if (clientType == ClientType::Spotifyd)
	{
		arguments.append({
			"--no-daemon",
			"--device-name", "spotify-qt (spotifyd)",
		});
	}

	auto backend = QString::fromStdString(settings.spotify.backend);
	if (backend.isEmpty() && supportsPulse())
	{
		backend = "pulseaudio";
	}
	arguments.append({
		"--backend", backend
	});

	if (clientType == ClientType::Librespot && settings.spotify.disable_discovery)
	{
		arguments.append("--disable-discovery");
	}

	QProcess::connect(process, &QProcess::readyReadStandardOutput, this, &ClientHandler::readyRead);
	QProcess::connect(process, &QProcess::readyReadStandardError, this, &ClientHandler::readyError);

	lib::log::dev("starting: {} {}", path.toStdString(),
		arguments.join(' ').toStdString());

	process->start(path, arguments);
	return QString();
}

QString ClientHandler::clientExec(const QString &path, const QStringList &arguments)
{
	// Check if it exists
	QFileInfo file(path);
	if (!file.exists())
		return QString();

	// Check if either client
	if (getClientType(path) == ClientType::None)
		return QString();

	// Prepare process
	QProcess process;

	// Get version info
	process.start(file.absoluteFilePath(), arguments);
	process.waitForFinished();

	// Entire stdout is version
	return process.readAllStandardOutput().trimmed();
}

QStringList ClientHandler::availableBackends()
{
	QStringList items;

	if (clientType == ClientType::Librespot)
	{
		auto result = clientExec(path, QStringList({
			"--name", "",
			"--backend", "?"
		}));

		for (auto &line : result.split('\n'))
		{
			if (!line.startsWith("-"))
				continue;
			items.append(line.right(line.length() - 2)
				.remove("(default)")
				.trimmed());
		}
	}
	else if (clientType == ClientType::Spotifyd)
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
	return clientExec(path, clientType == ClientType::Librespot
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

	return clientType == ClientType::Spotifyd
		? clientExec(path, {
			"--version"
		}) : clientType == ClientType::Librespot
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

ClientType ClientHandler::getClientType(const QString &path)
{
	auto baseName = QFileInfo(path).baseName().toLower();

	return baseName == "spotifyd"
		? ClientType::Spotifyd
		: baseName == "librespot"
			? ClientType::Librespot
			: ClientType::None;
}
