#include "PropertyDialog.h"
#include <algorithm>
#include "InternalData.h"
#include "qmessagebox.h"
#include <math.h>

PropertyDialog::PropertyDialog(QWidget* parent, QSqlDatabase* db, QString steelIndexName)
	: QDialog(parent), db(db)
{
	ui.setupUi(this);
	initUI();
}

void PropertyDialog::initUI() {
	QStringList steelTypeList;
	std::for_each(Data::steelTypeVector.begin(), Data::steelTypeVector.end(), [&steelTypeList](const QString& type) {steelTypeList.push_back(type); });
	ui.steelTypeCB->addItems(steelTypeList);		// 添加钢束类型下拉框选项

	QStringList materialList;
	std::for_each(Data::materialPropertyVector.begin(), Data::materialPropertyVector.end(), [&materialList](std::pair<const QString, Data::MaterialProperties> material) {materialList.push_back(material.first); });
	ui.materialCB->addItems(materialList);			// 添加材料下拉框选项

	QStringList relaxCoeffList;
	std::for_each(Data::relaxCoeffVector.begin(), Data::relaxCoeffVector.end(), [&relaxCoeffList](const double& coeff) {relaxCoeffList.push_back(QString::number(coeff)); });
	ui.steelRelaxCoefCB->addItems(relaxCoeffList);			// 添加钢筋松弛系数下拉框选项

	connect(ui.materialCB, SIGNAL(currentIndexChanged(int)), this, SLOT(fpkChanged()));		// 改变钢束材料选项触发改变fpk值

	connect(ui.steelAreaTB, SIGNAL(clicked()), this, SLOT(areaDialog()));		// 点击面积选择按钮弹出面积输入对话框

	connect(ui.cancelBtn, SIGNAL(clicked()), this, SLOT(close()));			// 取消按钮按下关闭窗口

	connect(ui.confirmBtn, SIGNAL(clicked()), this, SLOT(submitData()));		// 点击确认按钮提交数据

	ui.steelFpkLE->setEnabled(false);		// 设置钢束fpk输入框无法进行编辑
	ui.steelFpkLE->setText(fpkChanged());		// 设置窗口打开时最初的fpk值

	ui.steelAreaLE->setText("0");
	setValidator(ui.steelAreaLE, 0.00, 99999.99, 2);		// 设置面积输入框验证器

	ui.cubeDiameterLE->setText("0");
	setValidator(ui.cubeDiameterLE, 0.00, 99999.99, 2);			// 设置导管直径输入框验证器

	ui.steelfCoefLE->setText("0.3");
	setValidator(ui.steelfCoefLE, 0.00, 10.00, 2);			// 设置摩擦系数输入框验证器

	ui.steeldCoefLE->setText("0.00000066");
	setValidator(ui.steeldCoefLE, 0.00, 1.00, 8);			// 设置摩擦系数影响系数输入框验证器

	ui.startLE->setText("6");
	setValidator(ui.startLE, 0.00, 100.00, 2);			// 设置钢筋压缩开始值输入框验证器

	ui.finalLE->setText("6");
	setValidator(ui.finalLE, 0.00, 100.00, 2);			// 设置钢筋压缩结束值输入框验证器

	ui.glueRB->setChecked(true);			// 设置粘结类型粘结选项选中状态
}

void PropertyDialog::setValidator(QLineEdit* lineEdit, double bottom, double top, unsigned decimal) {
	QDoubleValidator* validator = new QDoubleValidator(bottom, top, decimal, this);			// 创建验证器对象
	validator->setNotation(QDoubleValidator::StandardNotation);			// 设置数字以标准表示法输入，不允许使用科学计数法
	lineEdit->setValidator(validator);
}

void PropertyDialog::popWarningDialog(QString dialogTitle, QString warningText) {
	QMessageBox* warningDialog = new QMessageBox(QMessageBox::Icon::Warning, dialogTitle, warningText, QMessageBox::StandardButton::Yes, this);			// 创建信息对话框对象
	warningDialog->button(QMessageBox::StandardButton::Yes)->setText("确认");		// 将对话框默认的确认按钮名称改为“确认”
	warningDialog->setAttribute(Qt::WA_DeleteOnClose);
	warningDialog->exec();
}

QString PropertyDialog::fpkChanged() {
	QString fpk;
	for (auto c : ui.materialCB->currentText()) {
		if (c.isNumber()) {
			fpk += c;
		}
	}
	ui.steelFpkLE->setText(fpk);		// 将钢筋fpk值设置为当前选定的钢束材料类型对应的fpk值
	return fpk;
}

void PropertyDialog::areaDialog() {
	AreaDialog* areaInputDialog = new AreaDialog(this);
	areaInputDialog->setAttribute(Qt::WA_DeleteOnClose);		// 窗口关闭时销毁资源

	QString diameter;
	QStringList diameterList;
	for (auto c : ui.materialCB->currentText()) {
		if (!c.isNumber()) {
			diameter += c;
		}
	}
	auto diameterPairIter = std::find_if(Data::steelAreaVector.begin(), Data::steelAreaVector.end(), [diameter](const std::pair<QString, std::vector<std::pair<QString, double>>>& pair) {return pair.first == diameter; });
	auto diameterPair = *diameterPairIter;
	std::for_each(diameterPair.second.begin(), diameterPair.second.end(), [&diameterList](const std::pair<QString, double>& pair) {diameterList.push_back(pair.first); });
	areaInputDialog->setAreaComboBox(diameterList);		// 设置面积下拉框数据
	connect(areaInputDialog, SIGNAL(areaChanged(QString)), this, SLOT(setAreaValue(QString)));		// 按下确认按钮后发送数据并将其设置给面积输入框的当前值
	areaInputDialog->show();
}

