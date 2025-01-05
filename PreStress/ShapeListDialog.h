#pragma once
#include "PropertyOrShapeDialog.h"
#include <qstringlist.h>
#include <qstringlistmodel.h>
#include <qsqltablemodel.h>
#include <qmessagebox.h>
#include "ShapeDialog.h"

class ShapeListDialog :public PropertyOrShapeDialog {
	Q_OBJECT
public:
	explicit ShapeListDialog(QWidget* parent, QSqlDatabase* db);
	~ShapeListDialog() override;

	void initUI()override;
	void showShapeList();

private slots:
	void addSteelShape();
	void editSteelShape(QModelIndex listCurrentIndex);
	void removeSteelShape(QModelIndex listCurrentIndex);
	void updateSteelShape();

private:
	QSqlDatabase* db;			// 数据库对象
	QStringListModel* nameListModel;		// 数据模型对象
};
