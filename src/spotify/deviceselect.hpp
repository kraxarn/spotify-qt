#include "lib/spotify/deviceselect.hpp"

#include <QObject>

namespace spt
{
	class DeviceSelect: public lib::spt::device_select, public QObject
	{
	public:
		explicit DeviceSelect(QObject *parent);

		void select_device(const std::vector<lib::spt::device> &devices,
			lib::callback<lib::spt::device> &callback) override;
	};
}
