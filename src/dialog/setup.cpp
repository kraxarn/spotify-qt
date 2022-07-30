#include "dialog/setup.hpp"

Dialog::Setup::Setup(lib::settings &settings, QWidget *parent)
	: QDialog(parent),
	settings(settings)
{
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

	auto *authButton = new QPushButton(QStringLiteral("Authenticate"));
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
	const auto clientIdText = clientId->text().trimmed();
	const auto clientSecretText = clientSecret->text().trimmed();

	settings.account.client_id = clientIdText.toStdString();
	settings.account.client_secret = clientSecretText.toStdString();

	clientId->setDisabled(true);
	clientSecret->setDisabled(true);

	if (auth == nullptr)
	{
		auth = new spt::AuthServer(settings, this);
		if (!auth->listen())
		{
			QMessageBox::warning(this, QStringLiteral("Server error"),
				QString("Failed to start a temporary server on port 8888: %1")
					.arg(auth->errorString()));

			return;
		}

		spt::AuthServer::connect(auth, &spt::AuthServer::success,
			this, &Dialog::Setup::accept);

		spt::AuthServer::connect(auth, &spt::AuthServer::failed, [this](const QString &message)
		{
			lib::log::error("Authentication failed: {}", message.toStdString());
			clientId->setDisabled(false);
			clientSecret->setDisabled(false);
		});
	}

	auto url = lib::qt::spt::auth::url(clientIdText, spt::AuthServer::redirectUrl());
	Url::open(url, LinkType::Web, this);
}
