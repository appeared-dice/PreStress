#include "ShapeDialog.h"

ShapeDialog::ShapeDialog(QWidget* parent, QSqlDatabase* db, QString steelIndexName)
	: QDialog(parent), db(db)
{
	ui.setupUi(this);
	initUI();
}

void ShapeDialog::initUI() {
	QDoubleValidator* validator = new QDoubleValidator(this);			// 创建验证器对象
	validator->setRange(0.00, 99999.99);
	validator->setNotation(QDoubleValidator::StandardNotation);
	validator->setDecimals(2);
	ui.sigmaLE->setValidator(validator);			// 设置钢束张拉力验证器

	ui.horCoordinateTW->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.verCoordinateTW->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(ui.horCoordinateTW, &QTableWidget::customContextMenuRequested, this, [this](const QPoint& pos) {generateMenu(pos, ui.horCoordinateTW); });			// 右键点击水平坐标表格窗弹出菜单栏
	connect(ui.verCoordinateTW, &QTableWidget::customContextMenuRequested, this, [this](const QPoint& pos) {generateMenu(pos, ui.verCoordinateTW); });			// 右键点击竖直坐标表格窗弹出菜单栏
	ui.horCoordinateTW->installEventFilter(this);		// 安装事件过滤器
	ui.verCoordinateTW->installEventFilter(this);

	connect(ui.updateBtn, SIGNAL(clicked()), this, SLOT(updateShapeView()));		// 点击更新图像按钮更新钢束形状
	connect(ui.confirmBtn, SIGNAL(clicked()), this, SLOT(submitData()));		// 点击确认按钮提交数据
	connect(ui.cancelBtn, SIGNAL(clicked()), this, SLOT(close()));		// 点击取消按钮关闭窗口

	drawHorizontalShape();		// 绘制水平钢束形状视图
	drawVerticalShape();		// 绘制竖直钢束形状视图

	initSteelPropertyCB();		// 初始化钢束特性值下拉框
}

void ShapeDialog::initSteelPropertyCB() {
	if (!db->open()) {
		qDebug() << "数据库打开失败！";
	}
	QSqlQuery query;
	if (query.exec("SELECT 钢束特性名称 FROM steel_property")) {
		while (query.next()) {
			ui.steelPropertyCB->addItem(query.value(0).toString());
		}
	}
	db->close();
}

void ShapeDialog::drawHorizontalShape(std::vector<coordinate> coordinateVector) {
	if (coordinateVector.size() == 0) {
		CustomGraphicsView* horizontalView = new CustomGraphicsView(this, 380, 80, 400, 200);		// 创建水平钢束形状视图
		horizontalView->show();
	}
	else {
		CustomGraphicsView* horizontalView = new CustomGraphicsView(this, 380, 80, 400, 200);
		drawSteelShape(coordinateVector, horizontalView);
		horizontalView->show();
	}
}

void ShapeDialog::drawVerticalShape(std::vector<coordinate> coordinateVector) {
	if (coordinateVector.size() == 0) {
		CustomGraphicsView* verizontalView = new CustomGraphicsView(this, 380, 300, 400, 200);		// 创建竖直钢束形状视图
		verizontalView->show();
	}
	else {
		CustomGraphicsView* verizontalView = new CustomGraphicsView(this, 380, 300, 400, 200);
		drawSteelShape(coordinateVector, verizontalView);
		verizontalView->show();
	}
}

