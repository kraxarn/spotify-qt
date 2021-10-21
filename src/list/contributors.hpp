#pragma once

#include "lib/github/api.hpp"

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
		const lib::http_client &httpClient;
	};
}
