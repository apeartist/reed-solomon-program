#include "F256calculator.h"

F256calculator::F256calculator() {
	// Generate all the tables on start.

	// generator table
	BinaryPolynomial generator = BinaryPolynomial({ false,true }); // the equivalent of "x+0"
	BinaryPolynomial counter = createPolynomial(1);
	BinaryPolynomial mod = createPolynomial(285);
	generatorPowerTable[0] = 1;
	inverseGeneratorPowerTable[0] = 255;
	inverseGeneratorPowerTable[1] = 0;
	for (int i = 1; i < 255; i++) {
		counter = counter.multiply(generator).mod(mod);
		uint8 index = createF256(counter);
		generatorPowerTable[i] = index;
		inverseGeneratorPowerTable[index] = i;
	}

	// multiply table

	// edge cases; dont exist
	multiplyTable[0][0] = 0;
	inverseTable[0] = 0;
	
	// first factor
	for (uint16 i = 1; i < 256; i++) {
		multiplyTable[0][i] = 0;
		multiplyTable[i][0] = 0;
		uint16 p1 = inverseGeneratorPowerTable[i];
		for (uint16 j = 1; j < 256; j++) {
			uint16 p2 = inverseGeneratorPowerTable[j];
			// i = x^p1, j = x^p2
			// i*j = x^p1 * x^p2 = x^(p1+p2)
			multiplyTable[i][j] = generatorPowerTable[(p1 + p2) % 255];
			if (multiplyTable[i][j] == 1) {
				// if it multiplies to 1, i and j are inverses
				// inverseTable[j] will be set on another pass
				inverseTable[i] = j;
			}
		}
	}

	// power table
	for (uint16 i = 0; i < 256; i++) {
		uint8 count = 1;
		for (uint16 j = 0; j < 255; j++) {
			powerTable[i][j] = count; // multiplies count by i every time, so i^j
			count = multiplyTable[count][i];
		}
	}

}

BinaryPolynomial F256calculator::createPolynomial(uint16 in) {
	BinaryPolynomial out = BinaryPolynomial();
	while (in) {
		out.addNewCoeff(in & 1);
		in >>= 1;
	}
	return out;
}

uint8 F256calculator::createF256(BinaryPolynomial in) {
	uint8 out = 0;
	for (int i = in.getDegree(); i >= 0; i--) { 
		out = (out<<1) + in.getCoeff(i);
	}
	return out;
}

uint8 F256calculator::add(uint8 a, uint8 b) const {
	return a ^ b; // under this field, addition doesn't carry. just xor the numbers
}

uint8 F256calculator::multiply(uint8 a, uint8 b) const {
	return multiplyTable[a][b];
}

uint8 F256calculator::pow(uint8 a, int exp) const {
	return powerTable[a][exp%255];
}

uint8 F256calculator::invert(uint8 a) const {
	return inverseTable[a];
}

uint8 F256calculator::divide(uint8 a, uint8 b) const {
	return multiplyTable[a][inverseTable[b]];
}

uint8 F256calculator::log(uint8 a) const {
	return inverseGeneratorPowerTable[a];
}

string F256calculator::toString() const {
	// debugging, prints out all the tables generated

	std::string rep = "Report:\n\n";

	rep += "Inverse table:\n";
	for (int i = 0; i < 256; i++)
	{
		rep += std::to_string(inverseTable[i]) + ", ";
	}
	rep += "\n";

	rep += "Power table column 7:\n";
	for (int i = 0; i < 255; i++)
	{
		rep += std::to_string(pow(7, i)) + ", ";
	}
	rep += "\n";

	rep += "Multiplication table column 137:\n";
	for (int i = 0; i < 256; i++)
	{
		rep += std::to_string(multiply(137, i)) + ", ";
	}
	rep += "\n";

	rep += "addition table column 61:\n";
	for (int i = 0; i < 256; i++)
	{
		rep += std::to_string(add(61, i)) + ", ";
	}
	rep += "\n";

	rep += "log table:\n";
	for (int i = 0; i < 255; i++)
	{
		rep += std::to_string(log(i)) + ", ";
	}
	rep += "\n";


	return rep;
}