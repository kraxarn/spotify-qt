#include "widget/apprefreshcombobox.hpp"

AppRefreshComboBox::AppRefreshComboBox(QWidget *parent)
	: QComboBox(parent)
{
	setEditable(true);
	setValidator(new QIntValidator(
		minRefreshInterval,
		maxRefreshInterval,
		this
	));

	connect(this, &QComboBox::currentTextChanged,
		this, &AppRefreshComboBox::onCurrentTextChanged);
}

void AppRefreshComboBox::setSuffixLabel(QLabel *label)
{
	mSuffixLabel = label;
	onCurrentTextChanged(currentText());
}

void AppRefreshComboBox::onCurrentTextChanged(const QString &text) const
{
	if (mSuffixLabel == nullptr)
	{
		return;
	}

	bool is_int = false;
	const int seconds = text.toInt(&is_int);

	mSuffixLabel->setVisible(is_int);
	mSuffixLabel->setText(seconds == 1
		? QStringLiteral("second")
		: QStringLiteral("seconds")
	);
}
