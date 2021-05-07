/*
Writers: H & M
Student Number(s): 2018400141 - 2018400156
*/

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
using namespace std;
 
/* ----- GLOBAL VARIABLES -----*/

// Records current line for printing syntax errors.
int line_count = 0;
// It helps to check whether line has while loop or not.
int while_opened = 0;
// It helps to check whether line has 'if' or not.
int if_opened = 0;
//For numerating while and if labels.
int conditioner = 1;
int conditioner1 = 1;
//It helps to numerate LLVM variables.
int temp = 1;

int choose_result = 0;

// Input and output files.
ifstream in_file;
ifstream in_file_two;
ofstream out_file;
ofstream out_file_two;

/*We put all the lines of the input code into this vector.
Later we parsed all lines one by one by iterating through all_lines.*/
vector<string> all_lines;

// We put allocated variables into this vector.
vector<string> all_variables;


/* ------- DECLARATION OF THE METHODS ------- */
// Methods divide into 3 main categories: Helper Methods, Syntax Checkers, Parser Methods.


/* --- Helper Methods --- */
void print_module();
void take_all_lines();
bool is_number(string s);

/* --- Syntax Checkers ---*/
void syntax_error_handler();
void paranthesis_count(int i);
void equal_sign_count(int i);
void white_space_eraser(int i); 
void while_if_checker(int i);
void print_checker(int i);
void comment_eraser(int i); 
bool isValidVar(string var);
void is_it_all_good(int i);
void syntax_checker(int i);

/* --- Parser Methods --- */
string assign_parser(string line);
string add_op(string line);
string sub_op(string line);
string mul_op(string line);
string div_op(string line);
string par_op(string line);
void parser(string line);
void choose_handler(string exp);
string choose_finder(string line);



/*	------ IMPLEMENTATION OF THE METHODS -------  */

/**
* Prints the beginning of the LLVM file.
*/
void print_module()
{
	out_file_two << "; ModuleID = 'mylang2ir'" << endl;
	out_file_two << "declare i32 @printf(i8*, ...)" << endl;
	out_file_two << "@print.str = constant [4 x i8] c\"%d\\0A\\00\"" << endl;
	out_file_two << endl;
	out_file_two << "define i32 @main() {" << endl;
}

/**
* Takes all the lines of the given input code and puts it into all_lines vector.
*/
void take_all_lines()
{
	string line;
	while (getline(in_file, line)) // While there is a line...
	{
		all_lines.push_back(line); // Keep going to push back into all_lines vector.
	}
}


/**
* Takes the given string and decides whether it is number or not. If false then it is a variable.
* @param s String to decide
* @return true if it is a number, false if it is not a number.
*/
bool is_number(string s)
{
	
	if(s.size()==0)
	{
		return false;
	}
	for (int i = 0; i < s.size(); i++) {

		if (!(s[i] <= 57 && s[i] >= 48)) { // Checks if chars of the string are number by looking ASCII interval for numbers.

			return false;

		}
	}

	return true;
}

void syntax_error_handler()
{

	out_file_two << "; ModuleID = 'mylang2ir'" << endl;
	out_file_two << "declare i32 @printf(i8*, ...)" << endl; // declare i32 @printf(i8*, ...)
	out_file_two << "@print.str = constant [23 x i8] c\"Line %d: syntax error\\0A\\00\""<< endl;
	out_file_two << endl;
	out_file_two << "define i32 @main() {" << endl;
	out_file_two << "call i32 (i8*, ...)* @printf(i8* getelementptr ([23 x i8]* @print.str, i32 0, i32 0), i32 "<<line_count<<")";
	out_file_two << endl;
	out_file_two << "\t" << "ret i32 0" << endl;
	out_file_two << "}";
	
	remove("file.txt");
	exit(0);
}

/**
* Checks that whichever open paranthesis in a line has a closed paranthesis.
* If every open paranthesis has a closed paranthesis does not print syntax error, prints and exits the program otherwise.
* @param i int to check ith line.
*/
void paranthesis_count(int i)
{

	int open = 0; /* Increases 1 for every ' ( ' encountered and decreases 1 for every ' ) ' encountered.
					 Will return true if at the end of the loop it is still zero.	*/
	string line = all_lines[i];
	for (int i = 0; i < line.size(); i++)
	{
		if (line[i] == '(')
		{
			open++;
		}
		if (line[i] == ')')
		{
			open--;
		}
	}
	if (open != 0)
	{
		syntax_error_handler();
	}
	open = 0;
}


