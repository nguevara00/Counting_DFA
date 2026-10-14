#include <vector>
#include <string>
#include <cassert>
#include <iostream>
#include <gmpxx.h>

struct DFA {
    int numStates;
    int alphabetSize;
    std::vector<std::vector<int>> transition;
    int startState = 0;
    std::vector<bool> accepting;
};

//
int encodeState(const std::string& buffer) {
    int offset = 0;
    int power = 1;

    for (std::size_t i = 0; i < buffer.size(); i++) {
        offset += power;
        power *= 4;
    }

    int value = 0;

    for (std::size_t i = 0; i < buffer.size(); i++) {
        value = value * 4 + (buffer[i] - 'a');
    }

    return offset + value;
}

std::string decodeState(int stateId) {
    //should never accept input 1365, handle as a special case
    int length = 0;
    int offset = 0;
    int numStates = 1;

    while (stateId >= offset + numStates) {
        offset += numStates;
        numStates *= 4;
        length++;
    }

    int value = stateId - offset;

    std::string buffer(length, 'a');

    for (int i = length - 1; i >= 0; i--) {
        buffer[i] = 'a' + (value % 4);
        value /= 4;
    }

    return buffer;
}

bool isValidSix(const std::string& str) {
    bool found[4] = {false, false, false, false};

    for (int i = 0; i < 6; i++) {
        found[str[i] - 'a'] = true;
    }

    return found[0] && found[1] && found[2] && found[3];
}

DFA buildDFA_L() {
    DFA dfa;

    dfa.numStates = 1366;
    dfa.alphabetSize = 4;
    dfa.startState = 0;

    dfa.transition.resize(dfa.numStates,
                          std::vector<int>(dfa.alphabetSize));

    dfa.accepting.resize(dfa.numStates, true);

    int failState = 1365;
    dfa.accepting[failState] = false;

    for (int state = 0; state < failState; state++) {
        std::string buffer = decodeState(state);

        for (int symbol = 0; symbol < dfa.alphabetSize; symbol++) {
            char letter = 'a' + symbol;
            std::string updated = buffer + letter;

            if (updated.size() < 6) {
                dfa.transition[state][symbol] = encodeState(updated);
            }
            else if (isValidSix(updated)) {
                dfa.transition[state][symbol] =
                    encodeState(updated.substr(1));
            }
            else {
                dfa.transition[state][symbol] = failState;
            }
        }
    }

    for (int symbol = 0; symbol < dfa.alphabetSize; symbol++) {
        dfa.transition[failState][symbol] = failState;
    }

    return dfa;
}

//just think of mpz class as infinite integer
mpz_class countAccepted(const DFA& dfa, int n) {
    std::vector<mpz_class> prev(dfa.numStates);
    std::vector<mpz_class> next(dfa.numStates);

    for (int j = 0; j < dfa.numStates; j++) {
        if (dfa.accepting[j]) {
            prev[j] = 1;
        }
        else {
            prev[j] = 0;
        }
    }

    for (int k = 1; k <= n; k++) {
        for (int j = 0; j < dfa.numStates; j++) {
            next[j] = 0;

            for (int x = 0; x < dfa.alphabetSize; x++) {
                int destination = dfa.transition[j][x];
                next[j] += prev[destination];
            }
        }

        prev = next;
    }

    return prev[0];
}
//temporary helper function for testing
bool acceptsString(const DFA& dfa, const std::string& str) {
    int state = dfa.startState;

    for (std::size_t i = 0; i < str.size(); i++) {
        int symbol = str[i] - 'a';
        state = dfa.transition[state][symbol];
    }

    return dfa.accepting[state];
}

int main(){
    DFA dfa = buildDFA_L();
    /*
    assert(dfa.numStates == 1366);
    assert(dfa.alphabetSize == 4);
    assert(dfa.startState == 0);

    assert(dfa.transition[0][0] == encodeState("a"));

    assert(dfa.transition[encodeState("abcd")][0] ==
        encodeState("abcda"));

    assert(dfa.transition[encodeState("abcda")][1] ==
        encodeState("bcdab"));

    assert(dfa.transition[encodeState("aaaaa")][0] == 1365);

    for (int symbol = 0; symbol < 4; symbol++) {
        assert(dfa.transition[1365][symbol] == 1365);
    }

    assert(dfa.accepting[0] == true);
    assert(dfa.accepting[1365] == false);

    std::cout << "DFA L tests passed." << std::endl;
    */

unsigned long expected[] = {
    1, 4, 16, 64, 256, 1024,
    1560, 4440, 12072, 31584, 81552
};

for (int n = 0; n <= 10; n++) {
    mpz_class result = countAccepted(dfa, n);

    assert(result == expected[n]);
}

assert(acceptsString(dfa, ""));
assert(acceptsString(dfa, "aaaaa"));
assert(!acceptsString(dfa, "aaaaaa"));

assert(acceptsString(dfa, "abcdbc"));
assert(acceptsString(dfa, "abcdabc"));
assert(!acceptsString(dfa, "abcdbcd"));

assert(acceptsString(dfa, "abcdabcd"));

mpz_class result = countAccepted(dfa, 300);

mpz_class upperBound = 1;

for (int i = 0; i < 300; i++) {
    upperBound *= 4;
}

assert(result >= 4);
assert(result <= upperBound);

std::cout << "Length 300 test passed." << std::endl;
std::cout << "Accepted strings: " << result << std::endl;

std::cout << "String acceptance tests passed." << std::endl;

std::cout << "All counting tests passed." << std::endl;
}