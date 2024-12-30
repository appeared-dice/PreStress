#include "InternalData.h"

namespace Data {
	const std::vector<std::pair<QString, MaterialProperties>>materialPropertyVector = {
		{"Strand1720",{1.95e5, 0.3, 1.2e-5, 7.85e-5}},
		{"Strand1860",{1.95e5, 0.3, 1.2e-5, 7.85e-5}},
		{"Strand1960",{1.95e5, 0.3, 1.2e-5, 7.85e-5}},
		{"Wire1470",{2.05e5, 0.3, 1.2e-5, 7.85e-5}},
		{"Wire1570",{2.05e5, 0.3, 1.2e-5, 7.85e-5}},
		{"Wire1770",{2.05e5, 0.3, 1.2e-5, 7.85e-5}},
		{"Wire1860",{2.05e5, 0.3, 1.2e-5, 7.85e-5}},
		{"Steelbar785",{2e5, 0.3, 1.2e-5, 7.85e-5}},
		{"Steelbar930",{2e5, 0.3, 1.2e-5, 7.85e-5}},
		{"Steelbar1080",{2e5, 0.3, 1.2e-5, 7.85e-5}},
	};

	const std::vector<std::pair<QString, std::vector<std::pair<QString, double>>>>steelAreaVector = {
		{"Strand", {{"Strand 9.5mm(1x7)", 54.8}, {"Strand 12.7mm(1x7)", 98.7}, {"Strand 15.2mm(1x7)", 139}, {"Strand 17.8mm(1x7)", 191}, {"Strand 21.6mm(1x7)", 285}}},
		{"Wire", {{"Wire 5mm", 19.63}, {"Wire 7mm", 38.48}, {"Wire 9mm", 63.62}}},
		{"Steelbar", {{"Steelbar 18mm", 254.3}, {"Steelbar 25mm", 490.9}, {"Steelbar 32mm", 804.2}, {"Steelbar 40mm", 1256.6}, {"Steelbar 50mm", 1963.5}}}
	};

	const std::vector<double>relaxCoeffVector = { 1.0, 0.3 };

	const std::vector<QString>steelTypeVector = { "内部（先张）","内部（后张）","外部" };
}