#pragma once

#include <string>

namespace lib
{
	namespace spt
	{
		namespace core
		{
			/**
			 * Result from a request
			 */
			class result
			{
			public:
				/**
				 * Request was successful
				 */
				bool success = false;

				/**
				 * Any err message if the request failed
				 */
				std::string message;

				/**
				 * Result with success set to true
				 * @return Result
				 */
				static result ok();

				/**
				 * Result with success set to false with message
				 * @param message Error message
				 * @return Result
				 */
				static result err(const std::string &message);

			private:
				result() = default;
			};

		}
	}
}
