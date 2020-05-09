#include "clienthandler.hpp"

using namespace spt;

ClientHandler::ClientHandler(QWidget *parent) : parent(parent)
{
	path = Settings().sptPath();
	process = new QProcess();
}

ClientHandler::~ClientHandler()
{
	if (process == nullptr)
		return;
	process->close();
	delete process;
	process = nullptr;
}

QString ClientHandler::start()
{
	// Don't start if already running
	if (isRunning())
		return QString();
	Settings settings;
	// Check if empty
	if (path.isEmpty())
		return "path is empty";
	// Check if path exists
	QFileInfo info(path);
	if (!info.exists())
		return "file in path does not exist";
	// If using global config, just start
	if (settings.sptGlobalConfig())
	{
		process->start(path);
		return QString();
	}
	// Check if username exists
	auto username = settings.sptUser();
	if (username.isEmpty())
		return "no username provided";
	// Ask for password
	auto password = QInputDialog::getText(parent,
		"Enter password",
		QString("Enter password for Spotify user \"%1\":").arg(username),
		QLineEdit::Password);
	if (password.isEmpty())
		return "no password provided";
	// Attempt to start spotifyd
	QStringList arguments({
		"--bitrate",		QString::number(settings.sptBitrate()),
		"--device-name",	"spotify-qt",
		"--username",		username,
		"--password",		password
	});
	if (supportsPulse())
		arguments.append({
			"--backend", "pulseaudio"
		});
	else
		qDebug() << "warning: spotifyd was compiled without pulseaudio support";
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
	if (file.baseName() != "spotifyd")
		return QString();
	// Prepare process
	QProcess process;
	// Get version info
	process.start(file.absoluteFilePath(), arguments);
	process.waitForFinished();
	// Entire stdout is version
	return process.readAllStandardOutput().trimmed();
}

bool ClientHandler::supportsPulse()
{
	return clientExec(path, {
		"--help"
	}).contains("pulseaudio");
}

QString ClientHandler::version(const QString &path)
{
	return clientExec(path, {
		"--version"
	});
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