/**
* Checks that given line has more than one equal sign.
* @param i int to check ith line.
*/
void equal_sign_count(int i)
{
	int open = 0; /*Increases 1 for every equal sign encountered.
					If it has a value whis is more than one at the end of the loop then prints a syntax error line.*/
	string line = all_lines[i];
	for (int k = 0; k < line.size(); k++)
	{
		if (line[k] == '=')
		{
			open++;
		}
	}
	if (open > 1)
	{
		syntax_error_handler();
	}
}

/**
* Erases all of the white spaces in a line.
  @param i int for ith line.
*/
void white_space_eraser(int i)
{
	string line = all_lines[i];
	string temp = ""; // Records the white spaceless version of the line.
	for (int k = 0; k < line.size(); k++)
	{
		if (!(line[k] == ' ' || line[k] == '\t' || line[k] == '\n' || line[k] == '\r' || line[k] == '\v' || line[k] == '\f'))

		{
			temp += line[k];
		}
	}
	all_lines[i] = temp; // White spaceless version of the line pushed back into all_lines vector.
}

/**
* Checks that while or if statement syntactically true.
* If they are not syntactically true then prints a syntax error message and exits the program.
* @param i int for ith line.
*/
void while_if_checker(int i)
{
	string line = all_lines[i];
	if (line.substr(0, 5) == "while") // If line is a while statement enters.
	{
		if (line[5] == '(') // 5th index of the while statement must be ' ( ' 
		{
			int length = line.size();

			if (line.substr(length - 2, length) != "){" || line.find('=') != string::npos) /*   The last two chars of the while statement must be ' ){ ' . If not then print syntax error.
																								Also while statement must not contain ' = ' */
			{
				syntax_error_handler();
			}
		}
	}
	else // If line is a if statement enters.
	{
		if (line[3] == '(') // 3rd index of the while statement must be ' ( ' 
		{
			int length = line.size();

			if (line.substr(length - 2, length) != "){" || line.find('=') != string::npos) /*   The last two chars of the if statement must be ' ){ ' . If not then print syntax error.
																								Also if statement must not contain ' = ' */
			{
				syntax_error_handler();
			}
		}
	}
}

/**
* Decides given line of the print statement is syntactically okay. If not then prints syntax error.
* @param i int for ith line.
*/
void print_checker(int i)
{
	string line = all_lines[i];
	if (line[5] == '(') // 5th index of the while statement must be ' ( ' 
	{

		if (line[line.size() - 1] != ')') // Last index of the print statement must be ' ) '
		{
			syntax_error_handler();
		}
	}
}

/**
* Erases every comment in the given input code which means erasing everything after the char ' # '.
* @param i int for ith line.
*/
void comment_eraser(int i)
{
	string line = all_lines[i];
	for (int i = 0; i < line.size(); i++)
	{
		if (line[i] == '#')
		{
			line = line.substr(0, i); // Record the line until where char '#' encountered.
		}
	}
	all_lines[i] = line;

}

/**
* Decides whether variable name is syntactically correct or not. Called
* @param var String to check.
* @return TRUE if variable is valid variable, FALSE if not.
*/
bool isValidVar(string var)
{

	if (var.size() == 0) // Checks for empty line.
	{
		syntax_error_handler();
	}
	if (isalpha(var[0]) == 0) // If first index is a number then print syntax error.
	{
		syntax_error_handler();
	}
	if(var=="if"||var=="while"){

		syntax_error_handler();
	}
	for (int i = 1; i < var.length(); i++) // Checks every char of the string var is syntactically correct.
	{
		if (isalnum(var[i]) == 0 || var[i] == ' ')
		{
			syntax_error_handler();
		}
	}

	return true;
}

/**
* Checks that if the given line has unpermitted char. Prints syntax error if it has.
* @param i int for ith line.
*/
void is_it_all_good(int i)
{
	string line = all_lines[i];
	for (int k = 0; k < line.size(); k++)
	{
		if (!(isalpha(line[k]) || isdigit(line[k]) || line[k] == '{' || line[k] == '}' || line[k] == '(' || line[k] == ')' || line[k] == '=' || line[k] == ',' || line[k] == '+'
			|| line[k] == '-' || line[k] == '*' || line[k] == '/')) // *** All permitted chars in the code. ***
		{
			syntax_error_handler();
		}
	}
}

