#pragma once

#include "lib/settings.hpp"
#include "lib/enums.hpp"

#include "enum/dialogaction.hpp"
#include "util/icon.hpp"
#include "widget/dragarea.hpp"

#include <QDialog>
#include <QVBoxLayout>
#include <QToolBar>
#include <QLabel>
#include <QDialogButtonBox>

namespace Dialog
{
	class Base: public QDialog
	{
	protected:
		explicit Base(QWidget *parent);

		void addAction(DialogAction dialogAction);
		auto addButton(const QString &text, QDialogButtonBox::ButtonRole role) -> QPushButton *;

		void setTitle(const QString &text);

		virtual void onOk(bool checked);
		virtual void onApply(bool checked);
		virtual void onCancel(bool checked);

		auto okButton() const -> QAbstractButton *;

		template<typename T>
		auto layout() -> T *
		{
			if (QDialog::layout() == nullptr)
			{
				new T(this);
			}

			return qobject_cast<T *>(QDialog::layout());
		}

	private:
		QDialogButtonBox *buttonBox = nullptr;
		QPushButton *ok = nullptr;
		QPushButton *apply = nullptr;
		QPushButton *cancel = nullptr;

		auto getButtonBox() -> QDialogButtonBox *;
	};
}