void ShapeDialog::drawSteelShape(std::vector<coordinate> coordinateVector, CustomGraphicsView* GraphicsView) {
	std::vector<std::tuple<double, double, size_t>>Straight;
	QPen pen;			// 画笔对象
	pen.setWidth(2);		// 设置画笔线宽
	for (int i = 0; i < coordinateVector.size(); ++i) {
		if (std::get<2>((coordinateVector)[i]) == 0) {
			double x = std::get<0>((coordinateVector)[i]);
			double y = std::get<1>((coordinateVector)[i]);
			Straight.push_back(std::make_tuple(x, y, i));
		}

		if (std::get<2>((coordinateVector)[i]) != 0) {
			double xa = std::get<0>((coordinateVector)[i - 1]);
			double ya = std::get<1>((coordinateVector)[i - 1]);
			double xb = std::get<0>((coordinateVector)[i + 1]);
			double yb = std::get<1>((coordinateVector)[i + 1]);
			double xp = std::get<0>((coordinateVector)[i]);
			double yp = std::get<1>((coordinateVector)[i]);
			double r = std::get<2>((coordinateVector)[i]);

			keyPoints keyPointsTuple = getKeyPoints(xa, ya, xb, yb, xp, yp, r);			// 获取关键点坐标

			double xt1 = std::get<0>(keyPointsTuple);
			double yt1 = std::get<1>(keyPointsTuple);
			double xt2 = std::get<2>(keyPointsTuple);
			double yt2 = std::get<3>(keyPointsTuple);
			double xo = std::get<4>(keyPointsTuple);
			double yo = std::get<5>(keyPointsTuple);
			double startAngle = std::get<6>(keyPointsTuple);
			double arcLength = std::get<7>(keyPointsTuple);

			Straight.push_back(std::make_tuple(xt1, yt1, i));
			Straight.push_back(std::make_tuple(xt2, yt2, i));

			QPainterPath arcPath;
			arcPath.moveTo(xt1, -yt1);
			arcPath.arcTo(xo - r, -yo - r, 2 * r, 2 * r, startAngle, arcLength);		// 绘制曲线段
			QGraphicsPathItem* arc = new QGraphicsPathItem(arcPath);

			arc->setPen(pen);
			arc->setData(1, "arc");			// 设置自定义属性
			GraphicsView->scene->addItem(arc);
		}

		if (i == (coordinateVector.size() - 2)) {
			break;
		}
	}
	Straight.push_back(std::make_tuple(std::get<0>((coordinateVector).back()), std::get<1>((coordinateVector).back()), Straight.size() + 1));
	for (int i = 0; i < Straight.size(); ++i) {
		if (std::get<2>(Straight[i]) != std::get<2>(Straight[i + 1])) {
			double x1 = std::get<0>(Straight[i]);
			double y1 = std::get<1>(Straight[i]);
			double x2 = std::get<0>(Straight[i + 1]);
			double y2 = std::get<1>(Straight[i + 1]);

			QGraphicsLineItem* line = new QGraphicsLineItem(x1, -y1, x2, -y2);			// 绘制直线段
			line->setPen(pen);
			line->setData(2, "line");
			GraphicsView->scene->addItem(line);
		}
		if (i == (Straight.size() - 2)) {
			break;
		}
	}
}

void ShapeDialog::generateMenu(const QPoint& pos, QTableWidget* table) {
	int rowNum = -1;
	rowNum = table->selectionModel()->selection().indexes().last().row();		// 获取被选中区域的最后一行行号

	QMenu* menu = new QMenu(this);			// 创建右键菜单对象
	QAction* m1 = menu->addAction("插入一行");		// 添加菜单项

	if (table->rowCount() > 1) {			// 当只有一行时只能进行插入操作
		QAction* m2 = menu->addAction("删除一行");
		QAction* action = menu->exec(table->mapToGlobal(pos));			// 在右键点击的位置弹出菜单栏
		if (action == m1) {
			table->insertRow(rowNum + 1);			// 插入一行
		}
		if (action == m2) {
			table->removeRow(rowNum);			// 删除一行
		}
	}
	else {
		QAction* action = menu->exec(table->mapToGlobal(pos));
		if (action == m1) {
			table->insertRow(rowNum + 1);
		}
	}
}

