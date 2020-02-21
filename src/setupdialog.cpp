#include "setupdialog.hpp"

SetupDialog::SetupDialog(spt::Spotify *spt, QWidget *parent) : QDialog(parent)
{
	// Main layout
	auto mainLayout = new QVBoxLayout();
	// Welcome text
	auto welcomeText = new QLabel("Welcome to spotify-qt!\n"
		"Before using the app, you need to setup your Spotify Web API keys.\n"
		"You can do this by opening the Spotify Dashboard, create a new app and \n"
  		"set the redirect uri to stpqt://callback.\n"
		"Then, enter your Client ID and secret below from the same application page:",
		this);
	mainLayout->addWidget(welcomeText);
	// Client ID
	auto clientId = new QLineEdit(this);
	clientId->setPlaceholderText("Client ID");
	mainLayout->addWidget(clientId);
	// Client secret
	auto clientSecret = new QLineEdit(this);
	clientSecret->setPlaceholderText("Client secret");
	mainLayout->addWidget(clientSecret);
	// Add buttons
	auto cancelButton = new QPushButton("Cancel");
	QAbstractButton::connect(cancelButton, &QAbstractButton::clicked, [=](bool checked) {
		reject();
	});
	auto dashboardButton = new QPushButton("Spotify Dashboard");
	QAbstractButton::connect(dashboardButton, &QAbstractButton::clicked, [=](bool checked) {
		QDesktopServices::openUrl(QUrl("https://developer.spotify.com/dashboard/applications"));
	});
	auto authButton = new QPushButton("Authenticate");
	QAbstractButton::connect(authButton, &QAbstractButton::clicked, [=](bool checked) {
		auto clientIdText = clientId->text();
		auto clientSecretText = clientSecret->text();
		clientId->setDisabled(true);
		clientSecret->setDisabled(true);
		auto redirect = QString("http://localhost:8888");
		auto webview = new QWebEngineView(nullptr);
		QWebEngineView::connect(webview, &QWebEngineView::urlChanged, [=](const QUrl &url) {
			if (!url.toString().startsWith(redirect))
				return;
			qDebug() << url.query().remove(0, QString("code=").length());
			webview->close();
			webview->deleteLater();
			auto status = spt->auth(
				url.query().remove(0, QString("code=").length()),
				redirect, clientIdText, clientSecretText);
			if (status.isEmpty())
			{
				Settings settings;
				settings.setClientId(clientIdText);
				settings.setClientSecret(clientSecretText);
				accept();
			}
			else
			{
				QMessageBox::warning(this, "Auth error", status);
				clientId->setDisabled(false);
				clientSecret->setDisabled(false);
			}
		});
		webview->resize(1280, 720);
		webview->show();
		webview->load(QUrl(spt::Spotify::authUrl(clientIdText, redirect)));
	});
	auto buttonBox = new QHBoxLayout();
	buttonBox->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
	buttonBox->addWidget(cancelButton);
	buttonBox->addWidget(dashboardButton);
	buttonBox->addWidget(authButton);
	mainLayout->addLayout(buttonBox);
	setLayout(mainLayout);
}

int SetupDialog::exec()
{
	return QDialog::exec();
}