/**
* Parent method for all SYNTAX CHECKER methods.
* @param i int for ith line.
*/
void syntax_checker(int i)
{
	string line = all_lines[i];
	is_it_all_good(i); // Unpermitted char control.
	paranthesis_count(i); // Paranthesis control.
	equal_sign_count(i); // Equal sign control.

	if (line.substr(0, 5) == "while" || line.substr(0, 3) == "if") // If line is while or if statement.
	{
		while_if_checker(i); // While, if statement syntax control.
	}
	if (line.substr(0, 5) == "print") // If line is print statement.
	{
		print_checker(i); // Print statement syntax control.
	}

}

/**
* Assignment parser for assignment statements.
* Takes left of the = operator and checks whether it is a valid variable or not.
* Then takes the right of the = operator and puts it into add_op method.
* Then parsing begins.
* At the end of the method it writes the LLVM code that equalizes given variable with given expression.
* @param line String for parsing.
*/
string assign_parser(string line)
{
	int eq_index = line.find('=');

	string variable = line.substr(0, eq_index);

	std::vector<string>::iterator it;

	if (isValidVar(variable))
	{
		it = find(all_variables.begin(), all_variables.end(), variable);

		if (it == all_variables.end())
		{
			all_variables.push_back(variable);
		}
	}
	else
	{
		syntax_error_handler();
	}

	string exp = line.substr(eq_index + 1, line.size() - eq_index - 1);

	string b = add_op(exp);

	if (is_number(b))
	{
		out_file << "\t" << "store i32 " << b << ", i32* %" << variable << endl;
	}
	else if (b[0] != '%')
	{
		out_file << "\t" << "%_" << temp << "= load i32* %" << b << endl;
		temp++;
		out_file << "\t" << "store i32 %_" << temp - 1 << ", i32* %" << variable << endl;
	}
	else if (b[0] == '%')
	{
		out_file << "\t" << "store i32 " << b << ", i32* %" << variable << endl;
	}

	string a = "%_" + to_string(temp - 1);
	return a;
}

/**
* If line contains a choose statement than sends the line to the choose_finder function and continues after.
* Does the following part of the parsing algorithm. BNF Notation:
* <expr> ---> <term> + <expr>
* <expr> ---> <term>
* Right recursive.
* Iterates through chars of the line and stops when it see a + operand.
* Then divides expression into two part as <term> and <expr>.
* Continues it until all +'s are checked.
* Then calls sub_op function.
* @param line String to parse.
* @return result of the parsing.
*/
string add_op(string line)
{	
	if(line.find("choose(") != string::npos)
	{
		line = choose_finder(line);
	}
	
	int op_index = -1;
	int par = 0;

	for (int i = line.size() - 1; i >= 0; i--) 
	{
		if (line[i] == '(') 
		{
			par++;
		}
		if (line[i] == ')') 
		{
			par--;
		}

		if (par == 0 && line[i] == '+') 
		{
			op_index = i;
			break;
		}
	}

	if (op_index != -1) 
	{
		if(line.substr(op_index + 1, line.size()).size()==0||line.substr(0, op_index).size()==0)
		{
			syntax_error_handler();
		}

		string right = add_op(line.substr(op_index + 1, line.size()));
		string left = add_op(line.substr(0, op_index));

		if(!is_number(left))
		{
			if(left[0]!='%')
			{
				out_file << "\t" <<"%_"<< temp << "= load i32* %" << left<< endl;
				left= "%_"+to_string(temp);
				temp++;
			}
		}
				
		if(!is_number(right))
		{
			if(right[0]!='%')
			{
				out_file << "\t" <<"%_"<< temp << "= load i32* %" << right<< endl;
				right= "%_"+to_string(temp);
				temp++;
			}
		}
		string a= "%_"+to_string(temp);
		out_file << "\t" << "%_" << temp << " = add i32 " << left << ", " << right << endl;
		temp++;
		return a;
	}
	else 
	{
		return sub_op(line);
	}

}

