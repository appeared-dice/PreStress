#include "SteelListDialog.h"

SteelListDialog::SteelListDialog(QWidget* parent, QSqlDatabase* db)
	:PropertyOrShapeDialog(parent), db(db) {
	initUI();
}

void SteelListDialog::initUI() {
	this->setWindowTitle("钢束特性列表");
	ui.listView->setEditTriggers(QListView::EditTrigger::NoEditTriggers);		// 设置ListView条目不可编辑
	showPropertyList();
	connect(ui.addBtn, SIGNAL(clicked()), this, SLOT(addSteelProperty()));
	connect(ui.editBtn, SIGNAL(clicked()), this, SLOT(editSteelProperty()));
	connect(ui.removeBtn, SIGNAL(clicked()), this, SLOT(removeSteelProperty()));
}

void SteelListDialog::showPropertyList() {
	//QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");		// 连接数据库
	//db.setDatabaseName("steel_pp.db");
	if (db->open()) {
		//qDebug() << "数据库打开成功！";
		//qDebug() << db->connectionName();
		//QSqlQuery query(*db);
		//QString queryFindList = "SELECT 钢束特性名称 FROM steel_property;";
		//QStringList steelNameList;
		//QStringListModel* nameListModel = new QStringListModel();
		//if (query.exec(queryFindList)) {
		//	while (query.next()) {
		//		QString steelName = query.value(0).toString();		// 钢束特性表钢束名称字段
		//		qDebug() << "Steel Feature Name:" << steelName;
		//		steelNameList.push_back(steelName);		// 将每个数据项名称字段添加进StringList对象
		//	}
		//	nameListModel->setStringList(steelNameList);		// 设置数据模型的数据列表
		//	ui.listView->setModel(nameListModel);		// 数据视图设置数据模型


		//}
		//else {
		//	qDebug() << "Query failed:" << query.lastError().text();
		//}

		QSqlTableModel* tableModel = new QSqlTableModel(this, *db);
		tableModel->setTable("steel_property");
		if (!tableModel->select()) {
			qDebug() << "数据表打开失败！";
		}
		tableModel->setHeaderData(tableModel->fieldIndex("钢束特性名称"), Qt::Horizontal, "钢束特性名称");
		ui.listView->setModel(tableModel);

		db->close();
		qDebug() << "关闭数据库连接";
	}
}

void SteelListDialog::addSteelProperty() {
	PropertyDialog* propertyDialog = new PropertyDialog(this, db, tableModel);			// 钢束特性值输入窗口
	propertyDialog->show();
}

void SteelListDialog::editSteelProperty() {

}

void SteelListDialog::removeSteelProperty() {

}

SteelListDialog::~SteelListDialog() {
	delete nameListModel;
}