ShapeDialog::keyPoints ShapeDialog::getKeyPoints(double xa, double ya, double xb, double yb, double xp, double yp, double r) {
	double lap = pow((pow((xp - xa), 2) + pow((yp - ya), 2)), 0.5);
	double lpb = pow((pow((xb - xp), 2) + pow((yb - yp), 2)), 0.5);
	double lab = pow((pow((xb - xa), 2) + pow((yb - ya), 2)), 0.5);
	double theta = acos((pow(lap, 2) + pow(lpb, 2) - pow(lab, 2)) / (2 * lap * lpb)) / 2;

	double t = r / tan(theta);
	double theta_ap = atan((yp - ya) / (xp - xa));
	double theta_pb = atan((yb - yp) / (xb - xp));
	double xt1 = xp - t * cos(theta_ap);
	double yt1 = yp - t * sin(theta_ap);
	double xt2 = xp + t * cos(theta_pb);
	double yt2 = yp + t * sin(theta_pb);
	double yo = ((xp - xa) * (xb - xp) * (xt2 - xt1) + (xp - xa) * (yb - yp) * yt2 - (xb - xp) * (yp - ya) * yt1) / ((xp - xa) * (yb - yp) - (xb - xp) * (yp - ya));
	double xo = ((xp - xa) * xt1 + (yp - ya) * yt1 - (yp - ya) * yo) / (xp - xa);

	double xv = xo + 100;
	double yv = yo;

	double lot1 = pow((pow((xt1 - xo), 2) + pow((yt1 - yo), 2)), 0.5);
	double lov = pow((pow((xv - xo), 2) + pow((yv - yo), 2)), 0.5);
	double lvt1 = pow((pow((xt1 - xv), 2) + pow((yt1 - yv), 2)), 0.5);
	double cosStartAngle = (pow(lot1, 2) + pow(lov, 2) - pow(lvt1, 2)) / (2 * lot1 * lov);
	double startAngle = 0, arcLength = 0;

	if ((xt1 > xo && yt1 > yo) || (xt1 < xo && yt1 > yo)) {
		startAngle = acos(cosStartAngle) * 360 / (2 * acos(-1));
	}
	else if ((xt1 > xo && yt1 < yo) || (xt1 < xo && yt1 < yo)) {
		startAngle = -acos(cosStartAngle) * 360 / (2 * acos(-1));
	}
	else if ((xt1 == xo) && (yt1 > yo)) {
		startAngle = 90;
	}
	else if ((xt1 == xo) && (yt1 < yo)) {
		startAngle = -90;
	}

	if ((xt1 >= xo && yt1 > yo) || (xt1 <= xo && yt1 > yo)) {
		arcLength = -(180 - 2 * (theta * 360 / (2 * acos(-1))));
	}
	else if ((xt1 <= xo && yt1 < yo) || (xt1 >= xo && yt1 < yo)) {
		arcLength = 180 - 2 * (theta * 360 / (2 * acos(-1)));
	}

	return std::make_tuple(xt1, yt1, xt2, yt2, xo, yo, startAngle, arcLength);
}

std::vector<ShapeDialog::coordinate> ShapeDialog::getCoordinate(QTableWidget* table) {
	QRegularExpression reg1, reg2;		// 创建正则表达式变量
	reg1.setPattern("^(-?6[0-3]\\d{2}|-?[1-5]\\d{3}|-?\\d{1,3})(\\.\\d{1,4})?$");		// 设置匹配模式
	reg2.setPattern("^(-?1000(\\.\\d{1,4})?|-?[1-9]\\d{0,2}(\\.\\d{1,4})?|0(\\.\\d{1,4})?)$");

	int rows = table->rowCount();
	std::vector<ShapeDialog::coordinate> coordinateVector;
	QString x, y, r;
	for (int row = 0; row < rows; ++row) {
		if ((!table->item(row, 0)->text().isEmpty()) || (!table->item(row, 1)->text().isEmpty()) || (!table->item(row, 2)->text().isEmpty())) {
			x = table->item(row, 0)->text();
			y = table->item(row, 1)->text();
			r = table->item(row, 2)->text();
		}
		if ((!reg1.match(x).hasMatch()) || (!reg1.match(y).hasMatch()) || (!reg2.match(r).hasMatch())) {
			popWarningDialog("数据警告", "钢束坐标输入有误！");			// 如果坐标输入有误将会匹配失败，弹出警告框
			return coordinateVector;
		}
		coordinateVector.push_back(std::make_tuple(x.toDouble(), y.toDouble(), r.toDouble()));		// 将坐标添加进容器
	}
	return coordinateVector;
}

