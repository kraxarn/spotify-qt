#pragma once

#include "lib/qt/util/systemutils.hpp"

#include <QString>
#include <QMap>

namespace lib
{
	namespace qt
	{
		/**
		 * System information
		 */
		class system_info
		{
		public:
			/**
			 * Instance new system info with basic information
			 */
			system_info();

			/**
			 * Add custom value
			 */
			void add(const QString &key, const QString &value);

			/**
			 * Format info as plain text
			 */
			auto to_text() -> QString;

			/**
			 * Format info as html
			 */
			auto to_html() -> QString;

		private:
			QMap<QString, QString> info;
		};
	}
}
