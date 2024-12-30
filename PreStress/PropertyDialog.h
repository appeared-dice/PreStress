#pragma once

#include <QDialog>
#include "ui_PropertyDialog.h"
#include "AreaDialog.h"
#include <qsqldatabase.h>
#include <qsqltablemodel.h>
#include <qsqlrecord.h>
#include <qsqlquery.h>

class PropertyDialog : public QDialog
{
	Q_OBJECT

public:
	PropertyDialog(QWidget* parent, QSqlDatabase* db, QSqlTableModel* tableModel);
	~PropertyDialog();
	void initUI();
	static void setValidator(QLineEdit* lineEdit, double bottom, double top, unsigned decimal);		// 设置输入框验证器
	void popWarningDialog(QString dialogTitle, QString warningText);

private slots:
	QString fpkChanged();
	void areaDialog();
	void setAreaValue(QString areaData);
	void submitData();

private:
	Ui::PropertyDialogClass ui;
	QSqlDatabase* db;
	QSqlTableModel* tableModel;

signals:

};
