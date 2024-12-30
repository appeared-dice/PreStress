#include "MainWindow.h"
#include "PropertyOrShapeDialog.h"
#include "SteelListDialog.h"
#include "ShapeListDialog.h"
#include <qdebug.h>
#include <iostream>


MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
{
	qDebug() << "设置ui";
	try {
		ui.setupUi(this);
	}
	catch (const std::exception& e) {
		qDebug() << "异常：" << e.what();
	}

	qDebug() << "初始化UI";
	initUI();
	qDebug() << "初始化数据库";
	initDB();
}

void MainWindow::initUI() {
	this->setWindowTitle("预应力模块");
	connect(ui.propertyBtn, SIGNAL(clicked()), this, SLOT(steelStrandppDialog()));
	connect(ui.shapeBtn, SIGNAL(clicked()), this, SLOT(steelShapeDialog()));
}

void MainWindow::initDB() {
	db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));			// 连接数据库
	db->setDatabaseName("steel_pp.db");					// 创建数据库名称
	if (!db->open()) {
		qDebug() << "数据库打开失败！";
		return;
	}
	else {
		qDebug() << "数据库打开成功！";
	}
	QSqlQuery query(*db);
	QString queryPropertyTable = "CREATE TABLE IF NOT EXISTS steel_property(钢束特性名称 TEXT PRIMARY KEY,\
																			钢束类型 TEXT,\
																			材料 TEXT,\
																			钢束总面积 NUMBER,\
																			导管直径 NUMBER,\
																			钢筋松弛系数 NUMBER,\
																			fpk NUMBER,\
																			摩擦系数 NUMBER,\
																			摩擦影响系数 NUMBER,\
																			开始压缩值 NUMBER,\
																			结束压缩值 NUMBER,\
																			粘结类型 TEXT); ";				// 创建钢束特性表
	QString queryShapeTable = "CREATE TABLE IF NOT EXISTS steel_shape(钢束名称 TEXT PRIMARY KEY,\
																	钢束特性值 TEXT,\
																	张拉力 NUMBER,\
																	平弯坐标 TEXT,\
																	竖弯坐标 TEXT,\
																	平弯转角和 TEXT,\
																	竖弯转角和 TEXT,\
																	FOREIGN KEY(钢束特性值) REFERENCES steel_property(钢束特性名称) ON DELETE CASCADE ON UPDATE CASCADE\
																	); ";				// 创建钢束形状表
	query.exec(queryPropertyTable);		// 执行SQL语句
	query.exec(queryShapeTable);
	//db->close();		// 关闭数据库连接
	qDebug() << "关闭数据库连接";
}

void MainWindow::steelStrandppDialog() {
	PropertyOrShapeDialog* propertyListDialog = new SteelListDialog(this, db);
	propertyListDialog->setAttribute(Qt::WA_DeleteOnClose);		// 确保窗口关闭时自动销毁资源
	propertyListDialog->show();
}

void MainWindow::steelShapeDialog() {
	PropertyOrShapeDialog* shapeListDialog = new ShapeListDialog(this);
	shapeListDialog->setAttribute(Qt::WA_DeleteOnClose);
	shapeListDialog->show();
}

MainWindow::~MainWindow()
{
	//QSqlDatabase::removeDatabase("qt_sql_default_connection");		// 移除数据库连接
	qDebug() << "数据库移除！";
}
