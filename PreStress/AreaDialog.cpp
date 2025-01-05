#include "AreaDialog.h"
#include "InternalData.h"

AreaDialog::AreaDialog(QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	initUI();
}

void AreaDialog::initUI() {
	ui.steelNumberSB->setMinimum(1);		// 设置钢束数最小为1

	connect(ui.cancelBtn, SIGNAL(clicked()), this, SLOT(close()));		// 点击取消按钮关闭窗口

	connect(ui.confirmBtn, SIGNAL(clicked()), this, SLOT(emitData()));		// 点击确认按钮发送钢束数量选择数据
}

void AreaDialog::emitData() {
	QString steelType, data, currentText = ui.steelDiameterCB->currentText();
	int steelNumber;
	double steelArea;
	for (auto c : ui.steelDiameterCB->currentText()) {
		if (!c.isNumber()) {
			steelType += c;
		}
		else {
			break;
		}
	}
	steelType = steelType.simplified();
	qDebug() << steelType;

	steelNumber = ui.steelNumberSB->value();
	auto steelPairIter = std::find_if(Data::steelAreaVector.begin(), Data::steelAreaVector.end(), [steelType](const std::pair<QString, std::vector<std::pair<QString, double>>>& pair) {return pair.first == steelType; });
	auto steelAreaVector = (*steelPairIter).second;
	auto steelAreaPairIter = std::find_if(steelAreaVector.begin(), steelAreaVector.end(), [currentText](const std::pair<QString, double>& pair) {return currentText == pair.first; });
	steelArea = (*steelAreaPairIter).second;
	data = QString::number(steelNumber * steelArea, 'f', 2);

	emit areaChanged(data);			// 将选择的钢束的对应的面积数据信号发送出去
	qDebug() << "发送了数据";
	close();
}

void AreaDialog::setAreaComboBox(QStringList list) {
	ui.steelDiameterCB->addItems(list);
}


AreaDialog::~AreaDialog()
{}