void PropertyDialog::setAreaValue(QString areaData) {
	ui.steelAreaLE->setText(areaData);		// 将面积选择窗口发送的信号数据设置到钢束面积输入框中
}

void PropertyDialog::submitData() {
	QString steelName = ui.steelNameLE->text();				// 钢束名称
	QString steelType = ui.steelTypeCB->currentText();			// 钢束类型
	QString material = ui.materialCB->currentText();			// 钢束材料
	double steelArea = ui.steelAreaLE->text().toDouble();			// 钢束总面积
	double cubeDiameter = ui.cubeDiameterLE->text().toDouble();			// 导管直径
	double relaxCoeff = ui.steelRelaxCoefCB->currentText().toDouble();			// 松弛系数
	double fpk = ui.steelFpkLE->text().toDouble();			// fpk值
	double frictionCoeff = ui.steelfCoefLE->text().toDouble();			// 摩擦系数
	double deviation = ui.steeldCoefLE->text().toDouble();			// 摩擦偏差系数
	double startCompress = ui.startLE->text().toDouble();			// 压缩开始值
	double finalCompress = ui.finalLE->text().toDouble();			// 压缩结束值
	QString bondType = ui.glueRB->isChecked() ? ui.glueRB->text() : ui.unglueRB->text();		// 粘结类型
	QSqlQuery query;		// 创建query对象

	if (steelName.isEmpty()) {
		popWarningDialog("数据警告", "您未输入钢束特性名称！");			// 如果未输入钢束名称则弹出警告框
		return;
	}
	if (steelArea == 0 || cubeDiameter == 0 || steelArea > std::pow(cubeDiameter, 2) * std::acos(-1) / 4) {
		popWarningDialog("数据警告", "钢束面积或导管直径数据有误！");			// 如果导管面积小于钢束总面积则弹出警告框
		return;
	}

	if (!db->open()) {
		qDebug() << "数据库打开失败！";
	}

	if (!steelIndexName.isEmpty()) {
		query.prepare("SELECT * FROM steel_property WHERE 钢束特性名称 = ?;");			// 如果初始化的钢束索引名称不为空，说明是点击了编辑按钮，则进行编辑操作
		query.bindValue(0, steelName);			// 绑定替换SQL语句的?变量
		if (query.exec()) {
			while (query.next()) {			// 循环拿到每条记录
				if (query.value(0) == steelName && query.value(0) != steelIndexName) {
					popWarningDialog("数据警告", QString("钢束特性名称%1已存在").arg(steelName));
					return;
				}
			}
		}

		query.prepare("UPDATE steel_property SET 钢束特性名称=?,钢束类型=?,材料=?,钢束总面积=?,导管直径=?,\
													钢筋松弛系数=?,fpk=?,摩擦系数=?,摩擦影响系数=?,开始压缩值=?,\
													结束压缩值=?,粘结类型=? where 钢束特性名称 = ?;");				// 如果是编辑状态，提交即更新数据
		query.bindValue(0, steelName);
		query.bindValue(1, steelType);
		query.bindValue(2, material);
		query.bindValue(3, steelArea);
		query.bindValue(4, cubeDiameter);
		query.bindValue(5, relaxCoeff);
		query.bindValue(6, fpk);
		query.bindValue(7, frictionCoeff);
		query.bindValue(8, deviation);
		query.bindValue(9, startCompress);
		query.bindValue(10, finalCompress);
		query.bindValue(11, bondType);
		query.bindValue(12, steelIndexName);
		query.exec();
	}
	else {			// 如果初始化的钢束索引名称为空，说明是点击了添加按钮，则进行添加操作
		query.prepare("SELECT * FROM steel_property WHERE 钢束特性名称 = ?;");			// 查找是否已经存在同名钢束
		query.bindValue(0, steelName);
		if (query.exec()) {
			while (query.next()) {
				if (query.value(0) == steelName) {
					popWarningDialog("数据警告", QString("钢束特性名称%1已存在").arg(steelName));
					return;
				}
			}
		}
		query.prepare("INSERT INTO steel_property VALUES (?,?,?,?,?,?,?,?,?,?,?,?);");
		query.bindValue(0, steelName);
		query.bindValue(1, steelType);
		query.bindValue(2, material);
		query.bindValue(3, steelArea);
		query.bindValue(4, cubeDiameter);
		query.bindValue(5, relaxCoeff);
		query.bindValue(6, fpk);
		query.bindValue(7, frictionCoeff);
		query.bindValue(8, deviation);
		query.bindValue(9, startCompress);
		query.bindValue(10, finalCompress);
		query.bindValue(11, bondType);
		query.exec();
	}

	db->close();
	close();
	emit steelListChanged();		// 发射钢束列表变化的信号
}

PropertyDialog::~PropertyDialog()
{

}
