#pragma once

#include "lib/github/api.hpp"
#include "util/url.hpp"
#include "util/parent.hpp"
#include "dialog/settings.hpp"

#include <QListWidget>

namespace List
{
	class Contributors: public QListWidget
	{
	Q_OBJECT

	public:
		Contributors(const HttpClient &httpClient);

	protected:
		void showEvent(QShowEvent *event) override;

	private:
		static constexpr int urlRole = 0x100;

		const HttpClient &httpClient;

		void onItemDoubleClicked(QListWidgetItem *item);
	};
}
