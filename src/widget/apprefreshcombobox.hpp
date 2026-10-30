#pragma once

#include <QComboBox>
#include <QLabel>
#include <QMetaObject>

class AppRefreshComboBox : public QComboBox
{
	Q_OBJECT

public:
	static constexpr int minRefreshInterval = 1;
	static constexpr int maxRefreshInterval = 60;

	explicit AppRefreshComboBox(QWidget *parent);

	void setSuffixLabel(QLabel *label);

private:
	QLabel *mSuffixLabel = nullptr;

	void onCurrentTextChanged(const QString &text) const;
};
