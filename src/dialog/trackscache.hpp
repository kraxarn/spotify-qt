#pragma once

#include "lib/cache.hpp"
#include "dialog/base.hpp"

#include <QTreeWidget>

namespace Dialog
{
	class TracksCache: public Base
	{
	Q_OBJECT

	public:
		TracksCache(lib::cache &cache, QWidget *parent);

	protected:
		void showEvent(QShowEvent *event) override;

	private:
		static constexpr int width = 500;
		static constexpr int height = 300;

		QTreeWidget *tree = nullptr;
		lib::cache &cache;
	};
}
