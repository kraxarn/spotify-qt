#pragma once

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

			/**
			 * Allow user to select device
			 * @param devices Devices to select from
			 * @param callback Selected device
			 */
			virtual void get(const std::vector<lib::spt::device> &devices,
				lib::callback<lib::spt::device> &callback) const = 0;
		};
	}
}
