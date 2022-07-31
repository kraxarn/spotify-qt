#include "dialog/apirequest.hpp"

Dialog::ApiRequest::ApiRequest(const lib::settings &settings, QWidget *parent)
	: QDialog(parent),
	settings(settings)
{
	setWindowTitle(QStringLiteral("API request"));
	resize(width, height);

	auto *layout = new QVBoxLayout(this);
	layout->setAlignment(Qt::AlignTop);
	setLayout(layout);

	auto *urlLayout = new QHBoxLayout();

	urlType = new QComboBox(this);
	urlType->addItems({
		QStringLiteral("https://api.spotify.com/v1/"),
		QStringLiteral("https://spclient.wg.spotify.com/"),
	});
	urlLayout->addWidget(urlType);

	urlPath = new QLineEdit(this);
	urlLayout->addWidget(urlPath);

	layout->addLayout(urlLayout);

	auto *requestLayout = new QHBoxLayout();

	requestType = new QComboBox(this);
	requestType->addItems({
		QStringLiteral("GET"),
		QStringLiteral("PUT"),
		QStringLiteral("POST"),
		QStringLiteral("DELETE"),
	});
	requestLayout->addWidget(requestType, 1);

	auto *sendButton = new QPushButton(QStringLiteral("Send"), this);
	QPushButton::connect(sendButton, &QPushButton::clicked,
		this, &Dialog::ApiRequest::sendRequest);
	requestLayout->addWidget(sendButton);

	layout->addLayout(requestLayout);

	jsonRequest = new QTextEdit(this);
	jsonRequest->setFontFamily(QStringLiteral("monospace"));
	jsonRequest->setWordWrapMode(QTextOption::NoWrap);

	jsonResponse = new QTextEdit(this);
	jsonResponse->setReadOnly(true);
	jsonResponse->setFontFamily(QStringLiteral("monospace"));
	jsonResponse->setWordWrapMode(QTextOption::NoWrap);

	tabs = new QTabWidget();
	tabs->addTab(jsonRequest, QStringLiteral("Request"));
	tabs->addTab(jsonResponse, QStringLiteral("Response"));

	layout->addWidget(tabs, 1);
}

void Dialog::ApiRequest::sendRequest(bool /*checked*/)
{
	// Similar to Spotify, but without refresh handling etc.

	QJsonParseError jsonParseError{};
	QByteArray jsonData;

	if (!jsonRequest->toPlainText().isEmpty())
	{
		auto jsonBody = QJsonDocument::fromJson(jsonRequest->toPlainText().toUtf8(),
			&jsonParseError);
		if (jsonParseError.error != QJsonParseError::NoError)
		{
			QMessageBox::warning(this, "JSON parse error", jsonParseError.errorString());
			return;
		}
		jsonData = jsonBody.toJson();
	}

	if (networkManager == nullptr)
	{
		networkManager = new QNetworkAccessManager(this);
	}

	QNetworkRequest request(urlType->currentText().append(urlPath->text()));
	request.setRawHeader("Authorization",
		QString("Bearer %1")
			.arg(QString::fromStdString(settings.account.access_token)).toUtf8());

	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	auto *reply = networkManager->sendCustomRequest(request,
		requestType->currentText().toUtf8(), jsonData);

	while (!reply->isFinished())
	{
		QCoreApplication::processEvents();
	}
	auto replyBody = reply->readAll();
	reply->deleteLater();

	if (replyBody.isEmpty())
	{
		jsonResponse->setPlainText(QString());
		return;
	}

	auto json = QJsonDocument::fromJson(replyBody, &jsonParseError);

	jsonResponse->setPlainText(jsonParseError.error == QJsonParseError::NoError
		? json.toJson(QJsonDocument::JsonFormat::Indented)
		: QString("Failed to parse JSON: %1\n%2")
			.arg(jsonParseError.errorString(), QString(replyBody)));
	tabs->setCurrentIndex(1);
}
