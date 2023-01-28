#include "lib/spotify/device.hpp"
#include "lib/spotify/callback.hpp"

namespace lib
{
	namespace spt
	{
		class device_select
		{
		public:
			device_select() = default;

			virtual void select_device(const std::vector<lib::spt::device> &devices,
				lib::callback<lib::spt::device> &callback) = 0;
		};
	}
}
