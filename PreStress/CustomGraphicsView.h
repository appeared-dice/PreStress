#pragma once

#include <qgraphicsview.h>
#include <qgraphicsscene.h>
#include <qgraphicsitem.h>
#include <qpen.h>
#include <qfont.h>
#include <qevent.h>

class CustomGraphicsView : public QGraphicsView
{
	Q_OBJECT

public:
	CustomGraphicsView(QWidget* parent, int x, int y, int w, int h);
	~CustomGraphicsView();
	QGraphicsScene* scene;
	void initView();
	void addAxis();

protected:
	void wheelEvent(QWheelEvent* event);

private:
	int x, y, w, h;
	int limit = 0;
};
