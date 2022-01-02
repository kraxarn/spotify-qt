#pragma once
#include "lib/logmessage.hpp"

#include <QWidget>
#include <QTreeWidget>

namespace Log
{
	class Base: public QWidget
	{
	protected:
		explicit Base(QWidget *parent);

		virtual auto getMessages() -> const std::vector<lib::log_message> & = 0;

		void showEvent(QShowEvent *event) override;

	private:
		static constexpr int messageRole = 0x100;

		QTreeWidget *list;

		auto collectLogs() -> QString;

		void onCopyToClipboard(bool checked);
		void onSaveToFile(bool checked);
	};
}
