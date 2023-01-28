#include "lib/spotify/deviceselect.hpp"

#include <QObject>

namespace spt
{
	class DeviceSelect: public lib::spt::device_select, public QObject
	{
	Q_OBJECT

	public:
		explicit DeviceSelect(QObject *parent);

		void get(const std::vector<lib::spt::device> &devices,
			lib::callback<lib::spt::device> &callback) const override;
	};
}
