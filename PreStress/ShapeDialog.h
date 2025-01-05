#pragma once

#include <QDialog>
#include "ui_ShapeDialog.h"
#include "CustomGraphicsView.h"
#include <qsqldatabase.h>
#include <qsqltablemodel.h>
#include <qsqlrecord.h>
#include <qsqlquery.h>
#include "SteelListDialog.h"
#include <qmenu.h>
#include <math.h>
#include <tuple>
#include <qregularexpression.h>
#include <vector>
#include <qclipboard.h>
#include <algorithm>
#include "ShapeListDialog.h"

class ShapeDialog : public QDialog
{
	friend class ShapeListDialog;
	Q_OBJECT

public:
	ShapeDialog(QWidget* parent, QSqlDatabase* db, QString steelIndexName = NULL);
	~ShapeDialog();

	typedef std::tuple<double, double, double, double, double, double, double, double> keyPoints;		// 关键点数据
	typedef std::tuple<double, double, double> coordinate;		// 钢束坐标数据
	
	void initUI();
	void initSteelPropertyCB();			// 初始化钢束特性下拉框列表
	void drawHorizontalShape(std::vector<coordinate> coordinateVector = {});		// 绘制平弯图形
	void drawVerticalShape(std::vector<coordinate> coordinateVector = {});			// 绘制竖弯图形
	void drawSteelShape(std::vector<coordinate> coordinateVector, CustomGraphicsView* horizontalView);		// 绘制钢束形状
	void popWarningDialog(QString dialogTitle, QString warningText);		// 弹出警告框
	void copyContent(QTableWidget* table, QClipboard* clipboard);		// 复制tableWidget内容
	void pasteContent(QTableWidget* table, QClipboard* clipboard);		// 粘贴内容至tableWidget
	void deleteContent(QTableWidget* table);		// 删除tableWidget选中的单元格内容
	keyPoints getKeyPoints(double xa, double ya, double xb, double yb, double xp, double yp, double r);		// 获取关键点坐标
	std::vector<coordinate> getCoordinate(QTableWidget* table);			// 通过tableWidget获取钢束坐标
	std::tuple<QString, QString> getDataSequence(std::vector<coordinate> coordinateVector);		// 获取数据字符串序列


protected:
	bool eventFilter(QObject* watched, QEvent* event);		// 重写监听事件

private:
	Ui::ShapeDialogClass ui;
	QSqlDatabase* db;
	QString steelIndexName;

private slots:
	void generateMenu(const QPoint& pos, QTableWidget* table);
	void updateShapeView();			// 更新钢束视图
	void submitData();		// 提交钢束形状数据至数据库

signals:
	void steelListChanged();		// 钢束列表改变信号
};
