#include "audiofeaturesview.hpp"

AudioFeaturesView::AudioFeaturesView(spt::Spotify &spotify, const std::string &trackId,
	QWidget *parent)
	: QTreeWidget(parent)
{
	setEnabled(false);

	spotify.track_audio_features(trackId,
		[this](const lib::spt::audio_features &audioFeatures)
		{
			this->loaded(audioFeatures);
			this->setEnabled(true);
		});

	setEditTriggers(QAbstractItemView::NoEditTriggers);
	header()->hide();
	setSelectionMode(QAbstractItemView::NoSelection);
	setRootIsDecorated(false);
	setAllColumnsShowFocus(true);
	setColumnCount(2);
	header()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void AudioFeaturesView::loaded(const lib::spt::audio_features &features)
{
	for (const auto &value : features.items())
	{
		addTopLevelItem(TreeUtils::item(this,
			value.get_feature(), value.get_value()));
	}
}