/**
* Does the following part of the parsing algorithm. BNF Notation:
* <expr> ---> <term> - <expr>
* <expr> ---> <term>
* Right recursive.
* Iterates through chars of the line and stops when it see a - operand.
* Then divides expression into two part as <term> and <expr>.
* Continues it until all -'s are checked.
* Then calls mul_op function.
* @param line String to parse.
* @return result of the parsing.
*/
string sub_op(string line)
{
	int op_index = -1;
	int par = 0;
	for (int i = line.size() - 1; i >= 0; i--) 
	{
		if (line[i] == '(') 
		{
			par++;
		}
		if (line[i] == ')') 
		{
			par--;
		}

		if (par == 0 && line[i] == '-')
		{
			op_index = i;
			break;
		}
	}

	if (op_index != -1) 
	{

		if (line.substr(op_index + 1, line.size()).size() == 0 || line.substr(0, op_index).size() == 0) 
		{
			syntax_error_handler();
		}

		string right = sub_op(line.substr(op_index + 1, line.size()));
		string left = sub_op(line.substr(0, op_index));

		if (!is_number(left)) 
		{

			if (left[0] != '%') 
			{
				out_file << "\t" << "%_" << temp << "= load i32* %" << left << endl;
				left = "%_" + to_string(temp);
				temp++;
			}
		}

		if (!is_number(right)) 
		{

			if (right[0] != '%') 
			{
				out_file << "\t" << "%_" << temp << "= load i32* %" << right << endl;
				right = "%_" + to_string(temp);
				temp++;
			}
		}

		string a = "%_" + to_string(temp);
		out_file << "\t" << "%_" << temp << " = sub i32 " << left << ", " << right << endl;
		temp++;
		return a;
	}
	else 
	{
		return mul_op(line);
	}
}


/**
* Does the following part of the parsing algorithm. BNF Notation:
* <term> ---> <factor> * <term>
* <term> ---> <factor>
* Right recursive.
* Iterates through chars of the line and stops when it see a * operand.
* Then divides expression into two part as <factor> and <term>.
* Continues it until all *'s are checked.
* Then calls div_op function.
* @param line String to parse.
* @return result of the parsing.
*/
string mul_op(string line)
{
	int op_index = -1;
	int par = 0;
	for (int i = line.size() - 1; i >= 0; i--)
	{
		if (line[i] == '(') {
			par++;
		}
		if (line[i] == ')') {
			par--;
		}
		if (par == 0 && line[i] == '*') 
		{

			op_index = i;
			break;

		}
	}

	if (op_index != -1) 
	{
		if (line.substr(op_index + 1, line.size()).size() == 0 || line.substr(0, op_index).size() == 0) 
		{
			syntax_error_handler();
		}

		string right = mul_op(line.substr(op_index + 1, line.size()));
		string left = mul_op(line.substr(0, op_index));

		if (!is_number(left)) 
		{

			if (left[0] != '%') 
			{
				out_file << "\t" << "%_" << temp << "= load i32* %" << left << endl;
				left = "%_" + to_string(temp);
				temp++;
			}
		}

		if (!is_number(right)) 
		{
			if (right[0] != '%') 
			{
				out_file << "\t" << "%_" << temp << "= load i32* %" << right << endl;
				right = "%_" + to_string(temp);
				temp++;
			}
		}

		string a = "%_" + to_string(temp);
		out_file << "\t" << "%_" << temp << " = mul i32 " << left << ", " << right << endl;
		temp++;
		return a;
	}
	else 
	{
		return div_op(line);
	}
}

