/*
	F256 calculator. Created once on startup to create tables for all calculations using BinaryPolynomial.
*/
#pragma once
#include "BinaryPolynomial.h"
#include <string>
typedef unsigned char uint8;
typedef unsigned short uint16;
class F256calculator
{
public:
	F256calculator(); 
	// BinaryPolynomial generator is x mod (binary form of 285)

	// lookups
	uint8 add(uint8 a,uint8 b) const;
	uint8 multiply(uint8 a, uint8 b) const;
	uint8 pow(uint8 a, int exp) const;
	uint8 invert(uint8 a) const;
	uint8 divide(uint8 a, uint8 b) const;
	uint8 log(uint8 a) const;

	string toString() const;
private:
	// addition table is optional, it's just XOR
	uint8 multiplyTable[256][256]; 
	uint8 generatorPowerTable[255];	// generator ^ [index]
	uint8 inverseTable[256]; // index * val = 1 (zero is empty, it has no inverse)
	uint8 inverseGeneratorPowerTable[256]; // log_generator (index)
	uint8 powerTable[256][255]; // a to the power of b, ^256 excluded bc its always 1

	// conversion
	BinaryPolynomial createPolynomial(uint16 in);
	uint8 createF256(BinaryPolynomial in);
};