std::tuple<QString, QString> ShapeDialog::getDataSequence(std::vector<coordinate> coordinateVector) {
	QString coordinateSequence;
	std::vector<double> thetaList;

	for (int i = 0; i < coordinateVector.size(); ++i) {
		ShapeDialog::coordinate row = (coordinateVector)[i];
		double x = std::get<0>(row);
		double y = std::get<1>(row);
		double r = std::get<2>(row);
		coordinateSequence += QString::number(x) + "," + QString::number(y) + "," + QString::number(r) + ";";
	}

	for (int i = 0; i < coordinateVector.size(); ++i) {
		if (std::get<2>((coordinateVector)[i]) != 0) {
			double xa = std::get<0>((coordinateVector)[i - 1]);
			double ya = std::get<1>((coordinateVector)[i - 1]);
			double xb = std::get<0>((coordinateVector)[i + 1]);
			double yb = std::get<1>((coordinateVector)[i + 1]);
			double xp = std::get<0>((coordinateVector)[i]);
			double yp = std::get<1>((coordinateVector)[i]);

			double lap = pow((pow((xp - xa), 2) + pow((yp - ya), 2)), 0.5);
			double lpb = pow((pow((xb - xp), 2) + pow((yb - yp), 2)), 0.5);
			double lab = pow((pow((xb - xa), 2) + pow((yb - ya), 2)), 0.5);
			double theta = acos((pow(lap, 2) + pow(lpb, 2) - pow(lab, 2)) / (2 * lap * lpb)) / 2;
			qDebug() << theta;
			thetaList.push_back(acos(-1) - theta);
		}
		if (i == (coordinateVector.size() - 2)) {
			break;
		}
	}
	std::vector<double>alphaList(thetaList.size());
	std::partial_sum(thetaList.begin(), thetaList.end(), alphaList.begin());

	QString alphaSequence;
	for (int i = 0; i < alphaList.size(); ++i) {
		alphaSequence += std::to_string(alphaList[i]);
		if (i != alphaList.size() - 1) {
			alphaSequence += ",";
		}
	}
	qDebug() << alphaSequence;

	return std::make_tuple(coordinateSequence, alphaSequence);
}

