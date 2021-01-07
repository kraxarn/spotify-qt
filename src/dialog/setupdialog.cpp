#include "setupdialog.hpp"

SetupDialog::SetupDialog(lib::settings &settings, QWidget *parent)
	: settings(settings), QDialog(parent)
{
	// Auth
	auth = new spt::Auth(settings);

	// Main layout
	auto mainLayout = new QVBoxLayout();

	// Welcome text
	auto welcomeText = new QLabel(
		"Welcome to spotify-qt!\n"
		"Before using the app, you need to setup your Spotify Web API keys.\n"
		"You can do this by opening the Spotify Dashboard, create a new app and \n"
		"set the redirect uri (not website) to http://localhost:8888.\n"
		"Then, enter your Client ID and secret below from the same application page:",
		this);
	mainLayout->addWidget(welcomeText);

	// Client ID
	clientId = new QLineEdit(QString::fromStdString
		(settings.account.client_id), this);
	clientId->setPlaceholderText("Client ID");
	mainLayout->addWidget(clientId);

	// Client secret
	clientSecret = new QLineEdit(QString::fromStdString
		(settings.account.client_secret), this);
	clientSecret->setPlaceholderText("Client secret");
	mainLayout->addWidget(clientSecret);

	// Add buttons
	auto cancelButton = new QPushButton("Cancel");
	QAbstractButton::connect(cancelButton, &QAbstractButton::clicked, [this](bool checked)
	{
		reject();
	});
	auto dashboardButton = new QPushButton("Spotify Dashboard");
	QAbstractButton::connect(dashboardButton, &QAbstractButton::clicked, [this](bool checked)
	{
		QString url("https://developer.spotify.com/dashboard/applications");
		Utils::openUrl(url, LinkType::Web, this);
	});

	auto authButton = new QPushButton("Authenticate");
	server = nullptr;
	QAbstractButton::connect(authButton, &QAbstractButton::clicked, [this](bool checked)
	{
		clientIdText = clientId->text().trimmed();
		clientSecretText = clientSecret->text().trimmed();
		clientId->setDisabled(true);
		clientSecret->setDisabled(true);
		redirect = QString("http://localhost:8888");

		if (server == nullptr)
		{
			server = new QTcpServer(this);
			if (!server->listen(QHostAddress::LocalHost, 8888))
			{
				QMessageBox::warning(this,
					"server error",
					QString("failed to start a temporary server on port 8888: %1")
						.arg(server->errorString()));
				return;
			}
			QTcpServer::connect(server, &QTcpServer::newConnection, [this]()
			{
				// Read
				auto socket = server->nextPendingConnection();
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
				auto status = auth->auth(code, redirect, clientIdText, clientSecretText);

				// Write
				socket->write(QString("HTTP/1.1 200 OK\r\n\r\n%1")
					.arg(status.isEmpty()
						? "success, you can now return to spotify-qt"
						: QString("failed to authenticate: %1").arg(status)).toUtf8());
				socket->flush();
				socket->waitForBytesWritten(3000);
				socket->close();

				// Close it all down if ok
				if (status.isEmpty())
				{
					this->settings.account.client_id = clientIdText
						.toStdString();
					this->settings.account.client_secret = clientSecretText
						.toStdString();
					this->settings.save();
					server->close();
					delete server;
					accept();
				}
				else
				{
					// Otherwise re-enable fields
					clientId->setDisabled(false);
					clientSecret->setDisabled(false);
				}
			});
		}
		auto url = spt::Auth::authUrl(clientIdText, redirect);
		Utils::openUrl(url, LinkType::Web, this);
	});

	auto buttonBox = new QHBoxLayout();
	buttonBox->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
	buttonBox->addWidget(cancelButton);
	buttonBox->addWidget(dashboardButton);
	buttonBox->addWidget(authButton);
	mainLayout->addLayout(buttonBox);
	setLayout(mainLayout);
}

SetupDialog::~SetupDialog()
{
	delete auth;
}