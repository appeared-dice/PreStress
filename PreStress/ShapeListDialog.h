#pragma once
#include "PropertyOrShapeDialog.h"

class ShapeListDialog :public PropertyOrShapeDialog {
	Q_OBJECT
public:
	explicit ShapeListDialog(QWidget* parent);
	~ShapeListDialog() override;

	void initUI()override;
private slots:
	void addSteelShape();
	void editSteelShape();
	void removeSteelShape();
};
