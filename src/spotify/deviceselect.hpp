#pragma once

#include "lib/spotify/deviceselect.hpp"
#include <QObject>

namespace spt
{
	class DeviceSelect: public QObject, public lib::spt::device_select
	{
	Q_OBJECT

	public:
		explicit DeviceSelect(QObject *parent);

		void get(const std::vector<lib::spt::device> &devices,
			lib::callback<lib::spt::device> &callback) const override;
	};
}
