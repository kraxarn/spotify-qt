#pragma once

#include "lib/cache.hpp"
#include "util/dateutils.hpp"

#include <QWidget>
#include <QHBoxLayout>
#include <QListWidget>
#include <QTextEdit>
#include <QDateTime>

namespace View
{
	class Crashes: public QWidget
	{
	public:
		Crashes(lib::cache &cache, QWidget *parent);

	protected:
		void showEvent(QShowEvent *event) override;

	private:
		static constexpr int stack_trace_role = 0x100;

		lib::cache &cache;

		QListWidget *list;
		QTextEdit *log;

		void logItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
	};
}