/**
* Does the following part of the parsing algorithm. BNF Notation:
* <term> ---> <factor> / <term>
* <term> ---> <factor>
* Right recursive.
* Iterates through chars of the line and stops when it see a / operand.
* Then divides expression into two part as <factor> and <term>.
* Continues it until all /'s are checked.
* Then calls par_op function.
* @param line String to parse.
* @return result of the parsing.
*/
string div_op(string line)
{
	int op_index = -1;
	int par = 0;
	for (int i = line.size() - 1; i >= 0; i--)
	{
		if (line[i] == '(') 
		{
			par++;
		}
		if (line[i] == ')') 
		{
			par--;
		}
		if (par == 0 && line[i] == '/')
		{
			op_index = i;
			break;

		}
	}

	if (op_index != -1) 
	{

		if (line.substr(op_index + 1, line.size()).size() == 0 || line.substr(0, op_index).size() == 0) 
		{
			syntax_error_handler();
		}
		string right = div_op(line.substr(op_index + 1, line.size()));
		string left = div_op(line.substr(0, op_index));

		if (!is_number(left)) 
		{

			if (left[0] != '%') 
			{
				out_file << "\t" << "%_" << temp << "= load i32* %" << left << endl;
				left = "%_" + to_string(temp);
				temp++;
			}
		}

		if (!is_number(right)) 
		{
			if (right[0] != '%') 
			{
				out_file << "\t" << "%_" << temp << "= load i32* %" << right << endl;
				right = "%_" + to_string(temp);
				temp++;
			}
		}

		string a = "%_" + to_string(temp);
		out_file << "\t" << "%_" << temp << " = sdiv i32 " << left << ", " << right << endl;
		temp++;
		return a;
	}
	else
	{
		return par_op(line);
	}
}

/**
*
* Does the following part of the parsing algorithm. BNF Notation:
* <factor> ---> (<expr>)
* <factor> --->  <var>
* <factor> ---> <number>
* Right recursive.
* If zeroth index of the line is paranthesis then:
* Removes outer paranthesis.
* And calls the add_op method.
* This means <factor> ---> (<expr>)
* ( and ) are lexemes.
* -------------------------------------------
* If zeroth index of the line is not paranthesis then that means it is a variable or number.
* Controls whether it is a number or variable.
* If number returns the line.
* If variable then controls whether it is a valid variable or not.
* If not prints syntax error.
* If it is a variable then it pushes back variable into all_variables vector.
* @param line String to parse.
* @return result of the parsing.
*/
string par_op(string line)
{

	if (line[0] == '(' && line[line.size() - 1] == ')')
	{
		if (line.substr(1, line.size() - 2).size() == 0) 
		{
			syntax_error_handler();
		}
		return add_op(line.substr(1, line.size() - 2));
	}

	else 
	{
		if (is_number(line))
		{
			return line;
		}
		else if (isValidVar(line)) 
		{
			bool isRegistered = false;

			for (int i = 0; i < all_variables.size(); i++) 
			{
				if (all_variables[i] == line) 
				{
					isRegistered = true;
				}
			}
			if (!isRegistered) 
			{
				all_variables.push_back(line);
			}

			return line;
		}
		else 
		{
			syntax_error_handler();
		}

	}
	return line;
}

