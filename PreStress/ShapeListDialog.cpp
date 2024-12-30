#include "ShapeListDialog.h"

ShapeListDialog::ShapeListDialog(QWidget* parent)
	:PropertyOrShapeDialog(parent) {
	initUI();
}

ShapeListDialog::~ShapeListDialog() {

}

void ShapeListDialog::initUI() {
	this->setWindowTitle("钢束形状列表");
	connect(ui.addBtn, SIGNAL(clicked()), this, SLOT(addSteelShape()));
	connect(ui.editBtn, SIGNAL(clicked()), this, SLOT(editSteelShape()));
	connect(ui.removeBtn, SIGNAL(clicked()), this, SLOT(removeSteelShape()));
}

void ShapeListDialog::addSteelShape() {

}

void ShapeListDialog::editSteelShape() {

}

void ShapeListDialog::removeSteelShape() {

}