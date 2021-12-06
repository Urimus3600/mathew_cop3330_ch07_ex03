/*
	calculator08buggy.cpp | From: Programming -- Principles and Practice Using C++, by Bjarne Stroustrup
	We have inserted 3 bugs that the compiler will catch and 3 that it won't.
*/

#include "std_lib_facilities.h"

struct Token {
    //holds type of token
	char kind;
    //holds number
	double value;
    //holds name
	string name;
    //initializes single character, sets value to 0, shoud be an operator
	Token(char ch) :kind(ch), value(0) { }
    //initializes token with specific type and value
	Token(char ch, double val) :kind(ch), value(val) { }
    //holds kind and string, holds name
    Token(char ch, string n) :kind{ch}, name{n} { }
};

class Token_stream {
    //keeps track if buffer is occupied
	bool full;
    //holds a token at a time
	Token buffer;
public:
    //at start buffer is set to empty
	Token_stream() :full(0), buffer(0) { }

    //undefined get, will read in tokens
	Token get();
    //unget puts back a taken character into buffer
	void unget(Token t) { if(!full){
        //if not full, place in buffer, prevents lost inputs
        buffer = t; full = true; }
        else{
            error("Buffer is full, cannot unget");
        }
    }

	void ignore(char);
};

const char let = 'L';
const char quit = 'Q';
const char print = ';';
const char number = '8';
const char name = 'a';
const char fsqrt = '2';
const char fpow = 'p';
const char decConst = 'c';
//sets token kinds

Token Token_stream::get()
{   //if buffer is occupied, return buffer
	if (full) { full = false; return buffer; }
	char ch;
	cin >> ch;
	switch (ch) {
        case '(':
        case ')':
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
        case ';':
        case '=':
        case ',':
            return Token(ch);
        case '.':
        {
            if(cin){
                char afterDec;
                cin >> afterDec;
                if(!isdigit(afterDec)){
                    return Token(number, 0);
                }
            }
        }
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        {	cin.unget();
            //if a number part was read in, put the piece back
        double val;
        cin >> val;
        //read in entire number
        return Token(number, val);
        //return number type Token of value
        }
        default:
            //if none of the above, check if name or command
            if (isalpha(ch)) {
                //first must be a letter
                string s;//holds name
                s += ch;//add first char
                while (cin.get(ch) && (isalpha(ch) || isdigit(ch)||ch=='_')) s += ch;
                //read in any subsequent letters and numbers, adds to s until white space or symbol
                cin.unget();
                //one extra char will be read, send to input
                //if (s == "#") return Token(let);//for declaration
                if (s == "let") return Token(let);//for declaration
                if (s == "exit") return Token(quit);//for quit command
                if (s == "sqrt") return Token(fsqrt);//for sqrt function
                if (s == "pow") return Token(fpow);//for pow funcion
                if (s == "const") return Token(decConst);//for pow funcion


                //recognizes commands
                return Token(name, s);
            }
            cout<<ch;
            error("Bad token");
            cout<<"REACHED";
            return Token('e');
            //compiler would not let me compile without return value here
    }
}

void Token_stream::ignore(char c)
{   //defines ignore
	if (full && c == buffer.kind) {
        //if buffer is occupied and c is equal to buffer kind
        //what if c is not a recognized type, but could be a match?
        //this is public so may be an issue
		full = false;
		return;
	}
	full = false;
    //if c doesn't match, buffer is set to empty or is already empty

	char ch;
	while (cin >> ch)
		if (ch == c) return;
        //reads in characters until character matches c
        //all are lost after so buffer is free and end character is gone too
}

struct Variable {
	string name;
    bool constant;
	double value;
	Variable(string n, double v) :name(n), value(v), constant(0) { }
    Variable(string n, double v, bool c) :name(n), value(v), constant(c) { }
};

vector<Variable> names;//global Variable vector

double get_value(string s)
{
	for (int i = 0; i < names.size(); ++i)//iterates thru vector
		if (names[i].name == s) return names[i].value;//returns value if name is found
	error("get: undefined name ", s);
    cout<<"REACHED";
    return 0.0;
    //compiler did not let me leave out return here
}

void set_value(string s, double d)
{
	for (int i = 0; i <= names.size(); ++i)//iterates thru vector
		if (names[i].name == s) {//if name matches
			names[i].value = d;//set value
			return;
            //other parts of the code prevent redefinition
		}
	error("set: undefined name ", s);
}

bool is_declared(string s)
{
	for (int i = 0; i < names.size(); ++i)
		if (names[i].name == s) return true;
        //finds if name exists within vector
	return false;
}

Token_stream ts;//global Token_stream

double expression(); //undefined expression, for call in other functions

