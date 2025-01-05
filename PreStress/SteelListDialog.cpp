#include "SteelListDialog.h"

SteelListDialog::SteelListDialog(QWidget* parent, QSqlDatabase* db)
	:PropertyOrShapeDialog(parent), db(db), nameListModel(new QStringListModel(this)) {
	initUI();
}

void SteelListDialog::initUI() {
	this->setWindowTitle("钢束特性列表");
	ui.listView->setEditTriggers(QListView::EditTrigger::NoEditTriggers);		// 设置ListView条目不可编辑
	showPropertyList();
	connect(ui.addBtn, SIGNAL(clicked()), this, SLOT(addSteelProperty()));
	connect(ui.editBtn, &QPushButton::clicked, this, [this]() {editSteelProperty(ui.listView->currentIndex()); });		// SLOT宏只能接受普通的成员函数，不能包含具体的函数代码
	connect(ui.removeBtn, &QPushButton::clicked, this, [this]() {removeSteelProperty(ui.listView->currentIndex()); });
}

void SteelListDialog::showPropertyList() {

	if (db->open()) {
		qDebug() << "数据库打开成功！";
		qDebug() << db->connectionName();
		QSqlQuery query(*db);
		QString queryFindList = "SELECT 钢束特性名称 FROM steel_property;";
		QStringList steelNameList;
		if (query.exec(queryFindList)) {
			while (query.next()) {
				QString steelName = query.value(0).toString();		// 钢束特性表钢束名称字段
				qDebug() << "Steel Feature Name:" << steelName;
				steelNameList.push_back(steelName);		// 将每个数据项名称字段添加进StringList对象
			}
			nameListModel->setStringList(steelNameList);		// 设置数据模型的数据列表
			ui.listView->setModel(nameListModel);		// 数据视图设置数据模型


		}
		else {
			qDebug() << "Query failed:" << query.lastError().text();
		}

		/*QSqlTableModel* tableModel = new QSqlTableModel(this, *db);
		tableModel->setTable("steel_property");
		if (!tableModel->select()) {
			qDebug() << "数据表打开失败！";
		}
		tableModel->setHeaderData(tableModel->fieldIndex("钢束特性名称"), Qt::Horizontal, "钢束特性名称");
		ui.listView->setModel(tableModel);*/

		db->close();
		qDebug() << "关闭数据库连接";
	}
}

void SteelListDialog::addSteelProperty() {
	PropertyDialog* propertyDialog = new PropertyDialog(this, db);			// 钢束特性值输入窗口
	propertyDialog->setAttribute(Qt::WA_DeleteOnClose);
	propertyDialog->show();
	connect(propertyDialog, SIGNAL(steelListChanged()), this, SLOT(updateSteelList()));			// 关闭钢束特性输入窗口时更新钢束列表
}

void SteelListDialog::editSteelProperty(QModelIndex listCurrentIndex) {
	QVariant steelIndexName = ui.listView->model()->data(listCurrentIndex);		// 点击编辑按钮时将对应钢束列表listView中的当前项的index传入PropertyDialog初始化函数
	PropertyDialog* propertyDialog = new PropertyDialog(this, db, steelIndexName.toString());
	propertyDialog->setAttribute(Qt::WA_DeleteOnClose);
	if (!db->open()) {
		qDebug() << "数据库打开失败！";
		return;
	}

	QSqlQuery query;
	qDebug() << steelIndexName.toString();
	if (!steelIndexName.toString().isEmpty()) {
		query.prepare("SELECT * FROM steel_property WHERE 钢束特性名称 = ?;");			// 查找对应的数据项，将propertyDialog各组件设置上对应的值
		query.bindValue(0, steelIndexName.toString());
		query.exec();
		if (query.next()) {
			propertyDialog->ui.steelNameLE->setText(query.value(0).toString());
			propertyDialog->ui.steelTypeCB->setCurrentText(query.value(1).toString());
			propertyDialog->ui.materialCB->setCurrentText(query.value(2).toString());
			propertyDialog->ui.steelAreaLE->setText(query.value(3).toString());
			propertyDialog->ui.cubeDiameterLE->setText(query.value(4).toString());
			propertyDialog->ui.steelRelaxCoefCB->setCurrentText(query.value(5).toString());
			propertyDialog->ui.steelFpkLE->setText(query.value(6).toString());
			propertyDialog->ui.steelfCoefLE->setText(query.value(7).toString());
			propertyDialog->ui.steeldCoefLE->setText(query.value(8).toString());
			propertyDialog->ui.startLE->setText(query.value(9).toString());
			propertyDialog->ui.finalLE->setText(query.value(10).toString());
			if (query.value(11).toString() == "粘结") {
				propertyDialog->ui.glueRB->setChecked(true);
			}
			else {
				propertyDialog->ui.unglueRB->setChecked(false);
			}
		}
	}
	propertyDialog->show();
	connect(propertyDialog, SIGNAL(steelListChanged()), this, SLOT(updateSteelList()));			// 关闭钢束特性输入窗口时更新钢束列表
	db->close();
}

void SteelListDialog::removeSteelProperty(QModelIndex listCurrentIndex) {
	QString steelName = ui.listView->model()->data(listCurrentIndex).toString();		// 根据当前钢束列表的currentIndex删除对应钢束数据项
	if (!steelName.isEmpty()) {
		if (!db->open()) {
			qDebug() << "数据库打开失败！";
			return;
		}
		QSqlQuery query;
		query.prepare("DELETE FROM steel_property WHERE 钢束特性名称=?;");
		query.bindValue(0, steelName);
		query.exec();
	}
	db->close();

	updateSteelList();
}

void SteelListDialog::updateSteelList() {
	if (!db->open()) {
		qDebug() << "数据库打开失败！";
		return;
	}
	QSqlQuery query;
	QStringList nameList;
	if (query.exec("SELECT 钢束特性名称 FROM steel_property;")) {
		while (query.next()) {
			nameList.push_back(query.value(0).toString());
		}
		nameListModel->setStringList(nameList);
		ui.listView->setModel(nameListModel);
	}
	db->close();
}

void SteelListDialog::popWarningDialog(QString dialogTitle, QString warningText) {
	QMessageBox* warningDialog = new QMessageBox(QMessageBox::Icon::Warning, dialogTitle, warningText, QMessageBox::StandardButton::Yes, this);			// 创建信息对话框对象
	warningDialog->button(QMessageBox::StandardButton::Yes)->setText("确认");		// 将对话框默认的确认按钮名称改为“确认”
	warningDialog->setAttribute(Qt::WA_DeleteOnClose);
	warningDialog->exec();
}

SteelListDialog::~SteelListDialog() {

}
