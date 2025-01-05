#pragma once
#include "PropertyOrShapeDialog.h"
#include "PropertyDialog.h"
#include <qstringlist.h>
#include <qstringlistmodel.h>
#include <qsqltablemodel.h>
#include <qmessagebox.h>


class SteelListDialog :public PropertyOrShapeDialog {
	Q_OBJECT
public:
	explicit SteelListDialog(QWidget* parent, QSqlDatabase* db);
	~SteelListDialog() override;

	void initUI()override;
	void showPropertyList();
private slots:
	void addSteelProperty();
	void editSteelProperty(QModelIndex listCurrentIndex);
	void removeSteelProperty(QModelIndex listCurrentIndex);
	void updateSteelList();
	void popWarningDialog(QString dialogTitle, QString warningText);
private:
	QSqlDatabase* db;			// 数据库对象
	QStringListModel* nameListModel;		// 数据模型对象
};