#pragma once
#include <iostream>
#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqlquery.h>
#include <QtSql/qsqlerror.h>
#include <QtSql/qsqlrecord.h>

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();
	void initUI();
	void initDB();

private slots:
	void steelStrandppDialog();		// 弹出钢束物理特性列表窗口
	void steelShapeDialog();		// 弹出钢束形状列表窗口

private:
	Ui::MainWindowClass ui;
	QSqlDatabase* db;
};
