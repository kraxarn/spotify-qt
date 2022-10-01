#include "dialog/lyricssearch.hpp"
#include "view/sidepanel/view.hpp"

#include <QGroupBox>
#include <QPushButton>
#include <QMessageBox>

Dialog::LyricsSearch::LyricsSearch(const lib::http_client &httpClient, QWidget *parent)
	: Base(parent),
	lyrics(httpClient)
{
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
	lyrics.search(query, [this](const lib::result<std::vector<lib::lrc::search_result>> &result)
	{
		if (!result.success())
		{
			QMessageBox::warning(this, QStringLiteral("Error"),
				QString::fromStdString(result.message()));
		}
		else
		{
			for (const auto &searchResult: result.value())
			{
				const auto artists = lib::strings::join(searchResult.artists, ", ");
				auto *item = new QTreeWidgetItem(results);
				item->setText(0, QString::number(searchResult.lyrics_id));
				item->setText(1, QString::fromStdString(searchResult.track));
				item->setText(2, QString::fromStdString(artists));
				item->setText(3, QString::fromStdString(searchResult.album));
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
