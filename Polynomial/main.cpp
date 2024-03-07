// Polynomial.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include "polynomial.h"
#include "BinaryPolynomial.h"
#include "F256calculator.h"
#include "F256Polynomial.h"
#include "Decoder.h"


int main()
{
    /*
    actual encoding
    x_i = 2^i (2 corresponds to x in F256 which is a generator)
    
    string message = "Hi, this is a message, right?! >:D It's not ;(? It is XDXDXD!!314159**&# k let's go.";
    int n = 188; // size of code word (extended)
    int k = 84; // size of message
    */

    string message;
    while (!message.size()) {
        cout << "Type the message you want to send: ";
        getline(cin, message);
    }

    int k = message.size();
    cout << "Your message is " << k << " characters long." << endl;
    cout << "What length do you want to extend it to? ";

    int n;
    cin >> n;

    Decoder decoder = Decoder();

    F256Polynomial generator = decoder.generatorPolynomial(n, k);
    vector<F256element> numericalMessage = decoder.messageToNumbers(message);
    vector<F256element> padded = decoder.padMessage(numericalMessage, n); 
    F256Polynomial coded = decoder.codedMessage(padded, generator, n, k);

    cout << "Your coded message is " << coded.translated() << endl;

    string response;
    F256Polynomial corrupt;
    int maxErrors = (n - k) / 2;
    while (response != "Y" && response != "N") {
        cout << "Would you like to corrupt the message manually (Y) or automatically? (N)" << endl;
        cin >> response;
    }
    if (response == "Y") {
        string corruptString;
        cout << "The error decoder can handle " << maxErrors << "errors." << endl;
        cout << "Please enter the corrupted message:";
        getline(cin, corruptString);
        corrupt = F256Polynomial(decoder.messageToNumbers(corruptString));
    }
    else {
        srand(time(0));
        corrupt = decoder.corruptMessage(coded, rand() % maxErrors + 1, n, k);
        cout << "The corrupted message is " << corrupt.translated() << endl;
        cout << "There may be missing characters, as sometimes there will be unprintable numbers." << endl;
    }

    /*
    vector<int> ints = { 9, 160, 116, 69, 185, 237, 57, 146, 69, 16, 147, 190, 122, 220, 184, 15, 211, 111, 178, 4, 8, 146, 248, 51, 150, 59, 180, 93, 174, 63, 37, 85, 87, 41, 40, 92, 63, 184, 29, 160, 10, 177, 186, 121, 128, 242, 39, 182, 112, 171, 89, 187, 211, 117, 12, 67, 124, 77, 13, 184, 211, 188, 145, 99, 1, 163, 220, 80, 102, 158, 148, 80, 10, 10, 220, 127, 169, 216, 36, 197, 143, 110, 149, 125, 161, 18, 155, 129, 170, 174, 118, 138, 193, 253, 110, 86, 42, 106, 93, 153, 0, 0, 83, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 101, 114, 111, 108, 32, 110, 101, 116, 116, 111, 103, 114, 111, 102, 32, 102, 111, 32, 101, 109, 117, 108, 111, 118, 32, 115, 117, 111, 105, 114, 222, 99, 32, 100, 110, 97, 32, 116, 110, 105, 97, 117, 113, 32, 97, 32, 121, 110, 97, 109, 32, 114, 101, 118, 79, 32, 44, 121, 114, 97, 30, 119, 32, 100, 110, 97, 32, 107, 97, 101, 119, 32, 44, 100, 101, 114, 101, 100, 15, 111, 112, 32, 73, 32, 101, 108, 105, 104, 119, 32, 44, 121, 114, 97, 101, 114, 100, 32, 116, 104, 103, 105, 110, 100, 105, 109, 32, 97, 32, 110, 111, 112, 117, 32, 101, 99, 110, 79 };
    vector<F256element> in = vector<F256element>();
    for (int i : ints) {
        in.push_back(F256element(i));
    }
    */


    //string message = "Camp fake a midnight dreary, ahead a wandered, meek and teary, Over many a quaint and furious volume on forgotten careÖ]j*Vn²┴èv«¬ü¢↕️í}ònÅ┼$╪⌐⌂▄";
    //message = "Hi, t⌂is is a message, right?! >:D It's 'ot ;(? It is XDXDXD!!314159**&# k let's go.`É∟zMσÇCÄµqÆΘm/.╫╦¿╔■¶ΩN¥╠\r♀️∟ë $é$☼&╒q+☼w▐R±╘íAÇ√bU▒↕️╧#☻→N▌┐☻]╗╧▓╒6 ^æ╧┘\⌠☺️lN;♥️cΩ╕q*y┴c½O;ô!ÿmV╜░╕↕️nßÅ>";
    
    string decoded = decoder.decode(corrupt, n, k);
    cout << "The decoded data is " << decoded << endl;
    cout << "The resulting message is " << decoded.substr(0, k) << endl;
    

    /*
        notes
        the message is coeffiecients of the first part of polynomial
        second part is determined by making it divisible by the generator polynomial
    */

    return 0;

}