/**
* First checks if line has a equal sign char or not. If yes then sends the line to the assign_parser method.
* If no, then continues to parsing the line. There are five possibilities. A line can be:
* While statement, if statement, print statement, ' } ' or an empty line.
* Prints the necessary LLVM code for the 5 possibilities.
* @param line String to decide and parse
*/
void parser(string line)
{

	if (line.find('=') != string::npos)
	{
		assign_parser(line);
	}
	else if (line.substr(0, 6) == "while(")
	{
		while_opened++;

		if (if_opened != 0 || while_opened != 1) 
		{

			syntax_error_handler();
		}

		int length = line.length();
		string tempp = line.substr(6, length - 8);
		out_file << "\t" << "br label %whcond" << conditioner << endl << endl;
		out_file << "whcond" << conditioner << ":" << endl;
		string tried = add_op(tempp);

		if (!is_number(tried))
		{
			if (tried[0] != '%')
			{
				tried = "%" + tried;
				out_file << "\t" << "%_" << temp << "= load i32* " << tried << endl;
				temp++;
				out_file << "\t" << "%_" << temp << "= icmp ne i32 %_" << temp - 1 << ", 0 " << endl;
				temp++;
				out_file << "\t" << "br i1 %_" << temp - 1 << ", label %whbody" << conditioner << ", label %whend" << conditioner << endl << endl;

				out_file << "whbody" << conditioner << ":" << endl;
			}
			else
			{
				out_file << "\t" << "%_" << temp << "= icmp ne i32 " << tried << ", 0 " << endl;
				temp++;
				out_file << "\t" << "br i1 %_" << temp - 1 << ", label %whbody" << conditioner << ", label %whend" << conditioner << endl << endl;

				out_file << "whbody" << conditioner << ":" << endl;
			}
		}
		else
		{
			out_file << "\t" << "%_" << temp << "= icmp ne i32 " << tried << ", 0 " << endl;
			temp++;
			out_file << "\t" << "br i1 %_" << temp - 1 << ", label %whbody" << conditioner << ", label %whend" << conditioner << endl << endl;
			out_file << "whbody" << conditioner << ":" << endl;
		}
	}

	else if (line.substr(0, 3) == "if(")
	{
		if_opened++;

		if (if_opened != 1 || while_opened != 0) 
		{
			syntax_error_handler();
		}

		string tempp = line.substr(3, line.length() - 5);
		out_file << "\t" << "br label %" << "entry" << conditioner << endl << endl;
		out_file << "entry" << conditioner << ":" << endl;
		string tried = add_op(tempp);

		if (!is_number(tried))
		{
			if (tried[0] != '%')
			{
				tried = "%" + tried;
				out_file << "\t" << "%_" << temp << "= load i32* " << tried << endl;
				temp++;
				out_file << "\t" << "%_" << temp << "= icmp ne i32 %_" << temp - 1 << ", 0 " << endl;
				temp++;
				out_file << "\t" << "br i1 %_" << temp - 1 << ", label %btrue" << conditioner << ", label %" << "end" << conditioner << endl << endl;
				out_file << "btrue" << conditioner << ":" << endl;
			}
			else
			{
				out_file << "\t" << "%_" << temp << "= icmp ne i32 " << tried << ", 0 " << endl;
				temp++;
				out_file << "\t" << "br i1 %_" << temp - 1 << ", label %btrue" << conditioner << ", label %" << "end" << conditioner << endl << endl;
				out_file << "btrue" << conditioner << ":" << endl;
			}
		}
		else 
		{
			out_file << "\t" << "%_" << temp << "= icmp ne i32 " << tried << ", 0 " << endl;
			temp++;
			out_file << "\t" << "br i1 %_" << temp - 1 << ", label %btrue" << conditioner << ", label %" << "end" << conditioner << endl << endl;
			out_file << "btrue" << conditioner << ":" << endl;
		}

	}

	else if (line.substr(0, 6) == "print(")
	{
		string tempp = line.substr(6, line.length() - 7);
		string tried = add_op(tempp);

		if (!is_number(tried))
		{
			if (tried[0] != '%')
			{
				tried = "%" + tried;
				out_file << "\t" << "%_" << temp << "= load i32* " << tried << endl;
				temp++;
				out_file << "\t" << "call i32(i8*, ...)* @printf(i8* getelementptr( [4 x i8]* @print.str, i32 0, i32 0), i32 %_" << temp - 1 << ")" << endl;
			}
			else
			{
				out_file << "\t" << "call i32(i8*, ...)* @printf(i8* getelementptr( [4 x i8]* @print.str, i32 0, i32 0), i32 " << tried << ")" << endl;
			}
		}
		else
		{
			out_file << "\t" << "call i32(i8*, ...)* @printf(i8* getelementptr( [4 x i8]* @print.str, i32 0, i32 0), i32 " << tried << ")" << endl;
		}
	}

	else if (line[0] == '}')
	{
		if (if_opened == 1)
		{
			out_file << "\t" << "br label %" << "end" << conditioner << endl << endl;
			if_opened--;
			out_file << "end" << conditioner << ":" << endl;
			conditioner++;
		}
		else if (while_opened == 1)
		{
			out_file << "\t" << "br label %whcond" << conditioner << endl << endl;
			while_opened--;
			out_file << "whend" << conditioner << ":" << endl;
			conditioner++;
		}
		else 
		{
			syntax_error_handler();
		}
	}
	else if (line.size() == 0)
	{
	}
	else
	{
		syntax_error_handler();
	}
}

