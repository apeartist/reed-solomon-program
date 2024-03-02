#include "Decoder.h"

bool Decoder::reducedRowEchelon(vector<vector<F256element>>& matrix, vector<F256element>& solutions) {
//vector<F256element> reducedRowEchelon(vector<vector<F256element>> matrix) {
	// takes in a square matrix and a column of solutions
	// perform reduced row echelon. changes solutions, returns false if the matrix is unsolvable
	bool hasSolution = true;
	vector<F256element> hold; // hold a row
	F256element holdF256 = F256element(0);
	int N = matrix.size();

	for (int i = 0; i < N-1; i++) {
		// simplify
		F256element first = matrix[i][i];
		if (first == 0) {
			for (int j = i + 1; j < N; j++) {
				if (matrix[j][i] != 0) {
					hold = matrix[j];
					matrix[j] = matrix[i];
					matrix[i] = hold;
					holdF256 = solutions[j];
					solutions[j] = solutions[i];
					solutions[i] = holdF256;
					first = matrix[i][i];
					break;
				}
			}
			if (first == 0) {
				return false;
			}
		}

		for (int j = 0; j < N; j++) {
			matrix[i][j] = matrix[i][j] / first;
		}
		solutions[i] = solutions[i] / first;

		// subtract from rest

		for (int j = i+1; j < N; j++) {
			first = matrix[j][i];
			solutions[j] -= first * solutions[i];
			for (int k = 0; k < N; k++) {
				matrix[j][k] -= first*matrix[i][k]; 
			}
		}
	}

	cout << "." << endl;
	if (matrix[N - 1][N - 1] == 0) { return false; }
	solutions[N-1] = solutions[N-1] / matrix[N-1][N-1];
	matrix[N - 1][N - 1] = 1;

	// simplify
	for (int i = matrix.size() - 1; i > 0; i--) {
		for (int j = i - 1; j >= 0; j--) {
			solutions[j] -= matrix[j][i] * solutions[i];
			matrix[j][i] = 0;
		}
	}

	return true;
}

F256Polynomial Decoder::generatorPolynomial(const int n, const int k) {
	// uses lagrange interpolation
	// first element is (x-a^1)
	// last element is (x-a^(n-k))

	F256Polynomial outpoly = F256Polynomial();
	outpoly.addNewCoeff(1); // starts as 1 so multiplication works

	F256Polynomial multiply;
	multiply.addNewCoeff(1);
	multiply.addNewCoeff(1); // x+1

	F256element alpha = F256element(2); // equal to x+0 in BinaryPolynomial
	for (int i = 1; i <= n - k; i++) {
		multiply.setCoeff(0, alpha.pow(i)); // factor of (x+a^i) 
		outpoly = outpoly.multiply(multiply);
	}
	return outpoly;
}


vector<F256element> Decoder::messageToNumbers(const string message) {
	// translates each char's data to an F256 element byte
	vector<F256element> out = vector<F256element>();
	for (char c : message) {
		out.push_back(F256element(c));
	}
	return out;
}

vector<F256element> Decoder::padMessage(vector<F256element> v, int k) {
	// adds zeroes to make it size k to encode later
	while (v.size() < k) {
		v.push_back(F256element(0));
	}
	return v;
}

F256Polynomial Decoder::codedMessage(const vector<F256element>& v, const F256Polynomial& generator, const int n, const int k) {
	// use generator and the list of numbers to make a F256Polynomial that is essentially reversed (first char is highest degree, end of coeffs)
	// lagrange interpolation
	F256Polynomial out = F256Polynomial();
	for (int i = k - 1; i >= 0; i--) {
		out.addNewCoeff(v[i]);
	}
	out = out.multiply_x(n - k);
	
	F256Polynomial remainder = out.mod(generator);
	out.add_r(remainder); // adding the remainder is the same as subtracting, so it ends up being divible by generator
	return out;
}

F256Polynomial Decoder::corruptMessage(const F256Polynomial& message, int errors, int n, int k) {
	// uses a generator and "randomly" changes values
	int errorNum = min((n - k) / 2, errors); // might not actually be all errors if you get the same #
	F256Polynomial out = message.copy();
	int cur = 17; // "random" generator, this is the seed
	int errorSpot;
	for (int i = 0; i < errorNum; i++) {
		cur = ((75 * cur) + 74) % ((1 << 16) + 1);
		errorSpot = cur % n;
		cur = ((75 * cur) + 74) % ((1 << 16) + 1);
		out.setCoeff(errorSpot, F256element(cur % 256));
	}
	return out;
}

string Decoder::decode(string message, int n, int k) {

	// decode from string
	vector<F256element> messageInNums = messageToNumbers(message);
	F256Polynomial corrupt = F256Polynomial();
	for (int i = k - 1; i >= 0; i--) {
		corrupt.addNewCoeff(messageInNums[i]);
	}

	vector<F256element> xCoords = vector<F256element>();
	F256element alpha = F256element(2); // generator
	for (int i = 1; i <= n - k; i++) {
		xCoords.push_back(alpha.pow(i));
	}

	vector<F256element> syndromes = vector<F256element>();
	for (F256element x : xCoords) {
		syndromes.push_back(corrupt.evaluate(x));
	}

	int v = (n - k) / 2; // max number of errors

	// decoding time
	// find coeffs of error location polynomial (lambda i)

	vector<F256element> lambdas = vector<F256element>();
	bool found = false;
	for (int i = v; i > 0 && !found; i--) {
		vector<vector<F256element>> matrix = vector<vector<F256element>>();
		for (int j = 0; j < i; j++) {
			matrix.push_back(vector<F256element>());
			for (int k = 0; k < i; k++) {
				matrix[j].push_back(syndromes[j + k]);
			}
		}
		lambdas = vector<F256element>();
		for (int j = i; j < 2 * i; j++) {
			lambdas.push_back(syndromes[j]);
		}
		found = reducedRowEchelon(matrix, lambdas);
	}
	if (!found) { cout << "no errors?" << endl; }
	else {
		F256Polynomial errorLocatorPolynomial = F256Polynomial();
		errorLocatorPolynomial.addNewCoeff(1);
		for (int i = lambdas.size() - 1; i >= 0; i--) {
			errorLocatorPolynomial.addNewCoeff(lambdas[i]);
		}
		vector<F256element> errorLocators = vector<F256element>();
		for (int i = 0; i < 256; i++) {
			if (errorLocatorPolynomial.evaluate(F256element(i)) == 0) {
				errorLocators.push_back(F256element(i).inverse());
			}
		}

		int v = errorLocators.size();
		vector<F256element> errorValues = vector<F256element>();
		vector<vector<F256element>> matrix = vector<vector<F256element>>();
		for (int i = 0; i < v; i++) {
			matrix.push_back(vector<F256element>());
			errorValues.push_back(syndromes[i]);
			for (int j = 0; j < v; j++) {
				matrix[i].push_back(errorLocators[j].pow(i + 1));
			}
		}

		reducedRowEchelon(matrix, errorValues);

		vector<int> errorLocations = vector<int>();
		for (F256element e : errorLocators) {
			errorLocations.push_back(e.log());
		}

		F256Polynomial errorPolynomial = F256Polynomial();
		for (int i = 0; i < v; i++) {
			errorPolynomial.setCoeff(errorLocations[i], errorValues[i]);
		}

		F256Polynomial corrected = corrupt.add_c(errorPolynomial);
		return corrected.translated();
	}
}