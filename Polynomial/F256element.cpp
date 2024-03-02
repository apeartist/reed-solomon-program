#include "F256element.h"

F256element::F256element() {
	value = 0;
	nan = false;
}

F256element::F256element(uint8 val) {
	value = val;
	nan = false;
}

uint8 F256element::get_value() const {
	return value;
}

F256element F256element::operator+(const F256element& summand) const
{
	return F256element(calc.add(value, summand.value));
}

void F256element::operator+=(const F256element& summand) {
	value = calc.add(value, summand.value);
}

F256element F256element::operator-(const F256element& subtrahend) const {
	return F256element(calc.add(value, subtrahend.value));
}

void F256element::operator-=(const F256element& subtrahend) {
	value = calc.add(value, subtrahend.value);
}

F256element F256element::operator*(const F256element& multiplicand) const {
	return F256element(calc.multiply(value, multiplicand.value));
}

void F256element::operator*=(const F256element& multiplicand) {
	value = calc.multiply(value, multiplicand.value);
}

F256element F256element::operator/(const F256element& dividend) const {
	F256element out = F256element(0);
	if (dividend.value == 0) {
		out.nan = true;
	}
	else {
		out.value = calc.divide(value, dividend.value);
	}
	return out;
}

void F256element::operator=(const F256element& assignend) {
	value = assignend.value;
	nan = assignend.nan;
}

void F256element::operator=(const uint8 assignend) {
	value = assignend;
	nan = false;
}

bool F256element::operator==(const F256element& el_b) const {
	return value == el_b.value;
}

bool F256element::operator==(int fint) const {
	return value == fint;
}

bool F256element::operator!=(const F256element& el_b) const {
	return value != el_b.value;
}

bool F256element::operator!=(int fint) const {
	return value != fint;
}

F256element F256element::inverse() const {
	F256element out = F256element(0);
	if (value == 0) {
		out.nan = true;
	}
	else {
		out.value = calc.invert(value);
	}
	return out;
}

F256element F256element::pow(int exp) const {
	return F256element(calc.pow(value,exp));
}

uint F256element::log() const {
	return calc.log(value);
}

bool F256element::is_zero() const {
	return value == 0;
}

bool F256element::is_nan() const {
	return nan;
}

char F256element::as_char() const {
	return (char)value;
}

std::string F256element::as_string() const {
	return std::to_string(value);
}


const F256calculator F256element::calc = F256calculator();