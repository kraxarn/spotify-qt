#include "dialog/lyricssearch.hpp"
#include "view/sidepanel/view.hpp"

#include <QGroupBox>
#include <QMessageBox>
#include <QPushButton>

Dialog::LyricsSearch::LyricsSearch(const lib::http_client &httpClient, QWidget *parent)
	: Base(parent),
	lyrics(httpClient)
{
	lyrics.set_app_info(APP_NAME, APP_VERSION,
		lib::fmt::format("https://github.com/{}/{}", ORG_NAME, APP_NAME));

	auto *layout = Base::layout<QVBoxLayout>();

	searchBox = new QGroupBox(this);
	searchBox->setTitle(QStringLiteral("Query"));
	auto *searchLayout = new QHBoxLayout(searchBox);
	layout->addWidget(searchBox);

	search = new QLineEdit(this);
	searchLayout->addWidget(search);

	auto *searchButton = new QPushButton(this);
	searchButton->setText(QStringLiteral("Search"));
	searchButton->setDefault(true);
	searchLayout->addWidget(searchButton);

	QPushButton::connect(searchButton, &QPushButton::clicked,
		this, &Dialog::LyricsSearch::onSearchClicked);

	results = new QTreeWidget(this);
	results->setHeaderLabels({
		QStringLiteral("ID"),
		QStringLiteral("Title"),
		QStringLiteral("Artist"),
		QStringLiteral("Album"),
	});
	layout->addWidget(results, 1);

	QTreeWidget::connect(results, &QTreeWidget::itemDoubleClicked,
		this, &Dialog::LyricsSearch::onResultsItemDoubleClicked);

	auto *header = results->header();
	header->resizeSection(0, widthId);
	header->resizeSection(1, widthTitle);
	header->resizeSection(2, widthArtist);

	Base::addAction(DialogAction::Ok);
	resize(width, height);
}

void Dialog::LyricsSearch::onSearchClicked(bool /*checked*/)
{
	searchBox->setEnabled(false);
	results->clear();

	const auto query = search->text().toStdString();
	lyrics.search(query, [this](const lib::result<std::vector<lib::lrc::lyrics>> &result)
	{
		if (!result.success())
		{
			QMessageBox::warning(this, QStringLiteral("Error"), result.message());
		}
		else
		{
			for (const auto &lyrics: result.value())
			{
				auto *item = new QTreeWidgetItem(results);
				item->setText(0, QString::number(lyrics.id));
				item->setText(1, QString::fromStdString(lyrics.track_name));
				item->setText(2, QString::fromStdString(lyrics.artist_name));
				item->setText(3, QString::fromStdString(lyrics.album_name));
			}
		}

		searchBox->setEnabled(true);
	});
}

void Dialog::LyricsSearch::onResultsItemDoubleClicked(QTreeWidgetItem *item, int /*column*/)
{
	SidePanel::View *sidePanel = nullptr;
	for (auto *window: QApplication::topLevelWidgets())
	{
		sidePanel = window->findChild<SidePanel::View *>();
		if (sidePanel != nullptr)
		{
			break;
		}
	}

	if (sidePanel == nullptr)
	{
		return;
	}

	const auto lyricsId = item->text(0).toInt();
	sidePanel->openLyrics(lyricsId);
	accept();
}
