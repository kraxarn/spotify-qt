#pragma once

#include <QDialog>
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>

class TextEditDialog : public QDialog
{
	Q_OBJECT

public:
	TextEditDialog(const QString &title, const QString &label,
		const QString &text = QString(), QWidget *parent = nullptr);
	QString text();
	static QString getText(const QString &title, const QString &label,
		const QString &text = QString(), QWidget *parent = nullptr);

private:
	QTextEdit	*textEdit;
};