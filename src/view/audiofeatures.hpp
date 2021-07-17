#pragma once

#include "util/treeutils.hpp"
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

	private:
		void loaded(const lib::spt::audio_features &features);
	};
}
