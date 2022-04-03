#pragma once

#include <QAction>
#include <QMouseEvent>
#include <QMainWindow>
#include <QWindow>

class DragArea: public QWidget
{
public:
	DragArea(QWidget *target, QWidget *parent);

protected:
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
	QWidget *target;
	QWindow *targetWindow;
	QPoint dragPosition;

	void menu(const QPoint &pos);
	auto isWindowMaximized() -> bool;

	void onMaximize(bool checked);
};
