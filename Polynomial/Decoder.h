#pragma once
#include "F256element.h"
#include "F256Polynomial.h"

class Decoder
{
public:
	
	bool reducedRowEchelon(vector<vector<F256element>>& matrix, vector<F256element>& solutions);

	F256Polynomial generatorPolynomial(const int n, const int k);

	vector<F256element> messageToNumbers(const string message);
	vector<F256element> padMessage(vector<F256element> v, int k);
	F256Polynomial codedMessage(const vector<F256element>& v, const F256Polynomial& generator, const int n, const int k);

	F256Polynomial corruptMessage(const F256Polynomial& message, int errors, int n, int k);

	string decode(F256Polynomial corrupt, int n, int k); // the final function: takes full

};

