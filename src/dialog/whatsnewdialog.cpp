#include "whatsnewdialog.hpp"

WhatsNewDialog::WhatsNewDialog(const QString &tag, lib::settings &settings, QWidget *parent)
	: QDialog(parent)
{
	auto window = dynamic_cast<MainWindow *>(parent);
	if (window == nullptr)
		return;
	auto json = window->getJson(
		QString("https://api.github.com/repos/kraxarn/spotify-qt/releases/tags/%1").arg(tag)).object();
	auto body = json["body"].toString();
	if (body.isEmpty())
		return;

	auto layout = new QVBoxLayout();
	auto title = new QLabel(QString("spotify-qt was updated to version %1").arg(tag));
	auto titleFont = title->font();
	titleFont.setPointSize(titleFont.pointSize() * 1.5);
	title->setFont(titleFont);
	layout->addWidget(title);
	auto text = new QTextEdit();
	text->setReadOnly(true);

	// Markdown formatting only supports Qt 5.14
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
	text->setMarkdown(body);
#else
	text->setText(body);
#endif

	layout->addWidget(text, 1);
	auto buttons = new QDialogButtonBox(this);
	QPushButton::connect(buttons->addButton("Don't show again", QDialogButtonBox::RejectRole),
		&QPushButton::clicked, [this, &settings](bool checked)
		{
			settings.general.show_changelog = false;
			settings.save();
			reject();
		});
	QPushButton::connect(buttons->addButton(QDialogButtonBox::Ok),
		&QPushButton::clicked, [=](bool checked)
		{
			accept();
		});

	layout->addWidget(buttons);
	setLayout(layout);
	resize(500, 400);
	setWindowTitle(QString("spotify-qt %1").arg(tag));
}
