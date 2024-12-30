#pragma once
#include "PropertyOrShapeDialog.h"
#include "PropertyDialog.h"
#include <qstringlist.h>
#include <qstringlistmodel.h>
#include <qsqltablemodel.h>


class SteelListDialog :public PropertyOrShapeDialog {
	Q_OBJECT
public:
	explicit SteelListDialog(QWidget* parent, QSqlDatabase* db);
	~SteelListDialog() override;

	void initUI()override;
	void showPropertyList();
private slots:
	void addSteelProperty();
	void editSteelProperty();
	void removeSteelProperty();
private:
	QSqlDatabase* db;			// 数据库对象
	QSqlTableModel* tableModel;
	QStringList nameList;
	QStringListModel* nameListModel;
};