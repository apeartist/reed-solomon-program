// Polynomial.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include "polynomial.h"
#include "BinaryPolynomial.h"
#include "F256calculator.h"
#include "F256Polynomial.h"
#include "Decoder.h"

int f256_poly_test()
{
    /*
    uint8_t  test_coeffs[8] = {0, 21, 45, 101, 112, 149, 187, 233};
    int ti = 0;
    fpoly::F256Polynomial test_poly;
    for (int i = 0; i < 32768; i++)
    {
        test_poly = fpoly::F256Polynomial();
        ti = i;
        while (ti>0)
        {
            test_poly.add_new_coefficient(test_coeffs[ti % 8]);
            ti >>= 3;
        }
        std::cout << test_poly.report() << std::endl;
    }
    return 0;
    */
    uint8_t test_coeffs_a[8] = { 0, 21, 45, 101, 112, 149, 187, 233 };
    F256Polynomial test_poly_a;
    for (int i = 0; i < 8; i++)
    {
        test_poly_a.addNewCoeff(test_coeffs_a[i]);
    }

    uint8_t test_coeffs_b[8] = { 18, 212, 192, 240, 16, 13, 155, 222 };
    F256Polynomial test_poly_b;
    for (int i = 0; i < 8; i++)
    {
        test_poly_b.addNewCoeff(test_coeffs_b[i]);
    }
    F256Polynomial test_poly_c = test_poly_a.multiply(test_poly_b);
    F256Polynomial test_poly_d = test_poly_a.add_c(test_poly_b);

    std::cout << test_poly_a.toString() << std::endl;

    test_poly_a.add_r(test_poly_b);

    std::cout << test_poly_b.toString() << std::endl;
    std::cout << test_poly_c.toString() << std::endl;
    std::cout << test_poly_d.toString() << std::endl;
    std::cout << test_poly_a.toString() << std::endl;

    return 0;
}

vector<F256element> messageToNumbers(string message) {
    vector<F256element> out = vector<F256element>();
    for (char c : message) {
        out.push_back(F256element(c));
    }
    return out;
}

vector<F256element> padMessage(vector<F256element> v, int k) { // to size k
    while (v.size() < k) {
        v.push_back(F256element(0));
    }
    return v;
}

F256Polynomial codedMessage(const vector<F256element>& v,const F256Polynomial& generator,  int n, int k) {
    F256Polynomial out = F256Polynomial();
    for (int i = k - 1; i >= 0; i--) {
        out.addNewCoeff(v[i]);
    }
    out = out.multiply_x(n - k);
    //cout << out.toString() << endl;
    F256Polynomial remainder = out.mod(generator);
    //cout << remainder.toString() << endl;
    out.add_r(remainder);
    return out;
}

F256Polynomial generatorPolynomial(int n, int k) {
    // first element is (x-a^1)
    // last element is (x-a^(n-k))
    F256Polynomial outpoly = F256Polynomial();
    outpoly.addNewCoeff(1);
    F256Polynomial multiply;
    multiply.addNewCoeff(1);
    multiply.addNewCoeff(1);
    F256element alpha = F256element(2);
    for (int i = 1; i <= n - k; i++) {
        multiply.setCoeff(0, alpha.pow(i));
        outpoly = outpoly.multiply(multiply);
    }
    return outpoly;
}

F256Polynomial corruptMessage(const F256Polynomial& message, int errors, int n, int k) {
    int errorNum = min((n - k) / 2, errors); // might not actually be all errors if you get the same #
    F256Polynomial out = message.copy();
    int cur = 17; // "random" generator, this is the seed
    int errorSpot;
    for (int i = 0; i < errorNum; i++) {
        cur = ((75 * cur) + 74) % ((1 << 16) + 1);
        errorSpot = cur%n;
        cur = ((75 * cur) + 74) % ((1 << 16) + 1);
        out.setCoeff(errorSpot, F256element(cur%256));
    }
    return out;
}

