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

	if (buttonBox == nullptr)
	{
		buttonBox = new QDialogButtonBox(this);
		QDialog::layout()->addWidget(buttonBox);
	}

	if (dialogAction == DialogAction::Ok && ok == nullptr)
	{
		ok = buttonBox->addButton(QDialogButtonBox::Ok);
		QPushButton::connect(ok, &QPushButton::clicked,
			this, &Dialog::Base::onOk);
	}
	else if (dialogAction == DialogAction::Apply && apply == nullptr)
	{
		apply = buttonBox->addButton(QDialogButtonBox::Apply);
		QPushButton::connect(apply, &QPushButton::clicked,
			this, &Dialog::Base::onApply);
	}
	else if (dialogAction == DialogAction::Cancel && cancel == nullptr)
	{
		cancel = buttonBox->addButton(QDialogButtonBox::Cancel);
		QPushButton::connect(cancel, &QPushButton::clicked,
			this, &Dialog::Base::onCancel);
	}
}

void Dialog::Base::setTitle(const QString &text)
{
	setWindowTitle(text);
}

auto Dialog::Base::okButton() const -> QAbstractButton *
{
	return ok;
}