void ShapeDialog::submitData() {
	if (ui.steelNameLE->text().isEmpty() || ui.sigmaLE->text().isEmpty()) {
		popWarningDialog("数据警告", "您未输入钢束名称或张拉力！");
		return;
	}
	std::vector<ShapeDialog::coordinate> horCoordinateVector = getCoordinate(ui.horCoordinateTW);		// 获取钢束平弯坐标
	std::vector<ShapeDialog::coordinate> verCoordinateVector = getCoordinate(ui.verCoordinateTW);		// 获取钢束竖弯坐标

	if (horCoordinateVector.size() == 0 || verCoordinateVector.size() == 0) {
		return;
	}
	if (std::get<0>((horCoordinateVector)[0]) != std::get<0>((verCoordinateVector)[0]) && std::get<0>(horCoordinateVector.back()) != std::get<0>(verCoordinateVector.back())) {
		popWarningDialog("数据警告", "钢束数据有误！");
		return;
	}

	QString steelName = ui.steelNameLE->text();		// 获取钢束名称
	QString steelProperty = ui.steelPropertyCB->currentText();		// 获取钢束特性
	double steelSigma = ui.sigmaLE->text().toDouble();		// 获取钢束张拉力
	QString horCoordinateSequence, verCoordinateSequence;
	QString horAlphaSequence, verAlphaSequence;

	std::tuple<QString, QString> horDataSequence = getDataSequence(horCoordinateVector);		// 获取钢束平弯坐标字符串序列
	std::tuple<QString, QString> verDataSequence = getDataSequence(verCoordinateVector);		// 获取钢束竖弯坐标字符串序列

	horCoordinateSequence = std::get<0>(horDataSequence);
	horAlphaSequence = std::get<1>(horDataSequence);			// 平弯转角序列
	verCoordinateSequence = std::get<0>(verDataSequence);
	verAlphaSequence = std::get<1>(verDataSequence);		// 竖弯转角序列

	if (!db->open()) {
		qDebug() << "数据库打开失败！";
	}

	QSqlQuery query;

	if (!steelIndexName.isEmpty()) {
		query.prepare("SELECT * FROM steel_shape WHERE 钢束名称 = ?;");
		query.bindValue(0, steelIndexName);
		if (query.exec()) {
			while (query.next()) {
				if (query.value(0) == steelName && query.value(0) != steelIndexName) {
					popWarningDialog("数据警告", QString("钢束名称%1已存在").arg(steelName));
					return;
				}
			}
		}

		query.prepare("UPDATE steel_shape SET 钢束名称=?,钢束特性值=?,张拉力=?,平弯坐标=?,竖弯坐标=?,平弯转角和=?,竖弯转角和=? WHERE 钢束名称 = ?;");
		query.bindValue(0, steelName);
		query.bindValue(1, steelProperty);
		query.bindValue(2, steelSigma);
		query.bindValue(3, horCoordinateSequence);
		query.bindValue(4, verCoordinateSequence);
		query.bindValue(5, horAlphaSequence);
		query.bindValue(6, verAlphaSequence);
		query.bindValue(7, steelIndexName);
		query.exec();
	}
	else {
		query.prepare("SELECT * FROM steel_shape WHERE 钢束名称 = ?;");
		query.bindValue(0, steelName);
		if (query.exec()) {
			while (query.next()) {
				if (query.value(0) == steelName) {
					popWarningDialog("数据警告", QString("钢束名称%1已存在").arg(steelName));
					return;
				}
			}
		}
		query.prepare("INSERT INTO steel_shape VALUES (?,?,?,?,?,?,?);");
		query.bindValue(0, steelName);
		query.bindValue(1, steelProperty);
		query.bindValue(2, steelSigma);
		query.bindValue(3, horCoordinateSequence);
		query.bindValue(4, verCoordinateSequence);
		query.bindValue(5, horAlphaSequence);
		query.bindValue(6, verAlphaSequence);
		query.exec();
	}
	db->close();
	close();
	emit steelListChanged();		// 发射钢束形状列表变化信号
}

void ShapeDialog::popWarningDialog(QString dialogTitle, QString warningText) {
	QMessageBox* warningDialog = new QMessageBox(QMessageBox::Icon::Warning, dialogTitle, warningText, QMessageBox::StandardButton::Yes, this);			// 创建信息对话框对象
	warningDialog->button(QMessageBox::StandardButton::Yes)->setText("确认");		// 将对话框默认的确认按钮名称改为“确认”
	warningDialog->setAttribute(Qt::WA_DeleteOnClose);
	warningDialog->exec();
}

void ShapeDialog::updateShapeView() {
	std::vector<ShapeDialog::coordinate> horCoordinateVector = getCoordinate(ui.horCoordinateTW);		// 获取平弯坐标
	std::vector<ShapeDialog::coordinate> verCoordinateVector = getCoordinate(ui.verCoordinateTW);		// 获取竖弯坐标
	drawHorizontalShape(horCoordinateVector);		// 绘制平弯图形
	drawVerticalShape(verCoordinateVector);			// 绘制竖弯图形
}

