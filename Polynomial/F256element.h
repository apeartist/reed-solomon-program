/*
	Element in F256Polynomial; handles all of its operators.
*/
#pragma once
#include "F256calculator.h"
typedef unsigned char uint8;
typedef unsigned int uint;
class F256element
{
public:
	F256element();
	F256element(uint8 val = 0);

	uint8 get_value() const;

	// operators
	F256element operator+(const F256element& summand) const;
	void operator+=(const F256element& summand);
	F256element operator-(const F256element& subtrahend) const;
	void operator-=(const F256element& subtrahend);
	F256element operator*(const F256element& multiplicand) const;
	void operator*=(const F256element& multiplicand);
	F256element operator/(const F256element& dividend) const;

	// equality
	void operator=(const F256element& assignend);
	void operator=(const uint8 assignend);
	bool operator==(const F256element& el_b) const;
	bool operator==(int fint) const;
	bool operator!=(const F256element& el_b) const;
	bool operator!=(int fint) const;

	F256element inverse() const;
	F256element pow(int exp) const;
	uint log() const;

	bool is_zero() const;
	bool is_nan() const;
	char as_char() const;
	std::string as_string() const;

private:
	uint8 value;
	bool nan; // not a number (ie divide by 0)
	static const F256calculator calc; // use one instance of F256calculator across all instances

};

