#include "dialog/base.hpp"
#include <QPushButton>

Dialog::Base::Base(QWidget *parent)
	: QDialog(parent)
{
}

void Dialog::Base::onOk(bool /*checked*/)
{
	accept();
}

void Dialog::Base::onApply(bool /*checked*/)
{
}

void Dialog::Base::onCancel(bool /*checked*/)
{
	accept();
}

void Dialog::Base::addAction(DialogAction dialogAction)
{
	if (QDialog::layout() == nullptr)
	{
		return;
	}

	if (dialogAction == DialogAction::Ok && ok == nullptr)
	{
		ok = getButtonBox()->addButton(QDialogButtonBox::Ok);

		QPushButton::connect(ok, &QPushButton::clicked,
			this, &Dialog::Base::onOk);
	}
	else if (dialogAction == DialogAction::Apply && apply == nullptr)
	{
		apply = getButtonBox()->addButton(QDialogButtonBox::Apply);

		QPushButton::connect(apply, &QPushButton::clicked,
			this, &Dialog::Base::onApply);
	}
	else if (dialogAction == DialogAction::Cancel && cancel == nullptr)
	{
		cancel = getButtonBox()->addButton(QDialogButtonBox::Cancel);

		QPushButton::connect(cancel, &QPushButton::clicked,
			this, &Dialog::Base::onCancel);
	}
}

auto Dialog::Base::addButton(const QString &text,
	QDialogButtonBox::ButtonRole role) -> QPushButton *
{
	getButtonBox()->addButton(text, role);
}

void Dialog::Base::setTitle(const QString &text)
{
	setWindowTitle(text);
}

auto Dialog::Base::okButton() const -> QAbstractButton *
{
	return ok;
}

auto Dialog::Base::getButtonBox() -> QDialogButtonBox *
{
	if (buttonBox == nullptr)
	{
		buttonBox = new QDialogButtonBox(this);
		QDialog::layout()->addWidget(buttonBox);
	}

	return buttonBox;
}