string decode(vector<F256element> message, int n, int k) {
    
    // https://en.wikipedia.org/wiki/Reed%E2%80%93Solomon_error_correction#Peterson%E2%80%93Gorenstein%E2%80%93Zierler_decoder 

    F256Polynomial corrupt = F256Polynomial(message);
    // decode from string
    /*vector<F256element> messageInNums = messageToNumbers(message);
    F256Polynomial corrupt = F256Polynomial();
    for (int i = k - 1; i >= 0; i--) {
        corrupt.addNewCoeff(messageInNums[i]);
    }*/

    Decoder decoder = Decoder();

    vector<F256element> xCoords = vector<F256element>(); // list of points to evaluate polynomial at
    F256element alpha = F256element(2); // generator
    for (int i = 1; i <= n - k; i++) {
        xCoords.push_back(alpha.pow(i));
    }

    vector<F256element> syndromes = vector<F256element>(); // the y values at generator's xCoords
    for (F256element x : xCoords) {
        syndromes.push_back(corrupt.evaluate(x));
    }

    int v = (n - k) / 2; // max number of errors

    // decoding time
    // find coeffs of error location polynomial (lambda i)

    vector<F256element> lambdas = vector<F256element>(); // coefficients of error locator polynomial
    bool found = false; // found the number of errors?
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
        found = decoder.reducedRowEchelon(matrix, lambdas); // returns true if the number of errors is found
    }
    if (!found) { cout << "no errors?" << endl; } // if found is still false, it went to the end without errors.
    else {
        F256Polynomial errorLocatorPolynomial = F256Polynomial(); // using lambda as coefficients
        errorLocatorPolynomial.addNewCoeff(1);
        for (int i = lambdas.size() - 1; i >= 0; i--) {
            errorLocatorPolynomial.addNewCoeff(lambdas[i]);
        }
        vector<F256element> errorLocators = vector<F256element>(); // since 256 is small, just iterate and find what evals to 0
        for (int i = 0; i < 256; i++) {
            if (errorLocatorPolynomial.evaluate(F256element(i)) == 0) {
                errorLocators.push_back(F256element(i).inverse());
            }
        }

        int v = errorLocators.size(); // number of roots
        vector<F256element> errorValues = vector<F256element>();
        vector<vector<F256element>> matrix = vector<vector<F256element>>();
        for (int i = 0; i < v; i++) {
            matrix.push_back(vector<F256element>());
            errorValues.push_back(syndromes[i]);
            for (int j = 0; j < v; j++) {
                matrix[i].push_back(errorLocators[j].pow(i + 1));
            }
        }

        decoder.reducedRowEchelon(matrix, errorValues);

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

int main()
{
    /*
    actual encoding
    x_i = 2^i (2 corresponds to x in F256 which is a generator)

    Hi, this is a message, right?! >:D It's not ;(? It is XDXDXD!!314159**&# k let's go.╗CÄµqÆΘm/.╫╦¿╔■¶ΩN¥╠\r♀️∟ë $é$"☼&╒q+{w▐R±╘íAÇ√bU▒↕️╧#☻→N▌┐☻]╗╧▓╒6 ^æ╧┘\⌠☺️lN;♥️cΩ╕q*y┴c½O;ô!ÿmV╜░╕↕️nßÅ>
99
244
Hi, t⌂is is a message, right?! >:D It's not ;(? It is XDXDXD!!314159**&# k let's go.ÇCÄµqÆΘm/.╫╦¿╔■¶ΩN¥╠\r♀️∟ë $é$"☼&╒q+{w▐R±╘íAÇ√bU▒↕️╧#☻→N▌┐☻]╗╧▓╒6 ^æ╧┘\⌠☺️lN;♥️cΩ╕q*y┴c½O;ô!ÿmV╜░╕↕️nßÅ>
Original poly:
Hi, this is a message, right?! >:D It's not ;(? It is XDXDXD!!314159**&# k let's go.╗CÄµqÆΘm/.╫╦¿╔■¶ΩN¥╠\r♀️∟ë $é$"☼&╒q+{w▐R±╘íAÇ√bU▒↕️╧#☻→N▌┐☻]╗╧▓╒6 ^æ╧┘\⌠☺️lN;♥️cΩ╕q*y┴c½O;ô!ÿmV╜░╕↕️nßÅ>
Decoded poly:
Hi, this is a message, right?! >:D It's not ;(? It is XDXDXD!!314159**&# k let's go.╗CÄµqÆΘm/.╫╦¿╔■¶ΩN¥╠\r♀️∟ë $é$"☼&╒q+{w▐R±╘íAÇ√bU▒↕️╧#☻→N▌┐☻]╗╧▓╒6 ^æ╧┘\⌠☺️lN;♥️cΩ╕q*y┴c½O;ô!ÿmV╜░╕↕️nßÅ>
Same = 1 --
Original message:
Hi, this is a message, right?! >:D It's not ;(? It is XDXDXD!!314159**&# k let's go.
Decoded message:
Hi, this is a message, right?! >:D It's not ;(? It is XDXDXD!!314159**&# k let's go.
    */

    

    
    string message = "Hi, this is a message, right?! >:D It's not ;(? It is XDXDXD!!314159**&# k let's go.";
    int n = 250; // size of code word (extended)
    int k = 150; // size of message
    
    F256Polynomial generator = generatorPolynomial(n, k);
    vector<F256element> numericalMessage = messageToNumbers(message);
    vector<F256element> padded = padMessage(numericalMessage, k); 
    F256Polynomial coded = codedMessage(padded, generator, n, k);
    F256Polynomial corrupt = corruptMessage(coded, 10, n, k);
    

    /*
    vector<int> ints = { 9, 160, 116, 69, 185, 237, 57, 146, 69, 16, 147, 190, 122, 220, 184, 15, 211, 111, 178, 4, 8, 146, 248, 51, 150, 59, 180, 93, 174, 63, 37, 85, 87, 41, 40, 92, 63, 184, 29, 160, 10, 177, 186, 121, 128, 242, 39, 182, 112, 171, 89, 187, 211, 117, 12, 67, 124, 77, 13, 184, 211, 188, 145, 99, 1, 163, 220, 80, 102, 158, 148, 80, 10, 10, 220, 127, 169, 216, 36, 197, 143, 110, 149, 125, 161, 18, 155, 129, 170, 174, 118, 138, 193, 253, 110, 86, 42, 106, 93, 153, 0, 0, 83, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 101, 114, 111, 108, 32, 110, 101, 116, 116, 111, 103, 114, 111, 102, 32, 102, 111, 32, 101, 109, 117, 108, 111, 118, 32, 115, 117, 111, 105, 114, 222, 99, 32, 100, 110, 97, 32, 116, 110, 105, 97, 117, 113, 32, 97, 32, 121, 110, 97, 109, 32, 114, 101, 118, 79, 32, 44, 121, 114, 97, 30, 119, 32, 100, 110, 97, 32, 107, 97, 101, 119, 32, 44, 100, 101, 114, 101, 100, 15, 111, 112, 32, 73, 32, 101, 108, 105, 104, 119, 32, 44, 121, 114, 97, 101, 114, 100, 32, 116, 104, 103, 105, 110, 100, 105, 109, 32, 97, 32, 110, 111, 112, 117, 32, 101, 99, 110, 79 };
    vector<F256element> in = vector<F256element>();
    for (int i : ints) {
        in.push_back(F256element(i));
    }
    */


    //string message = "Camp fake a midnight dreary, ahead a wandered, meek and teary, Over many a quaint and furious volume on forgotten careÖ]j*Vn²┴èv«¬ü¢↕️í}ònÅ┼$╪⌐⌂▄";
    //message = "Hi, t⌂is is a message, right?! >:D It's 'ot ;(? It is XDXDXD!!314159**&# k let's go.`É∟zMσÇCÄµqÆΘm/.╫╦¿╔■¶ΩN¥╠\r♀️∟ë $é$☼&╒q+☼w▐R±╘íAÇ√bU▒↕️╧#☻→N▌┐☻]╗╧▓╒6 ^æ╧┘\⌠☺️lN;♥️cΩ╕q*y┴c½O;ô!ÿmV╜░╕↕️nßÅ>";
    cout << decode(messageToNumbers(corrupt.translated()), n, k) << endl;
    

    /*
        notes
        the message is coeffiecients of the first part of polynomial
        second part is determined by making it divisible by the generator polynomial
    */

    return 0;

}

