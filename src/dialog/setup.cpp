#include "dialog/setup.hpp"
#include "lib/log.hpp"
#include "util/url.hpp"

#include <QMessageBox>
#include <QPushButton>

Dialog::Setup::Setup(lib::settings &settings, QWidget *parent)
	: Base(parent),
	settings(settings)
{
	auto *layout = Base::layout<QVBoxLayout>();

	// Welcome text
	auto *welcomeText = new QLabel(QString(
		"Welcome to %1!\n"
		"Before using the app, you need to setup your Spotify Web API keys.\n"
		"You can do this by opening the Spotify Dashboard, create a new app and \n"
		"set the redirect uri (not website) to http://localhost:8888.\n"
		"Then, enter your Client ID and secret below from the same application page:")
		.arg(APP_NAME), this);
	layout->addWidget(welcomeText);

	// Client ID
	clientId = new QLineEdit(QString::fromStdString(settings.account.client_id), this);
	clientId->setPlaceholderText(QStringLiteral("Client ID"));
	layout->addWidget(clientId);

	// Client secret
	clientSecret = new QLineEdit(QString::fromStdString(settings.account.client_secret), this);
	clientSecret->setPlaceholderText(QStringLiteral("Client secret"));
	layout->addWidget(clientSecret);

	Base::addAction(DialogAction::Cancel);

	auto *dashboardButton = addButton(QStringLiteral("Spotify Dashboard"),
		QDialogButtonBox::ActionRole);

	QAbstractButton::connect(dashboardButton, &QAbstractButton::clicked,
		this, &Dialog::Setup::onOpenDashboard);

	auto *authButton = addButton(QStringLiteral("Authenticate"),
		QDialogButtonBox::AcceptRole);

	QAbstractButton::connect(authButton, &QAbstractButton::clicked,
		this, &Dialog::Setup::onAuthenticate);
}

void Dialog::Setup::onOpenDashboard(bool /*checked*/)
{
	const auto url = QStringLiteral("https://developer.spotify.com/dashboard/applications");
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
				QStringLiteral("Failed to start a temporary server on port 8888: %1")
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

	auth->openUrl(this);
}
