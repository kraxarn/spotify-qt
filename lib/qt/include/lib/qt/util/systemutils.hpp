#pragma once

#include <QString>

namespace lib
{
	namespace qt
	{
		namespace util
		{
			/**
			 * System utilities
			 */
			class system
			{
			public:
				/**
				 * Get environmental variable
				 */
				static auto env(const char *name) -> QString;

				/**
				 * Environmental variable is set in system
				 */
				static auto has_env(const char *name) -> bool;

			private:
				system() = default;
			};
		}
	}
}
