#pragma once

#include <QDialog>
#include "ui_PropertyDialog.h"
#include "AreaDialog.h"
#include <qsqldatabase.h>
#include <qsqltablemodel.h>
#include <qsqlrecord.h>
#include <qsqlquery.h>
#include "SteelListDialog.h"

class PropertyDialog : public QDialog
{
	friend class SteelListDialog;
	Q_OBJECT

public:
	PropertyDialog(QWidget* parent = nullptr, QSqlDatabase* d = nullptr, QString steelIndexName = NULL);
	~PropertyDialog();
	void initUI();
	void setValidator(QLineEdit* lineEdit, double bottom, double top, unsigned decimal);		// 设置输入框验证器
	void popWarningDialog(QString dialogTitle, QString warningText);

private slots:
	QString fpkChanged();
	void areaDialog();
	void setAreaValue(QString areaData);
	void submitData();

private:
	Ui::PropertyDialogClass ui;
	QSqlDatabase* db;
	QString steelIndexName;			// 钢束列表索引名称

signals:
	void steelListChanged();
};