bool ShapeDialog::eventFilter(QObject* watched, QEvent* event) {
	if (QTableWidget* table = qobject_cast<QTableWidget*>(watched)) {		// 筛选被监听的对象是否是QtableWidget
		QClipboard* clipboard = QApplication::clipboard();		// 获取剪贴板对象

		if (event->type() == QEvent::KeyPress) {
			QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);		// 将事件对象类型转换为键盘事件对象

			if (keyEvent->modifiers() & Qt::ControlModifier) {			// 这里使用&位与运算符是因为keyEvent->modifiers()是一个位掩码，Qt::ControlModifier是特定位
				if (keyEvent->key() == Qt::Key_C) {
					copyContent(table, clipboard);			// 复制表格内容至剪贴板
					return true;
				}

				if (keyEvent->key() == Qt::Key_V) {
					pasteContent(table, clipboard);			// 粘贴剪贴板内容至表格
					return true;
				}
			}
			if (keyEvent->key() == Qt::Key_Delete) {
				deleteContent(table);		// 删除表格内容
				return true;
			}
		}
	}

	return QDialog::eventFilter(watched, event);
}

void ShapeDialog::copyContent(QTableWidget* table, QClipboard* clipboard) {
	auto selected = table->selectedRanges();		// 获取表格被选中的单元格对象
	if (selected.isEmpty()) {
		return;
	}
	int r1 = selected[0].topRow();		// 获取顶行行号
	int r2 = selected[0].bottomRow();		// 获取底行行号
	int c1 = selected[0].leftColumn();		// 获取左行行号
	int c2 = selected[0].rightColumn();		// 获取右行行号

	QString clipboardText;
	for (int row = r1; row < r2 + 1; ++row) {			// 遍历选中的单元格内容
		for (int col = c1; col < c2 + 1; ++col) {
			QTableWidgetItem* item = table->item(row, col);
			if (item) {
				clipboardText += item->text() + '\t';
			}
			else {
				clipboardText += '\t';
			}
		}
		clipboardText = clipboardText.trimmed() + '\n';
	}
	clipboard->setText(clipboardText);		// 设置剪贴板内容
}

void ShapeDialog::pasteContent(QTableWidget* table, QClipboard* clipboard) {
	auto selected = table->selectedRanges();
	if (selected.isEmpty()) {
		return;
	}
	int r = selected[0].topRow();
	int c = selected[0].leftColumn();

	QString clipboardText = clipboard->text().trimmed();		// 去除换行符
	QStringList lines = clipboardText.split('\n');			// 按行分割

	for (int row = 0; row < lines.size(); ++row) {
		QStringList cells = lines[row].split('\t');		// 将每行内容进行分隔
		for (int col = 0; col < cells.size(); ++col) {
			if ((r + row) >= table->rowCount() || (c + col) >= table->columnCount()) {
				continue;			// 超出表格范围时跳过
			}

			QTableWidgetItem* item = new QTableWidgetItem(cells[col]);
			table->setItem(r + row, c + col, item);
		}
	}
}

void ShapeDialog::deleteContent(QTableWidget* table) {
	auto selected = table->selectedRanges();
	if (selected.isEmpty()) {
		return;
	}
	int r1 = selected[0].topRow();
	int r2 = selected[0].bottomRow();
	int c1 = selected[0].leftColumn();
	int c2 = selected[0].rightColumn();

	for (int row = r1; row <= r2; ++row) {
		for (int col = c1; col <= c2; ++col) {
			QTableWidgetItem* item = table->item(row, col);
			if (item != nullptr) {
				item->setText("");		// 将内容清空
			}
		}
	}
}

ShapeDialog::~ShapeDialog()
{

}
