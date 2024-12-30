#pragma once
#include <string>
#include <vector>
//#include <utility>
#include <qstring.h>

namespace Data {
	// 材料特性
	struct MaterialProperties {
		double E;          // 弹性模量
		double miu;        // 泊松比
		double alpha;      // 热膨胀系数
		double unit_weight; // 单位重量
	};

	// 压缩值
	struct CompressionValue {
		double start;
		double end;
	};

	// 钢束特性值
	struct SteelProperties {
		QString name;       // 钢束名称
		QString type;       // 钢束类型
		QString material;       // 材料
		double area;        // 钢束总面积
		double cubeDiameter;        // 导管直径
		double relaxCoeff;      // 钢筋松弛系数
		double fpk;     // 钢筋抗拉强度标准值
		double frictionCoeff;       // 钢筋与管道摩擦系数
		double influenceCoeff;      // 管道每米局部偏差对摩擦的影响系数
		CompressionValue compressionValue;      // 钢筋压缩值
		QString bondType;       // 粘结类型
	};

	// 材料特性关联容器声明
	extern const std::vector<std::pair<QString, MaterialProperties>>materialPropertyVector;
	extern const std::vector<std::pair<QString, std::vector<std::pair<QString, double>>>>steelAreaVector;
	extern const std::vector<double>relaxCoeffVector;
	extern const std::vector<QString>steelTypeVector;
}