/**
* Handles choose expressions.
* @param exp string to process.
*/
void choose_handler(string exp)
{
	int comma = 0;
	int par = 1;
	for (int i = 0; i < exp.size(); i++)
	{
		if (exp[i] == '(')
		{
			par++;
		}
		if (exp[i] == ')')
		{
			par--;
		}
		if (exp[i] == ',' && par == 1)
		{
			comma++;
		}
	}

	if (comma != 3)
	{
		syntax_error_handler();
	}
	int index[3];
	int a = 0;
	par = 1;
	for (int i = 0; i < exp.size(); i++)
	{

		if (exp[i] == '(')
		{

			par++;
		}
		if (exp[i] == ')')
		{
			par--;
		}

		if (exp[i] == ',' && par == 1)
		{
			index[a] = i;
			a++;
		}

	}

	out_file << "\t" << "br label %" << "entry" << conditioner1 << endl << endl;
	out_file << "entry" << conditioner1 << ":" << endl;

	string tempp = exp.substr(0, index[0]);   
		cout<<temp<<endl;

	string tried = add_op(tempp);

	if (!is_number(tried))
	{
		if (tried[0] != '%')
		{

			tried = "%" + tried;
			out_file << "\t" << "%_" << temp << "= load i32* " << tried << endl;
			temp++;
			out_file << "\t" << "%_" << temp << "= icmp ne i32 %_" << temp - 1 << ", 0 " << endl;
			temp++;
			out_file << "\t" << "br i1 %_" << temp - 1 << ", label %bnotzero" << conditioner1 << ", label %bzero" << conditioner1 << endl << endl;

			out_file << "bnotzero" << conditioner1 << ":" << endl;


			out_file << "\t" << "%_" << temp << "= load i32* " << tried << endl;
			temp++;
			out_file << "\t" << "%_" << temp << "= icmp sgt i32 %_" << temp - 1 << ", 0 " << endl;
			temp++;
			out_file << "\t" << "br i1 %_" << temp - 1 << ", label %bpositive" << conditioner1 << ", label %bnegative" << conditioner1 << endl << endl;

			out_file << "bzero" << conditioner1 << ":" << endl;

		}
		else
		{

			out_file << "\t" << "%_" << temp << "= icmp ne i32 " << tried << ", 0 " << endl;
			temp++;
			out_file << "\t" << "br i1 %_" << temp - 1 << ", label %bnotzero" << conditioner1 << ", label %bzero" << conditioner1 << endl << endl;

			out_file << "bnotzero" << conditioner1 << ":" << endl;

			out_file << "\t" << "%_" << temp << "= icmp sgt i32 " << tried << ", 0 " << endl;
			temp++;
			out_file << "\t" << "br i1 %_" << temp - 1 << ", label %bpositive" << conditioner1 << ", label %bnegative" << conditioner1 << endl << endl;

			out_file << "bzero" << conditioner1 << ":" << endl;

		}

	}
	else
	{

		out_file << "\t" << "%_" << temp << "= icmp ne i32 " << tried << ", 0 " << endl;
		temp++;
		out_file << "\t" << "br i1 %_" << temp - 1 << ", label %bnotzero" << conditioner1 << ", label %bzero" << conditioner1 << endl << endl;
		out_file << "bnotzero" << conditioner1 << ":" << endl;

		out_file << "\t" << "%_" << temp << "= icmp sgt i32 " << tried << ", 0 " << endl;
		temp++;
		out_file << "\t" << "br i1 %_" << temp - 1 << ", label %bpositive" << conditioner1 << ", label %bnegative" << conditioner1 << endl << endl;
		out_file << "bzero" << conditioner1 << ":" << endl;

	}

	string zero = add_op(exp.substr(index[0] + 1, index[1] - index[0] - 1));
	cout<<zero<<endl;

	if (zero[0] != '%' && !is_number(zero))
	{
		zero = "%" + zero;
		out_file << "\t" << "%_" << temp << "= load i32* " << zero << endl;
		temp++;
		out_file << "\t" << "store i32 %_" << temp - 1 << ", i32* %" << "chooseresult" << choose_result << endl;
	}
	else
	{

		out_file << "\t" << "store i32 " << zero << ", i32* %" << "chooseresult" << choose_result << endl;

	}

	out_file << "\t" << "br label %bend" << conditioner1 << endl << endl;

	out_file << "bpositive" << conditioner1 << ":" << endl;

	string pos = add_op(exp.substr(index[1] + 1, index[2] - index[1] - 1));
		cout<<exp.substr(index[1] + 1, index[2] - index[1] - 1)<<endl;


	if (pos[0] != '%' && !is_number(pos))
	{
		pos = "%" + pos;
		out_file << "\t" << "%_" << temp << "= load i32* " << pos << endl;
		temp++;
		out_file << "\t" << "store i32 %_" << temp - 1 << ", i32* %" << "chooseresult" << choose_result << endl;
	}
	else
	{
		out_file << "\t" << "store i32 " << pos << ", i32* %" << "chooseresult" << choose_result << endl;
	}

	out_file << "\t" << "br label %bend" << conditioner1 << endl << endl;
	out_file << "bnegative" << conditioner1 << ":" << endl;

	string neg = add_op(exp.substr(index[2] + 1, exp.size() - index[2] - 1));
		cout<<neg<<endl;


	if (neg[0] != '%' && !is_number(neg))
	{
		neg = "%" + neg;
		out_file << "\t" << "%_" << temp << "= load i32* " << neg << endl;
		temp++;
		out_file << "\t" << "store i32 %_" << temp - 1 << ", i32* %" << "chooseresult" << choose_result << endl;
	}
	else
	{
		out_file << "\t" << "store i32 " << neg << ", i32* %" << "chooseresult" << choose_result << endl;
	}

	out_file << "\t" << "br label %bend" << conditioner1 << endl << endl;
	out_file << "bend" << conditioner1 << ":" << endl;
	conditioner1++;
}