double primary()
{
	Token t = ts.get();
    //gets next Token
	switch (t.kind) {
        case '(':
        {	double d = expression();
        t = ts.get();
        if (t.kind != ')') error("expected ')'");
        //checks syntax
        return d;
        }
        case '-':
            return -primary();
            //for unary negative
        case number:
            return t.value;
        case name:
            return get_value(t.name);
        case fsqrt:
        {//function sqrt is available, only works with parentheses

            t = ts.get();
            if(!(t.kind == '(')) error("expected '(' after sqrt");
            	double d = expression();
                t = ts.get();
                if (t.kind != ')') error("expected ')' after sqrt(a");
                //checks syntax
                return sqrt(d);
        }
        case fpow:{
            t = ts.get();
            if(!(t.kind == '(')) error("expected '(' after pow");
            double a = expression();
            t = ts.get();
            if(!(t.kind == ',')) error("expected ',' between parameters of pow");
            double b = expression();
            if(fmod(b,1)!=0) error("second parameter of pow must be an integer");
            t = ts.get();
            if(!(t.kind == ')')) error("expected ')' after pow(a,b");
            return pow(a,b);
        }
        default:
        {
            error("primary expected");
            cout<<"REACHED";
            return 0.0;
        }
	}
}

double term()
{
	double left = primary();//gets next primary
	while (true) {
		Token t = ts.get();//gets next token, contunues until not division or multiplication
		switch (t.kind) {
		case '*':
			left *= primary();
			break;
		case '/':
		{	double d = primary();
		if (d == 0) error("divide by zero");//cant divide by 0
		left /= d;
		break;
		}
        case '%':
        {
            double d = primary();
            if (d == 0) error("modulo by zero");//cant divide by 0
            left = fmod(left,d);
            break;
        }
		default:
			ts.unget(t);//returns not term operation to buffer
			return left;//returns final term
		}
	}
}

double expression()
{
	double left = term();//gets next term
	while (true) {//continues until addition or subtraction ends
		Token t = ts.get();//gets next token
		switch (t.kind) {
            case '+':
                left += term();
                break;
            case '-':
                left -= term();
                break;
            case ',':{
                ts.unget(t);//returns non expression operator to buffer
                return left;
            }
            default:
                ts.unget(t);//returns non expression operation to buffer
                return left;
		}
	}
}

double declaration(bool constant)
{
	Token t = ts.get();//gets next token
	if (t.kind != 'a') error("name expected in declaration");
	string name = t.name;
	if (is_declared(name)) error(name, " declared twice");
	Token t2 = ts.get();//gets next token, should be =
	if (t2.kind != '=') error("= missing in declaration of ", name);
	double d = expression();//finds other side of declaration
    //maybe second declaration should be allowed? no need for 2 of same value tho
    //method for clear variable needed
	names.push_back(Variable(name, d, constant));

    //puts in vector
	return d;
    //returns variables final value
}
double reassignment(string s, bool constant)
{
    int index = -1;
    for (int i = 0; i < names.size(); ++i){
        if (names[i].name == s) index = i;
    }
    if (index<0) error(s, " not declared");
    if(names[index].constant) error(s, " is const, cannot be reassigned");
    double d = expression();//finds other side of declaration
    //maybe second declaration should be allowed? no need for 2 of same value tho
    //method for clear variable needed
    names[index].value = d;
    names[index].constant = constant;
    //puts in vector
    return d;
    //returns variables final value
}

double statement()
{
	Token t = ts.get();//reads in token
    bool constant = false;
	switch (t.kind) {
        case let://if declaration
        {
            t = ts.get();
            if(t.kind == decConst){
                return declaration(true);
            }
            else{
                ts.unget(t);
                return declaration(false);
            }
        }
        case decConst:{
            constant = true;
            t = ts.get();
            if(!(t.kind == name)) error("name type expected after const");
        }
        case name:{
            if(is_declared(t.name)){
                char ch; cin >> ch;
                if(ch =='='){
                    return reassignment(t.name, constant);
                }
                else if(constant){
                    error("incorrect use of const");
                }
                ts.unget(t);
            }
        }
        default:
            ts.unget(t);//returns token to buffer
            return expression();//considers until ; an expression
        }
}

void clean_up_mess()
{
	ts.ignore(print);//ignores up to and including ;
}

const string prompt = "> ";
const string result = "= ";

void calculate()
{
    names.push_back(Variable("pi", 3.1415926535, true));
    names.push_back(Variable("e", 2.7182818284, true));
    names.push_back(Variable("k",1000, true));
	while (true) try {
		cout << prompt;//tells user to start
		Token t = ts.get();//reads in next token
		while (t.kind == print) t = ts.get();//skip tokens of kind ;

		if (t.kind == quit) return;//ends as soon as quit is entered
        ts.unget(t);//returns last token to buffer, it is the one after the last ;
        double answer = statement();//sometimes answers would not appear directly after =
		cout << result << answer << endl;
        //reads out = then calls statement which returns the answer as a double
	}
	catch (runtime_error& e) {
		cerr << e.what() << endl;
		clean_up_mess();
	}
}

int main()

try {
	calculate();//runs calculator until quit
	return 0;
}
catch (exception& e) {
	cerr << "exception: " << e.what() << endl;
	char c;
	while (cin >> c && c != ';');
	return 1;
}
catch (...) {
	cerr << "exception\n";
	char c;
	while (cin >> c && c != ';');
	return 2;
}
