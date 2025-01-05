#include "ShapeListDialog.h"

ShapeListDialog::ShapeListDialog(QWidget* parent, QSqlDatabase* db)
	:PropertyOrShapeDialog(parent), db(db), nameListModel(new QStringListModel(this)) {
	initUI();
}

void ShapeListDialog::initUI() {
	this->setWindowTitle("钢束形状列表");
	ui.listView->setEditTriggers(QListView::EditTrigger::NoEditTriggers);		// 设置ListView条目不可编辑
	showShapeList();
	connect(ui.addBtn, SIGNAL(clicked()), this, SLOT(addSteelShape()));
	connect(ui.editBtn, &QPushButton::clicked, this, [this]() {editSteelShape(ui.listView->currentIndex()); });		// 点击添加按钮添加钢束形状
	connect(ui.removeBtn, &QPushButton::clicked, this, [this]() {removeSteelShape(ui.listView->currentIndex()); });		// 点击删除按钮删除对应索引钢束形状信息
}

void ShapeListDialog::showShapeList() {
	if (db->open()) {
		qDebug() << "数据库打开成功！";
		qDebug() << db->connectionName();
		QSqlQuery query(*db);
		QString queryFindList = "SELECT 钢束名称 FROM steel_shape;";
		QStringList steelNameList;
		if (query.exec(queryFindList)) {
			while (query.next()) {
				QString steelName = query.value(0).toString();		// 钢束形状表钢束名称字段
				qDebug() << "Steel Feature Name:" << steelName;
				steelNameList.push_back(steelName);		// 将每个数据项名称字段添加进StringList对象
			}
			nameListModel->setStringList(steelNameList);		// 设置数据模型的数据列表
			ui.listView->setModel(nameListModel);		// 数据视图设置数据模型
		}
		else {
			qDebug() << "Query failed:" << query.lastError().text();
		}

		db->close();
		qDebug() << "关闭数据库连接";
	}
}

void ShapeListDialog::addSteelShape() {
	ShapeDialog* shapeDialog = new ShapeDialog(this, db);
	shapeDialog->setAttribute(Qt::WA_DeleteOnClose);
	shapeDialog->show();
	connect(shapeDialog, SIGNAL(steelListChanged()), this, SLOT(updateSteelShape()));
}

void ShapeListDialog::editSteelShape(QModelIndex listCurrentIndex) {
	QVariant steelIndexName = ui.listView->model()->data(listCurrentIndex);
	ShapeDialog* shapeDialog = new ShapeDialog(this, db, steelIndexName.toString());
	shapeDialog->setAttribute(Qt::WA_DeleteOnClose);
	if (!db->open()) {
		qDebug() << "数据库打开失败！";
		return;
	}

	QSqlQuery query;
	if (!steelIndexName.toString().isEmpty()) {
		query.prepare("SELECT * FROM steel_shape WHERE 钢束名称 = ?;");
		query.bindValue(0, steelIndexName.toString());
		query.exec();
		if (query.next()) {
			shapeDialog->ui.steelNameLE->setText(query.value(0).toString());
			shapeDialog->ui.steelPropertyCB->setCurrentText(query.value(1).toString());
			shapeDialog->ui.sigmaLE->setText(query.value(2).toString());
			QStringList horCoordinate = query.value(3).toString().split(';');
			QStringList verCoordinate = query.value(4).toString().split(';');
			horCoordinate.pop_back();		// 分隔数据后，尾元素为空，将其去除
			verCoordinate.pop_back();
			qDebug() << horCoordinate;
			shapeDialog->ui.horCoordinateTW->setRowCount(horCoordinate.size());
			shapeDialog->ui.verCoordinateTW->setRowCount(verCoordinate.size());
			for (int i = 0; i < horCoordinate.size(); ++i) {
				QStringList row = horCoordinate[i].split(',');
				QTableWidgetItem* hx = new QTableWidgetItem(row[0]);
				QTableWidgetItem* hy = new QTableWidgetItem(row[1]);
				QTableWidgetItem* hr = new QTableWidgetItem(row[2]);
				shapeDialog->ui.horCoordinateTW->setItem(i, 0, hx);
				shapeDialog->ui.horCoordinateTW->setItem(i, 1, hy);
				shapeDialog->ui.horCoordinateTW->setItem(i, 2, hr);
			}

			for (int i = 0; i < verCoordinate.size(); ++i) {
				QStringList row = verCoordinate[i].split(',');
				QTableWidgetItem* vx = new QTableWidgetItem(row[0]);
				QTableWidgetItem* vz = new QTableWidgetItem(row[1]);
				QTableWidgetItem* vr = new QTableWidgetItem(row[2]);
				shapeDialog->ui.verCoordinateTW->setItem(i, 0, vx);
				shapeDialog->ui.verCoordinateTW->setItem(i, 1, vz);
				shapeDialog->ui.verCoordinateTW->setItem(i, 2, vr);
			}
		}
	}
	shapeDialog->show();
	connect(shapeDialog, SIGNAL(steelListChanged()), this, SLOT(updateSteelShape()));
	db->close();
}

void ShapeListDialog::removeSteelShape(QModelIndex listCurrentIndex) {
	QString steelName = ui.listView->model()->data(listCurrentIndex).toString();
	if (!steelName.isEmpty()) {
		if (!db->open()) {
			qDebug() << "数据库打开失败！";
			return;
		}
		QSqlQuery query;
		query.prepare("DELETE FROM steel_shape WHERE 钢束名称=?;");
		query.bindValue(0, steelName);
		query.exec();
	}
	db->close();
	updateSteelShape();
}

void ShapeListDialog::updateSteelShape() {
	if (!db->open()) {
		qDebug() << "数据库打开失败！";
		return;
	}
	QSqlQuery query;
	QStringList nameList;
	if (query.exec("SELECT 钢束名称 FROM steel_shape;")) {
		while (query.next()) {
			nameList.push_back(query.value(0).toString());
		}
		nameListModel->setStringList(nameList);
		ui.listView->setModel(nameListModel);
	}
	db->close();
}

ShapeListDialog::~ShapeListDialog() {

}