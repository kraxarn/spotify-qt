#pragma once

#include "lib/github/api.hpp"
#include "util/urlutils.hpp"

#include <QListWidget>

namespace List
{
	class Contributors: public QListWidget
	{
	Q_OBJECT

	public:
		Contributors(const lib::http_client &httpClient);

	protected:
		void showEvent(QShowEvent *event) override;

	private:
		static constexpr int urlRole = 0x100;

		const lib::http_client &httpClient;

		void onItemDoubleClicked(QListWidgetItem *item);
	};
}
