#pragma once

namespace Optic {

class OpticBezier {
	double x1, y1, x2, y2;
	double getTForX(double aX);
	double getSlope(double aT, double aA1, double aA2);
	double A(double aA1, double aA2);
	double B(double aA1, double aA2);
	double C(double aA1);
	double calculate(double aT, double aA1, double aA2);

public:
	OpticBezier(double x1, double y1, double x2, double y2);
	double get(double t);
};

}