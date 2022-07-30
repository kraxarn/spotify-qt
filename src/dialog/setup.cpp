#include "dialog/setup.hpp"

Dialog::Setup::Setup(lib::settings &settings, QWidget *parent)
	: QDialog(parent),
	settings(settings)
{
	// Auth
	auth = new lib::qt::spt::auth(settings, this);

	// Main layout
	auto *mainLayout = new QVBoxLayout();

	// Welcome text
	auto *welcomeText = new QLabel(QString(
		"Welcome to %1!\n"
		"Before using the app, you need to setup your Spotify Web API keys.\n"
		"You can do this by opening the Spotify Dashboard, create a new app and \n"
		"set the redirect uri (not website) to http://localhost:8888.\n"
		"Then, enter your Client ID and secret below from the same application page:")
		.arg(APP_NAME), this);
	mainLayout->addWidget(welcomeText);

	// Client ID
	clientId = new QLineEdit(QString::fromStdString(settings.account.client_id),
		this);
	clientId->setPlaceholderText("Client ID");
	mainLayout->addWidget(clientId);

	// Client secret
	clientSecret = new QLineEdit(QString::fromStdString(settings.account.client_secret),
		this);
	clientSecret->setPlaceholderText("Client secret");
	mainLayout->addWidget(clientSecret);

	// Add buttons
	auto *cancelButton = new QPushButton("Cancel");
	QAbstractButton::connect(cancelButton, &QAbstractButton::clicked,
		this, &Dialog::Setup::reject);

	auto *dashboardButton = new QPushButton("Spotify Dashboard");
	QAbstractButton::connect(dashboardButton, &QAbstractButton::clicked,
		this, &Dialog::Setup::onOpenDashboard);

	auto *authButton = new QPushButton("Authenticate");
	server = nullptr;
	QAbstractButton::connect(authButton, &QAbstractButton::clicked,
		this, &Dialog::Setup::onAuthenticate);

	auto *buttonBox = new QHBoxLayout();
	buttonBox->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
	buttonBox->addWidget(cancelButton);
	buttonBox->addWidget(dashboardButton);
	buttonBox->addWidget(authButton);
	mainLayout->addLayout(buttonBox);
	setLayout(mainLayout);
}

void Dialog::Setup::onOpenDashboard(bool /*checked*/)
{
	QString url("https://developer.spotify.com/dashboard/applications");
	Url::open(url, LinkType::Web, this);
}

void Dialog::Setup::onAuthenticate(bool /*checked*/)
{
	clientIdText = clientId->text().trimmed();
	clientSecretText = clientSecret->text().trimmed();
	clientId->setDisabled(true);
	clientSecret->setDisabled(true);
	redirect = QString("http://localhost:8888");

	if (server == nullptr)
	{
		constexpr int serverPort = 8888;
		server = new QTcpServer(this);
		if (!server->listen(QHostAddress::LocalHost, serverPort))
		{
			QMessageBox::warning(this,
				"server error",
				QString("failed to start a temporary server on port 8888: %1")
					.arg(server->errorString()));
			return;
		}
		QTcpServer::connect(server, &QTcpServer::newConnection,
			this, &Dialog::Setup::onNewConnection);
	}
	auto url = lib::qt::spt::auth::url(clientIdText, redirect);
	Url::open(url, LinkType::Web, this);
}

void Dialog::Setup::onNewConnection()
{
	// Read
	auto *socket = server->nextPendingConnection();
	socket->waitForReadyRead();
	auto response = QString(socket->readAll());

	// Client might want to request favicon or something
	if (!response.contains("?code="))
	{
		socket->close();
		return;
	}

	// Do magic with code received
	// GET /?code=<code> HTTP...
	auto left = response.left(response.indexOf(" HTTP"));
	auto code = left.right(left.length() - left.indexOf("?code=") - 6);
	auto status = auth->get(code, redirect, clientIdText, clientSecretText);

	// Write
	socket->write(QString("HTTP/1.1 200 OK\r\n\r\n%1")
		.arg(status.isEmpty()
			? QString("success, you can now return to %1").arg(APP_NAME)
			: QString("failed to authenticate: %1").arg(status)).toUtf8());
	socket->flush();
	socket->waitForBytesWritten(3000);
	socket->close();

	// Close it all down if ok
	if (status.isEmpty())
	{
		settings.account.client_id = clientIdText.toStdString();
		settings.account.client_secret = clientSecretText.toStdString();
		settings.save();

		server->close();
		server->deleteLater();

		accept();
	}
	else
	{
		// Otherwise, re-enable fields
		clientId->setDisabled(false);
		clientSecret->setDisabled(false);
	}
}
