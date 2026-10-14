#include <vector>
#include <string>
#include <cassert>
#include <iostream>
#include <gmpxx.h>
#include <limits>
#include <algorithm> // for std::reverse()

const int L2_BUFFER_STATES = 1365;
const int L2_START_STATE = 0;
const int L2_FAIL_STATE = 1 + L2_BUFFER_STATES * L2_BUFFER_STATES;
const int L2_NUM_STATES = L2_FAIL_STATE + 1;

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

int encodeProductState(int leftState, int rightState) {
    return 1 + L2_BUFFER_STATES * leftState + rightState;
}

void decodeProductState(int stateId, int& leftState, int& rightState) {
    int value = stateId - 1;

    leftState = value / L2_BUFFER_STATES;
    rightState = value % L2_BUFFER_STATES;
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

DFA buildDFA_L2(const DFA& dfaL) {
    DFA dfa;

    dfa.numStates = L2_NUM_STATES;
    dfa.alphabetSize = 16;
    dfa.startState = L2_START_STATE;

    dfa.transition.resize(
        dfa.numStates,
        std::vector<int>(dfa.alphabetSize, L2_FAIL_STATE)
    );

    dfa.accepting.resize(dfa.numStates, true);
    dfa.accepting[L2_FAIL_STATE] = false;

    for (int symbol = 0; symbol < 16; symbol++) {
        char leftChar = 'a' + symbol / 4;
        char rightChar = 'a' + symbol % 4;

        std::string leftBuffer = "a";
        std::string rightBuffer = "a";

        leftBuffer += leftChar;
        rightBuffer += rightChar;

        int leftState = encodeState(leftBuffer);
        int rightState = encodeState(rightBuffer);

        dfa.transition[L2_START_STATE][symbol] =
            encodeProductState(leftState, rightState);
    }

    std::vector<std::string> buffers(L2_BUFFER_STATES);

    for (int i = 0; i < L2_BUFFER_STATES; i++) {
        buffers[i] = decodeState(i);
    }

    for (int leftState = 0; leftState < L2_BUFFER_STATES; leftState++) {
        std::string leftBuffer = buffers[leftState];

        for (int rightState = 0; rightState < L2_BUFFER_STATES; rightState++) {
            std::string rightBuffer = buffers[rightState];

            int state = encodeProductState(leftState, rightState);

            if (leftBuffer.size() != rightBuffer.size() ||
                leftBuffer.empty()) {
                dfa.accepting[state] = false;
                continue;
            }

            int length = leftBuffer.size();

            if (length < 5 &&
                (leftBuffer[0] != 'a' || rightBuffer[0] != 'a')) {
                dfa.accepting[state] = false;
                continue;
            }

            for (int symbol = 0; symbol < 16; symbol++) {
                int leftSymbol = symbol / 4;
                int rightSymbol = symbol % 4;

                if (length < 5) {
                    std::string updatedLeft = leftBuffer;
                    std::string updatedRight = rightBuffer;

                    updatedLeft += char('a' + leftSymbol);
                    updatedRight += char('a' + rightSymbol);

                    std::string fullString = updatedLeft;
                    std::reverse(fullString.begin(), fullString.end());
                    fullString += updatedRight;

                    bool valid = true;

                    for (std::size_t i = 0; i + 6 <= fullString.size(); i++) {
                        if (!isValidSix(fullString.substr(i, 6))) {
                            valid = false;
                            break;
                        }
                    }

                    if (valid) {
                        int newLeft = encodeState(updatedLeft);
                        int newRight = encodeState(updatedRight);

                        dfa.transition[state][symbol] =
                            encodeProductState(newLeft, newRight);
                    }
                }
                else {
                    int newLeft = dfaL.transition[leftState][leftSymbol];
                    int newRight = dfaL.transition[rightState][rightSymbol];

                    if (newLeft != 1365 && newRight != 1365) {
                        dfa.transition[state][symbol] =
                            encodeProductState(newLeft, newRight);
                    }
                }
            }
        }
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
/*
std::vector<mpz_class> countFromEveryState(const DFA& dfa, int n) {
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

    return prev;
}
*/

int main() {
    DFA dfaL = buildDFA_L();
    DFA dfaL2 = buildDFA_L2(dfaL);

    int choice;
    int n;

    while (true) {
        std::cout << "\n====================================\n";
        std::cout << "        DFA String Counter\n";
        std::cout << "====================================\n";
        std::cout << "1. Count strings in L\n";
        std::cout << "2. Count strings in L'\n";
        std::cout << "3. Quit\n";
        std::cout << "Enter choice: ";

        if (!(std::cin >> choice)) {
            if (std::cin.eof()) {
                break;
            }

            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            std::cout << "Invalid choice.\n";
            continue;
        }

        if (choice == 3) {
            std::cout << "Exiting program.\n";
            break;
        }

        if (choice != 1 && choice != 2) {
            std::cout << "Invalid choice.\n";
            continue;
        }

        std::cout << "Enter string length (1-300): ";

        if (!(std::cin >> n)) {
            if (std::cin.eof()) {
                break;
            }

            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            std::cout << "Invalid length.\n";
            continue;
        }

        if (n < 1 || n > 300) {
            std::cout << "Length must be between 1 and 300.\n";
            continue;
        }

        mpz_class result;

        if (choice == 1) {
            result = countAccepted(dfaL, n);
        }
        else {
            if (n % 2 != 0) {
                result = 0;
            }
            else {
                int k = (n - 2) / 2;
                result = countAccepted(dfaL2, k);
            }
        }

        std::cout << "Number of accepted strings: "
                  << result << std::endl;
    }

    return 0;
}