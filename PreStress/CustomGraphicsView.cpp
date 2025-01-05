#include "CustomGraphicsView.h"

CustomGraphicsView::CustomGraphicsView(QWidget* parent, int x, int y, int w, int h)
	: QGraphicsView(parent), scene(new QGraphicsScene(this)), x(x), y(y), w(w), h(h)
{
	initView();
}

void CustomGraphicsView::initView() {
	setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);
	setGeometry(x, y, w, h);
	setScene(scene);		// 设置场景
	addAxis();		// 添加坐标轴
}

void CustomGraphicsView::addAxis() {
	int w = width(), h = height();

	QGraphicsLineItem* xAxis = scene->addLine(-w / 2, 0, w / 2, 0, QPen(Qt::GlobalColor::black));		// 创建x轴
	QGraphicsLineItem* yAxis = scene->addLine(0, -h / 2, 0, h / 2, QPen(Qt::GlobalColor::black));		// 创建y轴

	for (int i = (-w / 2); i < (w / 2) + 50; i += 50) {
		QGraphicsLineItem* xTick = new QGraphicsLineItem(i, -5, i, 5);				// 创建x坐标轴标点以及坐标值对象
		QGraphicsTextItem* xLabel = new QGraphicsTextItem(QString::number(i));
		xLabel->setPos(i - 10, 10);
		xLabel->setFont(QFont("黑体", 8));
		xLabel->setData(0, "text");

		scene->addItem(xTick);			// 场景中添加x坐标轴标点以及坐标值对象
		scene->addItem(xLabel);
	}

	for (int j = (-h / 2); j < (h / 2) + 50; j += 50) {
		if (j != 0) {
			QGraphicsTextItem* yLabel = new QGraphicsTextItem(QString::number(-j));
			yLabel->setPos(-45, j - 10);
			yLabel->setFont(QFont("黑体", 8));
			yLabel->setData(0, "text");
			scene->addItem(yLabel);
		}

		QGraphicsLineItem* yTick = new QGraphicsLineItem(-5, j, 5, j);
		scene->addItem(yTick);
	}
}

void CustomGraphicsView::wheelEvent(QWheelEvent* event) {
	int factor = 2;			// 滚轮缩放因子
	if (event->angleDelta().y() > 0) {
		if (limit < 600) {
			for (auto item : scene->items()) {
				if (item != nullptr) {
					if (item->data(2) == "line" || item->data(1) == "arc") {
						item->setScale(1.0 / factor * item->scale());
						if (auto lineItem = dynamic_cast<QGraphicsLineItem*>(item)) {
							QPen pen = lineItem->pen();
							pen.setWidthF(pen.widthF() * factor);  // 按比例调整线宽
							lineItem->setPen(pen);
						}
						if (auto arcItem = dynamic_cast<QGraphicsPathItem*>(item)) {
							QPen pen = arcItem->pen();
							pen.setWidthF(pen.widthF() * factor);  // 按比例调整线宽
							arcItem->setPen(pen);
						}
					}
					if (auto textItem = dynamic_cast<QGraphicsTextItem*>(item)) {				// 动态类型转换，确定QGraphicsItem是否是QGraphicsTextItem的实例，将基类指针转换为派生类指针
						textItem->setPlainText(QString::number(textItem->toPlainText().toInt() * factor));		// 修改坐标值大小
					}
				}
			}
		}
	}
	else {
		if (limit > 0) {
			for (auto item : scene->items()) {
				if (item != nullptr) {
					if (item->data(2) == "line" || item->data(1) == "arc") {
						item->setScale(factor * item->scale());
						if (auto lineItem = dynamic_cast<QGraphicsLineItem*>(item)) {
							QPen pen = lineItem->pen();
							pen.setWidthF(pen.widthF() / factor);  // 按比例调整线宽
							lineItem->setPen(pen);
						}
						if (auto arcItem = dynamic_cast<QGraphicsPathItem*>(item)) {
							QPen pen = arcItem->pen();
							pen.setWidthF(pen.widthF() / factor);  // 按比例调整线宽
							arcItem->setPen(pen);
						}
					}
					if (auto textItem = dynamic_cast<QGraphicsTextItem*>(item)) {
						textItem->setPlainText(QString::number(textItem->toPlainText().toDouble() / factor));
					}
				}
			}
		}
	}

	if (0 < limit + event->angleDelta().y() && limit + event->angleDelta().y() < 600) {
		limit += event->angleDelta().y();			// 限制缩放次数，防止因为缩放成0值而无法放大
	}
	else if (limit + event->angleDelta().y() <= 0) {
		limit = 0;
	}
	else {
		limit = 600;
	}
}

CustomGraphicsView::~CustomGraphicsView() {

}
