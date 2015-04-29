#include "OpticBezier.h"

namespace Optic {

OpticBezier::OpticBezier(double x1, double y1, double x2, double y2) {
	this->x1 = x1;
	this->x2 = x2;
	this->y1 = y1;
	this->y2 = y2;
}

double OpticBezier::get(double t) {
	if (x1 == y1 && x2 == y2) {
		return t;
	}

	return calculate(getTForX(t), y1, y2);
}

double OpticBezier::getTForX(double aX) {
	double aGuessT = aX;

	for (int i = 0; i < 4; i++) {
		double currentSlope = getSlope(aGuessT, x1, x2);
		
		if(currentSlope == 0.0) {
			return aGuessT;
		}
		
		double currentX = calculate(aGuessT, x1, x2) - aX;
		aGuessT -= currentX / currentSlope;
	}

	return aGuessT;
}

double OpticBezier::A(double aA1, double aA2) {
	return 1.0f - 3.0 * aA2 + 3.0 * aA1; 
}

double OpticBezier::B(double aA1, double aA2) {
	return 3.0 * aA2 - 6.0 * aA1;
}

double OpticBezier::C(double aA1) {
	return 3.0 * aA1; 
}

double OpticBezier::getSlope(double aT, double aA1, double aA2) {
	return 3.0 * A(aA1, aA2) * aT * aT + 2.0 * B(aA1, aA2) * aT + C(aA1);
}

double OpticBezier::calculate(double aT, double aA1, double aA2) {
	return ((A(aA1, aA2) * aT + B(aA1, aA2)) * aT + C(aA1)) * aT;
}

};