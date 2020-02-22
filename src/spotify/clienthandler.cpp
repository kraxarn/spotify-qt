#include "clienthandler.hpp"

using namespace spt;

ClientHandler::ClientHandler(QWidget *parent) : parent(parent)
{
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
	Settings settings;
	// Check if empty
	auto path = settings.sptPath();
	if (path.isEmpty())
		return "path is empty";
	// Check if path exists
	QFileInfo info(path);
	if (!info.exists())
		return "file in path does not exist";
	// Check if username exists
	auto username = settings.sptUser();
	if (username.isEmpty())
		return "no username provided";
	// Ask for password
	auto password = QInputDialog::getText(parent,
		"Enter password",
		QString("Enter password for Spotify user \"%1\":").arg(username));
	if (password.isEmpty())
		return "no password provided";
	// Get all the common stuff
	// (temp default values)
	auto deviceName = QString("spotify-qt");
	auto bitrate = 320;
	// Attempt to start client (for now assume librespot)
	process = new QProcess();
	process->start(path, {
		"--no-daemon",
		"--bitrate", QString::number(bitrate),
		"--device-name", "spotify-qt",
		"--username", username,
		"--password", password
	});
	return QString();
}