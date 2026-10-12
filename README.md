# Counting_DFA
CS 454 Project 1 - Building DFA's for counting strings

## DESIGN
Main -> Build DFA for L -> Build DFA for L'' -> Shared DFA counting function -> Display Result

### COMPONENT: DFA Representation

Design a data structure to represent a DFA containing:  
-number of states (int)  
-alphabet size (int)  
-transition function (2d array)  
-starting state (int = 0)  
-accepting states (boolean vector of length n, where n = number of states)  

### COMPONENT: DFA Builder for L
State representation:
- Each state encodes a buffer containing the last 0–5 characters.
- Use base-4 encoding with length-based offsets to assign unique state IDs.
- Include one additional fail state.
Transition rules:
1. If the buffer contains fewer than 5 characters, append the input character.
2. If the buffer contains 5 characters, append the input and check all 6 characters.
3. If all four letters occur, discard the oldest character and transition to the updated buffer state.
4. Otherwise, transition to the fail state.
5. All transitions from the fail state return to itself.
6. All states except the fail state are accepting.

### COMPONENT: DFA Builder for L''
Design:
- Start with the fixed middle aa.
- Process pairs of characters, extending the string outward.
- Encode each pair as an input symbol from 0 through 15.
- Track sufficient information about both sides to enforce the length-6 condition.
- Define accepting states based on whether the resulting string is valid.
For an original string of length \(n=2m\), count strings of length \(m-1\) accepted by \(L''\).

### COMPONENT: Shared Counting Algorithm
Inputs: DFA representation, n (the string length to count)
Output: exact number of strings of length n accepted by the DFA
- Create two arrays, prev and next, each containing one entry for every DFA state
- Initialze prev[j] to 1 if state j is accepting, otherwise 0
- for each length k from 1 to n, calculate: \[next[j]=\sum_{x=0}^{|\Sigma|-1}prev[\delta(j,x)]\]
- after computing all states, copy next into prev
- after n iterations, return prev[0]

### COMPONENT: Main
1)Construct both DFAs (once).
2)Display the three-option menu.
3)Read and validate the user's choice.
4)Read \(n\), ensuring \(1\le n\le300\).
5)Invoke the counting algorithm with the appropriate DFA and length.
6)Display the exact result.
7)Repeat until the user chooses to quit.

## Proposed Functions
### encodeState
  -convert a buffer into a unique state id
### decodeState
  -convert a state ID into a buffer
### isValidSix
  -check whether a 6 character string contains a,b,c,d
### buildDFA_L
  -construct dfa for L
### buildDFA_L2
  -construct dfa for L''
### countAccepted
  -counts accepted strings. use dynamic programming.
### main
  -manage user io and calls the other functions

## STAGES:
1) Design DFA data structure specified above
2) Design state encoding - how to represent buffers of lengths 0-5 as unique integer states. Verify you can convert between buffer strings and numeric identities.
3) Implement DFA builder for L - generate transition table
4) Implement the counting algorithm. Use PREV and NEXT arrays. Make the function general enough to accept either DFA.
5) Test Problem 1
6) Implement DFA builder for L'' - outward-pair representation and additional state information needed for middle aa condition
7) Design menu and include library for arb-precision ints
8) Integrate and test full program 

## Requirements -
Use GMP Library to support arbitrarily large int  
DFA L - Integer-labeled states and 4 symbol alphabet  
DFA L'' - Integer-labeled states and a 16-symbol alphabet  
Generate transition functions programmatically.  
Represent DFA transitions in a form usable by the counting algortithm  
Use the specified recurrence relation with the two arrays prev and next  
Compute exact count for input lengths 1 to 300  
Support both counting problems through a repeating 3 choice menu  
Document every function with inputs, outputs, examples, and preconditions  
Test the program against instructor provided test cases  

important things to remember -   
There are two separate algorithms in this project.   
The first constructs a DFA.  
The second uses that DFA to count accepted strings of a given length 
