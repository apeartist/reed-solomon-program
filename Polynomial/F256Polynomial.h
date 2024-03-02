/*
	Polynomial with F256element as coefficients. Used to represent string of uint8's
	IMPORTANT: addition and subtraction are equivalent, because F256element is essentially a bitstring
	it means addition/subtraction is just XOR
*/
#pragma once
#include "F256element.h"
typedef unsigned char uint8;

class F256Polynomial
{
public:
	F256Polynomial();
	F256Polynomial(int size);
	F256Polynomial(const vector<F256element>& arr);

	int getDegree() const; // index of last nonzero element
	void calculateDegree();

	F256Polynomial copy() const;

	F256Polynomial add_c(const F256Polynomial& other) const; // constant, returns sum
	void add_r(const F256Polynomial& other); // replaces coeffs of self

	F256Polynomial multiply_x(int power) const; // multiply by a power of x
	F256Polynomial multiply_const(const F256element& val) const; // multiply all coefficients by a constant
	F256Polynomial multiply(const F256Polynomial& other); // multiply two F256 polynomials

	F256Polynomial mod(const F256Polynomial& other); // mod another polynomial

	void setCoeff(int index, F256element val); // changing coefficients, either setting or appending
	void setCoeff(int index, uint8 val);
	void addNewCoeff(F256element val);
	void addNewCoeff(uint8 val);
	F256element getCoeff(int index) const; // get coeff at index (power)

	string toString() const; // print as a polynomial
	string translated() const; // translate it back into a string

	void deleteLeadingZeroes(); // strip zeroes at the end of list (highest degrees)
	F256element evaluate(F256element x) const; // solve with a certain x
	
private:
	vector<F256element> coeff;
	int degree;
};

