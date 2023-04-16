#pragma once

#include "dialog/base.hpp"
#include "thirdparty/json.hpp"

#include <QTextEdit>

namespace Dialog
{
	class JsonDump: public Base
	{
	public:
		JsonDump(nlohmann::json json, QWidget *parent);

	protected:
		void showEvent(QShowEvent *event) override;

	private:
		static constexpr int width = 400;
		static constexpr int height = 600;

		nlohmann::json json;
		QTextEdit *textEdit;
	};

}
