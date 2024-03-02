#include "F256Polynomial.h"

// CONSTRUCTORS

F256Polynomial::F256Polynomial() {
	degree = -1;
}

F256Polynomial::F256Polynomial(int size) {
	degree = -1;
	for (int i = 0; i < size; i++) {
		addNewCoeff(0);
	}
}

F256Polynomial::F256Polynomial(const vector<F256element>& arr) {
	for (F256element v : arr) {
		addNewCoeff(v);
	}
}

// DEGREE

int F256Polynomial::getDegree() const {
	return degree;
}

void F256Polynomial::calculateDegree() {
	// finds last coeff (highest power) that is nonzero
	for (int i = 0; i < coeff.size(); i++) {
		if (!coeff[i].is_zero()) degree = i;
	}
}

// MATH

F256Polynomial F256Polynomial::copy() const {
	// copies to a new F256Polynomial, is returned
	F256Polynomial out = F256Polynomial();
	for (int i = 0; i <= degree; i++) {
		out.addNewCoeff(coeff[i]);
	}
	return out;
}

F256Polynomial F256Polynomial::add_c(const F256Polynomial& other) const // constant, returns sum 
{
	F256Polynomial out = F256Polynomial();
	int otherDegree = other.getDegree();
	for (int i = 0; i <= min(otherDegree, degree); i++) {
		out.addNewCoeff(other.getCoeff(i)+coeff[i]);
	}
	if (otherDegree > degree) {
		for (int i = degree + 1; i <= otherDegree; i++) {
			out.addNewCoeff(other.getCoeff(i));
		}
	}
	else {
		for (int i = otherDegree + 1; i <= degree; i++) {
			out.addNewCoeff(coeff[i]);
		}
	}
	out.deleteLeadingZeroes();
	return out;
}

void F256Polynomial::add_r(const F256Polynomial& other) { // replaces coeffs
	for (int i = 0; i <= min(degree, other.getDegree()); i++) {
		coeff[i] = other.getCoeff(i) + coeff[i];
	}
	if (other.getDegree() > degree) {
		for (int i = degree + 1; i <= other.getDegree(); i++) {
			addNewCoeff(other.getCoeff(i));
		}
	}
	deleteLeadingZeroes();
}

F256Polynomial F256Polynomial::multiply_x(int power) const {
	// multiply by a power of x. must be >0
	F256Polynomial out = F256Polynomial();
	for (int i = 0; i < power; i++) {
		out.addNewCoeff(0); // fill with zeros 
	}
	for (int i = 0; i <= degree; i++) {
		out.addNewCoeff(coeff[i]);
	}
	return out;
}

F256Polynomial F256Polynomial::multiply_const(const F256element& val) const {
	// multiply all coeffs by a constant
	F256Polynomial out = F256Polynomial();
	for (int i = 0; i <= degree; i++) {
		out.addNewCoeff(coeff[i] * val);
	}
	return out;
}


F256Polynomial F256Polynomial::multiply(const F256Polynomial& other) {
	// multiply by another polynomial
	F256Polynomial out = F256Polynomial();
	for (int i = 0; i <= degree; i++) {
		F256Polynomial add = other.multiply_x(i);
		add = add.multiply_const(coeff[i]);
		out.add_r(add);
	}
	return out;
}

F256Polynomial F256Polynomial::mod(const F256Polynomial& other) {
	// modulus for another F256Polynomial
	F256Polynomial out = copy();
	int otherDegree = other.getDegree();
	for (int i = degree; i >= otherDegree; i--) {
		if (!out.getCoeff(i).is_zero()) {
			F256element factor = out.getCoeff(i) / other.getCoeff(otherDegree);
			F256Polynomial scaled = other.multiply_const(factor);
			out.add_r(scaled.multiply_x(i-otherDegree));
		}
	}
	return out;
}

// COEFFICIENT HELPER FUNCTIONS

void F256Polynomial::setCoeff(int index, F256element val) {
	while (index >= coeff.size()) {
		coeff.push_back(0);
	}
	coeff[index] = val;
	deleteLeadingZeroes();
}

void F256Polynomial::setCoeff(int index, uint8 val) {
	while (index >= coeff.size()) {
		coeff.push_back(0);
	}
	coeff[index] = F256element(val);
	deleteLeadingZeroes();
}

F256element F256Polynomial::getCoeff(int index) const {
	if (index >= coeff.size()) {
		return false;
	}
	return coeff[index];
}

void F256Polynomial::addNewCoeff(F256element val) {
	coeff.push_back(val);
	if (!val.is_zero()) degree = coeff.size() - 1;
}

void F256Polynomial::addNewCoeff(uint8 val) {
	coeff.push_back(F256element(val));
	if (val!=0) degree = coeff.size() - 1;
}

// OUTPUT

string F256Polynomial::toString() const {
	// prints this as a polynomial (5x^2, etc)
	if (degree == -1) return "0";
	string out = "";
	for (int i = degree; i >= 0; i--) {
		out += coeff[i].as_string()+"x^"+to_string(i)+" ";
	}
	return out;
}

string F256Polynomial::translated() const {
	// convert into chars and return a readable string
	string out = "";
	for (int i = coeff.size()-1; i >= 0; i--) {
		out += coeff[i].as_char();
	}
	return out;
}

// UTILITY

void F256Polynomial::deleteLeadingZeroes() {
	// strips zeroes
	while (coeff.size()!=0 && coeff[coeff.size() - 1] == 0) {
		coeff.pop_back();
	}
	degree = coeff.size() - 1;
}

F256element F256Polynomial::evaluate(F256element x) const {
	// solves polynomial for a given x
	F256element out = F256element(0);
	for (int i = 0; i <= degree; i++) {
		out += coeff[i] * x.pow(i);
	}
	return out;
}