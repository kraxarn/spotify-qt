#include "whatsnewdialog.hpp"
#include "mainwindow.hpp"

WhatsNewDialog::WhatsNewDialog(lib::settings &settings,
	const lib::http_client &httpClient, QWidget *parent)
	: settings(settings),
	httpClient(httpClient),
	QDialog(parent)
{
	auto *layout = new QVBoxLayout();
	auto *title = new QLabel(QString("spotify-qt was updated to version %1")
		.arg(APP_VERSION));

	auto titleFont = title->font();
	constexpr float titleFontMulti = 1.5F;
	auto fontSize = static_cast<float>(titleFont.pointSize()) * titleFontMulti;
	titleFont.setPointSize(static_cast<int>(fontSize));

	title->setFont(titleFont);
	layout->addWidget(title);
	text = new QTextEdit();
	text->setReadOnly(true);
	text->setPlainText("...");

	layout->addWidget(text, 1);
	auto *buttons = new QDialogButtonBox(this);

	auto *dontShowAgain = buttons->addButton("Don't show again",
		QDialogButtonBox::RejectRole);
	QPushButton::connect(dontShowAgain, &QPushButton::clicked,
		this, &WhatsNewDialog::onDontShowAgain);

	auto *ok = buttons->addButton(QDialogButtonBox::Ok);
	QPushButton::connect(ok, &QPushButton::clicked,
		this, &WhatsNewDialog::onOk);

	layout->addWidget(buttons);
	setLayout(layout);
	resize(500, 400);
	setWindowTitle(QString("spotify-qt %1").arg(APP_VERSION));
}

void WhatsNewDialog::onDontShowAgain(bool /*checked*/)
{
	settings.general.show_changelog = false;
	settings.save();

	reject();
}

void WhatsNewDialog::onOk(bool /*checked*/)
{
	accept();
}

void WhatsNewDialog::open()
{
	auto url = lib::fmt::format("https://api.github.com/repos/kraxarn/spotify-qt/releases/tags/{}",
		APP_VERSION);

	httpClient.get(url, lib::headers(), [this](const std::string &body)
	{
		if (body.empty())
		{
			failed("No response from server");
			return;
		}

		try
		{
			auto json = nlohmann::json::parse(body);
			this->onReleaseInfo(json);
		}
		catch (const std::exception &e)
		{
			failed(e.what());
		}
	});
}

void WhatsNewDialog::onReleaseInfo(const nlohmann::json &json)
{
	lib::log::dev("release_info: {}", json);
	const auto &jsonBody = json.at("body");
	if (jsonBody.is_null() || !jsonBody.is_string())
	{
		failed("No release info");
		return;
	}
	auto body = QString::fromStdString(jsonBody.get<std::string>());

	// Markdown formatting only supports Qt 5.14
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
	text->setMarkdown(body);
#else
	text->setText(body);
#endif

	QDialog::open();
}

void WhatsNewDialog::failed(const std::string &reason)
{
	lib::log::error("Failed to fetch what's new in \"{}\": {}",
		APP_VERSION, reason);
}
