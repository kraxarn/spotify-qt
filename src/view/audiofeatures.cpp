#include "view/audiofeatures.hpp"

View::AudioFeatures::AudioFeatures(QWidget *parent)
	: QTreeWidget(parent)
{
	setEnabled(false);

	setEditTriggers(QAbstractItemView::NoEditTriggers);
	header()->hide();
	setSelectionMode(QAbstractItemView::NoSelection);
	setRootIsDecorated(false);
	setAllColumnsShowFocus(true);
	setColumnCount(2);
	header()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

View::AudioFeatures::AudioFeatures(lib::spt::api &spotify,
	const std::string &trackId, QWidget *parent)
	: AudioFeatures(parent)
{
	spotify.track_audio_features(trackId,
		[this](const lib::spt::audio_features &features)
		{
			loaded(features);
		});
}

View::AudioFeatures::AudioFeatures(lib::spt::api &spotify,
	const std::vector<std::string> &trackIds, QWidget *parent)
	: AudioFeatures(parent)
{
	spotify.track_audio_features(trackIds,
		[this](const std::vector<lib::spt::audio_features> &features)
		{
			loaded(features);
		});
}

void View::AudioFeatures::loaded(const lib::spt::audio_features &features)
{
	for (const auto &item: features.items())
	{
		auto *treeItem = new QTreeWidgetItem(this, {
			QString::fromStdString(item.get_feature_string()),
			QString::fromStdString(item.get_value_string()),
		});

		treeItem->setToolTip(1,
			QString::fromStdString(item.get_description()));
	}

	setEnabled(true);
}

void View::AudioFeatures::loaded(const std::vector<lib::spt::audio_features> &features)
{
	if (features.empty())
	{
		return;
	}

	for (const auto &frontItem: features.front().items())
	{
		const auto feature = frontItem.get_feature();
		std::vector<float> values;

		// Loop through other tracks
		for (auto iter = features.cbegin() + 1; iter < features.cend(); iter++)
		{
			// Loop through features for that track
			for (const auto &item: iter->items())
			{
				if (item.get_feature() == feature)
				{
					values.push_back(item.get_value());
					break;
				}
			}
		}

		const auto value = std::accumulate(values.cbegin(), values.cend(), 0.F);
		lib::spt::audio_feature item(feature, value);

		auto *treeItem = new QTreeWidgetItem(this, {
			QString::fromStdString(item.get_feature_string()),
			QString::fromStdString(item.get_value_string()),
		});

		treeItem->setToolTip(1,
			QString::fromStdString(item.get_description()));
	}

	setEnabled(true);
}
