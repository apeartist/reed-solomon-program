/*
	Polynomial over field 2 (binary)
	just 1s and 0s, so bools are used instead of ints
	used to generate F256 calculator
*/
#pragma once
#include <vector>
#include <string>
#include <iostream>
using namespace std;
class BinaryPolynomial
{
public:
	BinaryPolynomial();
	BinaryPolynomial(int size); // empty
	BinaryPolynomial(const vector<bool>& arr); // init with array of coefficients

	int getDegree() const;
	void calculateDegree();

	BinaryPolynomial copy() const;

	BinaryPolynomial add_c(const BinaryPolynomial& other) const; // constant, returns sum
	void add_r(const BinaryPolynomial& other); // replaces coeffs
	
	BinaryPolynomial multiply_x(int power) const;
	BinaryPolynomial multiply(const BinaryPolynomial& other);
	BinaryPolynomial mod(const BinaryPolynomial& other);

	void setCoeff(int index, bool val);
	bool getCoeff(int index) const;
	void addNewCoeff(bool val);

	string toString() const;
	void deleteLeadingZeroes();

private:
	vector<bool> coeff;
	int degree;
};

