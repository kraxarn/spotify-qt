#pragma once

#include "lib/spotify/api.hpp"

#include <QAbstractItemView>
#include <QDockWidget>
#include <QHeaderView>
#include <QTreeWidget>

namespace View
{
	class AudioFeatures: public QTreeWidget
	{
	Q_OBJECT

	public:
		AudioFeatures(lib::spt::api &spotify, const std::string &trackId, QWidget *parent);

		AudioFeatures(lib::spt::api &spotify, const std::vector<std::string> &trackIds,
			QWidget *parent);

	private:
		explicit AudioFeatures(QWidget *parent);

		void loaded(const lib::spt::audio_features &features);
		void loaded(const std::vector<lib::spt::audio_features> &features);
	};
}
