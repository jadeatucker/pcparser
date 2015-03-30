// pcparser.cpp : Defines the entry point for the console application.
//
// Simple expression parser using prcedence climbing approach.

#include "stdafx.h"

#include <iostream>
#include <map>
#include <sstream>
#include <string>

using namespace std;

int _pcparse(stringstream &tokens, int min_prec);

// Map of operator precedence
std::map<char, int> OPS;

void init() {
	OPS['+'] = 1;
	OPS['-'] = 1;
	OPS['*'] = 2;
	OPS['/'] = 2;
	OPS['^'] = 3;
}

int compute(std::string op, int a, int b) {
	switch (op[0]) {
		case '+': return a + b;
		case '-': return a - b;
		case '*': return a * b;
		case '/': return a / b;
		case '^': return pow(a, b);
		default:
			throw std::invalid_argument("invalid operator");
	}
}

int parse_primary(stringstream &tokens) {
	std::string token;

	if (tokens >> token) {
		if (token == "(") {
			int val = _pcparse(tokens, 1);
			tokens >> token; // eat closing paren

			if (token != ")") {
				throw std::exception("unmatched '('");	
			}
			return val;
		}
		else {
			return std::stoi(token); // parse int
		}
	}
	else {
		throw std::exception("stream ended unexpectedly");
	}
}

int _pcparse(stringstream &tokens, int min_prec) {
	std::string token;
	int lhs, rhs, next_prec;

	lhs = parse_primary(tokens);

	// while we have a binary operator with precedence >= current precedence
	while (tokens >> token) {
		next_prec = OPS[token[0]];
		if (next_prec >= min_prec) {
			if (token == "^") // ^ operator is right associative
				next_prec++;

			rhs = _pcparse(tokens, next_prec);  // parse right hand side
			lhs = compute(token, lhs, rhs);  // evaluate the operation
		}
		else {
			// next token has lower prcedence or is close paren so put it back and return.
			tokens.putback(token[0]); 
			break;
		}
	}

	return lhs;
}

int pcparse(std::string expr) {
	stringstream stream(expr);
	return _pcparse(stream, 1);  // main parse function, default precedence 1
}

int _tmain(int argc, _TCHAR* argv[])
{
	std::string expr = "( 9 - 8 ) - 7";

	init();
	
	try {
		cout << "Expression: " << expr << endl;
		cout << "Output: " << pcparse(expr) << endl;
	}
	catch (const std::exception e) {
		cerr << "Error: " << e.what() << endl;
		return 1;
	}
	
	return 0;
}