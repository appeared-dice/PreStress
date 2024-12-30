#pragma once

#include <QDialog>
#include "ui_AreaDialog.h"

class AreaDialog : public QDialog
{
	Q_OBJECT

public:
	AreaDialog(QWidget *parent = nullptr);
	~AreaDialog();
	void initUI();
	void setAreaComboBox(QStringList list);

private slots:
	void emitData();
	

private:
	Ui::AreaDialogClass ui;
signals:
	void areaChanged(QString data);
};
