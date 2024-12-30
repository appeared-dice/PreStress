#pragma once

#include <QDialog>
#include "ui_PropertyOrShapeDialog.h"
#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqlquery.h>
#include <QtSql/qsqlerror.h>
#include <QtSql/qsqlrecord.h>

class PropertyOrShapeDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PropertyOrShapeDialog(QWidget* parent);
	virtual ~PropertyOrShapeDialog();
	virtual void initUI() = 0;
private slots:
	void closeDialog();
protected:
	Ui::Dialog ui;
};
