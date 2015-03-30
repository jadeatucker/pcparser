// pcparser.cpp : Defines the entry point for the console application.
//
// Simple expression parser using prcedence climbing approach.
//
// TODO - Add support for unary operators

#include "stdafx.h"

#include <iostream>
#include <map>
#include <sstream>
#include <string>

using namespace std;

int _pcparse(stringstream &tokens, int min_prec);

// Map of operator precedence
map<char, int> OPS;

void init() {
	OPS['+'] = 1;
	OPS['-'] = 1;
	OPS['*'] = 2;
	OPS['/'] = 2;
	OPS['^'] = 3;
}

int compute(char op, int a, int b) {
	switch (op) {
	case '+': return a + b;
	case '-': return a - b;
	case '*': return a * b;
	case '/': return a / b;
	case '^': return pow(a, b);
	default:
		throw invalid_argument("invalid operator");
	}
}

int parse_primary(stringstream &tokens) {
	string token;

	if (tokens >> token) {
		if (token == "(") {
			int val = _pcparse(tokens, 1);
			tokens >> token; // eat closing paren

			if (token != ")") {
				throw exception("unmatched '('");
			}
			return val;
		}
		else if (token == "-") {
			return -(parse_primary(tokens));  // handle unary negation
		}
		else {
			return stoi(token); // parse int
		}
	}
	else {
		throw exception("stream ended unexpectedly");
	}
}

int _pcparse(stringstream &tokens, int min_prec) {
	string token;
	int lhs, rhs, next_prec;

	lhs = parse_primary(tokens);

	// while we have a binary operator with precedence >= current precedence
	while (tokens >> token) {
		next_prec = OPS[token[0]];
		if (next_prec >= min_prec) {
			if (token == "^") // ^ operator is right associative
				next_prec++;

			rhs = _pcparse(tokens, next_prec);  // parse right hand side
			lhs = compute(token[0], lhs, rhs);  // evaluate the operation
		}
		else {
			// next token has lower prcedence or is close paren so put it back and return.
			tokens.putback(token[0]);
			break;
		}
	}
	return lhs;
}

int pcparse(string expr) {
	stringstream stream(expr);
	return _pcparse(stream, 1);  // main parse function, default precedence 1
}

struct test {
	std::string expr;
	int val;
};

struct test TESTS[] = {
	{ "( 9 - 8 ) - 7", -6 },
	{ "6 * 7 + 4", 46 },
	{ "4 ^ 2 - 8", 8 },
	{ "11 * 2 - 8 / 2", 18 },
	{ "1 + 14 - ( 8 * 2 )", -1 },
	{ "- 8", -8 },
	{ "- ( 8 * 7 ) + 2",  -54},
	{ "2 * - ( 2 + 4 )",  -12},
};
#define TESTS_LEN (sizeof(TESTS) / sizeof(struct test))

int _tmain(int argc, _TCHAR* argv[])
{
	init();

	int n, pass = 0;
	struct test t;
	for (int i = 0; i < TESTS_LEN; i++) {
		t = TESTS[i];
		try {
			if ((n = pcparse(t.expr)) != t.val) {
				cout << "FAIL: expected " << t.val << " but got " << n;
				cout << " for expression: '" << t.expr << "'" << endl;
			}
			else {
				pass++;
			}
		}
		catch (exception e) {
			cerr << "ERROR: '" << e.what() << "' for expression: '" << t.expr << "'" << endl;
		}
	}

	if (pass != TESTS_LEN) {
		cout << (TESTS_LEN - pass) << " of " << TESTS_LEN << " tests FAILED!" << endl;
	}
	else {
		cout << "TESTS PASS!" << endl;
	}
	return 0;
}