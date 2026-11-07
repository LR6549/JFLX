#ifndef CeasarCypherHPP
#define CeasarCypherHPP

#include <climits>   // INT_MAX as the default upper bound
#include <cstdlib>   // abs()
#include <stdexcept>
#include <string>

using namespace std;

namespace ceasarCypher {
    struct CeasarCypher {

        CeasarCypher () {
            
        }

        /*
        * ASCII Codes:
        * 
        * Big Letter Alphabet:
        * - 65 to 90
        * 
        * Small Letter Alphabet:
        * - 97 to 122
        * 
        * Numbers;
        * - 48 to 57
        */
        string encrypt(int key, string message, int floor = 0, int ceil = INT_MAX) {
            string result = "";

            for (char messageChar : message) {
                char encryptedChar = messageChar + key;
                while (encryptedChar > ceil) {
                    encryptedChar -= abs(floor-ceil);
                }
                result += encryptedChar;
            }

            return result;
        }

        /*
        * ASCII Codes:
        * 
        * Big Letter Alphabet:
        * - 65 to 90
        * 
        * Small Letter Alphabet:
        * - 97 to 122
        * 
        * Numbers;
        * - 48 to 57
        */
        string decrypt(int key, string message, int floor = 0, int ceil = INT_MAX) {
            string result = "";

            for (char messageChar : message) {
                char encryptedChar = messageChar - key;
                while (encryptedChar < floor) {
                    encryptedChar += abs(ceil-floor);
                }
                result += encryptedChar;
            }

            return result;
        }
    };
}

#endif