/**
* Finds choose statements in a line.
* @param line String to process.
* @return Processed line.
*/
string choose_finder(string line)
{
	while (line.find("choose(") != string::npos)
	{
		for (int i = line.size() - 7; i >= 0; i--)
		{
			if (line.substr(i, 7) == "choose(")
			{
				int par_count = 1;

				for (int j = i + 7; j < line.size(); j++)
				{
					if (line[j] == '(')
					{
						par_count++;
					}
					if (line[j] == ')')
					{
						par_count--;
					}
					if (par_count == 0)
					{
						choose_handler(line.substr(i + 7, j - i - 7));
						line = line.substr(0, i) + "chooseresult" + to_string(choose_result) + line.substr(j + 1, line.size() - j - 1);
						choose_result++;
						break;
					}
				}
				break;
			}
		}
	}
	return line;
}

int main(int argc, char* argv[])
{
	string file_name = argv[1]; // Open input and output files.
	
	string a= argv[1];
	a=a.substr(0, a.size()-2);
	a=a+"ll";
	in_file.open(file_name);
	out_file.open("file.txt");
	out_file_two.open(a);
	in_file_two.open("file.txt");

	take_all_lines(); // Take all lines of the given code and push back into a global vector named all_lines.

	int i = 0;

	while (i < all_lines.size()) // Iterate through all the lines.
	{
		white_space_eraser(i); // Erase white spaces.
		string line = all_lines[i];
		comment_eraser(i); // Erase comments.
		syntax_checker(i); // Check syntax.
		parser(all_lines[i]); // Parse the code.

		i++;
		line_count++; // Record the line number.
	}

	if (while_opened != 0 || if_opened != 0)
	{
		out_file_two << "; ModuleID = 'mylang2ir'" << endl;
		out_file_two << "declare i32 @printf(i8*, ...)" << endl; // declare i32 @printf(i8*, ...)
		out_file_two << "@print.str = constant [23 x i8] c\"Line %d: syntax error\\0A\\00\""<< endl;
		out_file_two << endl;
		out_file_two << "define i32 @main() {" << endl;
		out_file_two << "call i32 (i8*, ...)* @printf(i8* getelementptr ([23 x i8]* @print.str, i32 0, i32 0), i32 "<<line_count-1<<")";
		out_file_two << endl;
		out_file_two << "\t" << "ret i32 0" << endl;
		out_file_two << "}";
	
		remove("file.txt");
		exit(0);
	}

	print_module(); // Print the first lines of the LLVM code.

	for (int i = 0; i < all_variables.size(); i++) // Allocates all of the variables.
	{
		out_file_two << "%" << all_variables[i] << "= alloca i32" << endl;
	}

	out_file_two << endl;

	for (int i = 0; i < all_variables.size(); i++) // Stores all of the variables.
	{
		out_file_two << "store i32 0, i32* %" << all_variables[i] << endl;
	}

	out_file_two << endl;

	string line_two;

	while (getline(in_file_two, line_two))
	{
		out_file_two << line_two << endl;
	}

	remove("file.txt");

	out_file_two << "\t" << "ret i32 0" << endl; // End of the LLVM file.
	out_file_two << "}";

	return 0;
}
