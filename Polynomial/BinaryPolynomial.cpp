#include "BinaryPolynomial.h"

BinaryPolynomial::BinaryPolynomial() {
	// initialize blank BinaryPolynomial
	degree = -1;
}

BinaryPolynomial::BinaryPolynomial(int size) {
	// initalizie empty (coefficients are all 0)
	degree = -1;
	for (int i = 0; i < size; i++) {
		addNewCoeff(false);
	}
}

BinaryPolynomial::BinaryPolynomial(const vector<bool>& arr) {
	// initialize with vector of coefficients
	for (bool v : arr) {
		addNewCoeff(v);
	}
}

void BinaryPolynomial::calculateDegree() {
	// sets degree to position of last coeff of 1. maxDegree grows with degree
	for (int i = 0; i < coeff.size(); i++) {
		if (coeff[i]) degree = i;
	}
}

int BinaryPolynomial::getDegree() const {
	return degree;
}

BinaryPolynomial BinaryPolynomial::copy() const {
	// copies this polynomial
	BinaryPolynomial out = BinaryPolynomial();
	for (int i = 0; i <= degree; i++) {
		out.addNewCoeff(coeff[i]);
	}
	return out;
}

BinaryPolynomial BinaryPolynomial::add_c(const BinaryPolynomial& other) const {
	// adds two BinaryPolynomials and returns the output. no carry, and addition is the same as subtraction (XOR)

	BinaryPolynomial out = BinaryPolynomial();
	int otherDegree = other.getDegree();
	for (int i = 0; i <= min(otherDegree, degree); i++) {
		out.addNewCoeff(other.getCoeff(i) ^ coeff[i]);
	}

	// checks if either is longer and adds it on
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

	// strip and return
	out.deleteLeadingZeroes();
	return out;
}

void BinaryPolynomial::add_r(const BinaryPolynomial& other) {
	// adds to this BinaryPolynomial, same base logic as add_c
	for (int i = 0; i <= min(degree,other.getDegree()); i++) {
		coeff[i] = other.getCoeff(i) ^ coeff[i];
	}
	if (other.getDegree() > degree) {
		for (int i = degree + 1; i <= other.getDegree(); i++) {
			addNewCoeff(other.getCoeff(i));
		}
	}
	deleteLeadingZeroes();
}

BinaryPolynomial BinaryPolynomial::multiply_x(int power) const {
	// multiply by a power of x, shifting the polynomial over x coefficients
	BinaryPolynomial out = BinaryPolynomial();
	for (int i = 0; i < power; i++) {
		out.addNewCoeff(0);
	}
	for (int i = 0; i <= degree; i++) {
		out.addNewCoeff(coeff[i]);
	}
	return out;
}

BinaryPolynomial BinaryPolynomial::multiply(const BinaryPolynomial& other) {
	// multiplies two BinaryPolynomials
	BinaryPolynomial out = BinaryPolynomial();

	for (int i = 0; i <= degree; i++) {
		// add other multiplied by the shifted number of x, only if the coeff is one
		if (coeff[i]) {
			out.add_r(other.multiply_x(i));
		}
	}
	return out;
}

BinaryPolynomial BinaryPolynomial::mod(const BinaryPolynomial& other) {
	// modulo function. adding is the same as subtracting
	// "subtracts" if the current coeff is 1 until irreducible, then returns

	BinaryPolynomial out = copy();
	for (int i = degree; i >= other.getDegree(); i--) {
		if (out.getCoeff(i)) {
			out.add_r(other.multiply_x(i-other.getDegree()));
		}
	}
	return out;
}

// UTILITY

void BinaryPolynomial::setCoeff(int index, bool val) {
	coeff[index] = val;
	deleteLeadingZeroes();
}

bool BinaryPolynomial::getCoeff(int index) const{
	if (index >= coeff.size()) { 
		return false;
	}
	return coeff[index];
}

void BinaryPolynomial::addNewCoeff(bool val) {
	coeff.push_back(val);
	if (val) degree = coeff.size() - 1;
}

string BinaryPolynomial::toString() const {
	string out = "";
	for (int i = degree; i >= 0; i--) {
		if (coeff[i]) {
			out += "1";
		}
		else {
			out += "0";
		}
	}
	return out;
}

void BinaryPolynomial::deleteLeadingZeroes() {
	while (coeff[coeff.size()-1]==false) {
		coeff.pop_back();
	}
	degree = coeff.size() - 1;
}