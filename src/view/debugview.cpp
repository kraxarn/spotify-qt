#include "debugview.hpp"

DebugView::DebugView(const Settings &settings, QWidget *parent)
	: settings(settings), QWidget(parent)
{
	auto layout = new QVBoxLayout(this);
	layout->setAlignment(Qt::AlignTop);
	setLayout(layout);

	auto urlLayout = new QHBoxLayout();

	requestType = new QComboBox(this);
	requestType->addItems({
		"GET", "PUT", "POST", "DELETE"
	});
	urlLayout->addWidget(requestType);

	urlLayout->addWidget(new QLabel("https://api.spotify.com/v1/"));

	urlPath = new QLineEdit(this);
	urlLayout->addWidget(urlPath, 1);

	auto sendButton = new QPushButton("Send", this);
	QPushButton::connect(sendButton, &QPushButton::clicked, this, &DebugView::sendRequest);
	urlLayout->addWidget(sendButton);

	layout->addLayout(urlLayout);

	jsonRequest = new QTextEdit(this);
	jsonRequest->setFontFamily("monospace");

	jsonResponse = new QTextEdit(this);
	jsonResponse->setReadOnly(true);
	jsonResponse->setFontFamily("monospace");

	tabs = new QTabWidget();
	tabs->addTab(jsonRequest, "Request");
	tabs->addTab(jsonResponse, "Response");

	layout->addWidget(tabs, 1);
}

void DebugView::sendRequest(bool)
{
	// Similar to Spotify, but without refresh handling etc.

	QJsonParseError jsonParseError{};
	auto jsonBody = QJsonDocument::fromJson(jsonRequest->toPlainText().toUtf8(), &jsonParseError);
	if (jsonParseError.error != QJsonParseError::NoError)
	{
		QMessageBox::warning(this, "JSON parse error", jsonParseError.errorString());
		return;
	}

	if (networkManager == nullptr)
		networkManager = new QNetworkAccessManager(this);

	QNetworkRequest request(QString("https://api.spotify.com/v1/%1").arg(urlPath->text()));
	request.setRawHeader("Authorization",
		("Bearer " + settings.account.accessToken).toUtf8());

	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	auto reply = networkManager->sendCustomRequest(request, requestType->currentText().toUtf8(),
		jsonBody.toJson());

	while (!reply->isFinished())
		QCoreApplication::processEvents();
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
		: QString("Failed to parse JSON: %1").arg(jsonParseError.errorString()));
	tabs->setCurrentIndex(1);
}
