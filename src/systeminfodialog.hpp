#pragma once

#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

class SystemInfoDialog : public QDialog
{
	Q_OBJECT

public:
	SystemInfoDialog(const QString &info, QWidget *parent = nullptr);
};
