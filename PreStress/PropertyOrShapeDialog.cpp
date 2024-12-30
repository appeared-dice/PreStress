#include "PropertyOrShapeDialog.h"

PropertyOrShapeDialog::PropertyOrShapeDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	connect(ui.closeBtn, SIGNAL(clicked()), this, SLOT(closeDialog()));
}

PropertyOrShapeDialog::~PropertyOrShapeDialog()
{
}

void PropertyOrShapeDialog::closeDialog() {
	this->close();
}