#include <fstream>/*{{{*/
#include <chrono>
#include <iostream>
#include <iomanip>
#include <cerrno>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
using namespace boost;
using namespace std;/*}}}*/

bool debug = false;/*{{{*/
string usage = "Usage: ./extract file.java [--repeat=<n>] [--debug]";

typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::milliseconds ms;
typedef std::chrono::duration<float> fsec;/*}}}*/

inline bool isBraces(const char &ch){/*{{{*/
    return    ch == '{' || ch == '(' || ch == '['
           || ch == '}' || ch == ')' || ch == ']';
}/*}}}*/

string get_file_contents(const char *filename)/*{{{*/
{
    ifstream in(filename, ios::in | ios::binary);
    if (in) {
        string contents;
        in.seekg(0, ios::end);
        contents.resize(in.tellg());
        in.seekg(0, ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return(contents);
    }

    throw(errno);
}/*}}}*/

/*
 * Parser class: will be used to store intermediate steps after:
 * 1. Comment and string stripping.
 * 2. Tokenize strings.
 * 2. Removing 2nd and further level of code block.
*/
class Parser {/*{{{*/
    public:
        Parser(const string &str) : code(str){
            strippedCode = "";
            tokens.clear();
        }
        string runParser();
    private:
        void            stripCommentsAndStrings();
        void            tokenize();
        string          getClassName();
        const string    &code;
        string          strippedCode;
        vector <string> tokens;
};/*}}}*/

/*
 * Removes all single-line comment, multi-line comment
 * and strings.
 *
 * Precedence order:
 *  single-line comment == multi-line comment == string
 *
 * 1. Since all of them have same priority, whoever comes
 *    first will be removed ignoring others.
 *
 * 2. Strings will be purged into empty strings, "".
 *    Take care if nested quotes ("\"") and characters '\"', '\'', '*'.
*/
void Parser::stripCommentsAndStrings() {/*{{{*/
    int length = (int)code.size();
    int idx = 0;
    strippedCode.reserve(length);

    while(idx < length) {

        // Check for single line comment
        if(idx+1 < length && code[idx] == '/' && code[idx+1] == '/') {
            idx += 2;
            while(idx < length && code[idx] != '\n')
                idx++;
            strippedCode += '\n';
            idx++;
        }

        // Check for multi line comment
        else if(idx+1 < length && code[idx] == '/' && code[idx+1] == '*') {
            idx += 2;
            while(idx+1< length && (code[idx] != '*' || code[idx+1] != '/' ))
                idx++;
            idx += 2;
        }

        /*
         * // Check for character '\"', and replace it with '-'.
         *else if (idx+3 < length && code[idx] == '\'' &&
         *         code[idx+1] == '\\' && code[idx+2] == '\"'
         *         && code[idx+3] == '\'') {
         *    strippedCode += "\'-\'";
         *    idx+=4;
         *}
         */

        // Check for any escaped character '\"', and replace it with '-'.
        else if (idx+3 < length && code[idx] == '\'' &&
                 code[idx+1] == '\\' && code[idx+3] == '\'') {
            strippedCode += "\'-\'";
            idx+=4;
        }

        // Check for any non-escaped character '*', and replace it with '-'.
        else if (idx+2 < length && code[idx] == '\'' &&
                 code[idx+1] != '\\' && code[idx+2] == '\'') {
            strippedCode += "\'-\'";
            idx+=3;
        }

        // Check for strings
        else if(code[idx] == '\"') {
            strippedCode += '\"';
            idx++;

            while(idx < length) {

                // Check for escaped `\`, otherwise it will escape closing quotes "\\".
                if(idx+1 < length && code[idx] == '\\' && code[idx+1] == '\\')
                    idx += 2;
                // Check for escaped `\"`.
                else if(idx+1 < length && code[idx] == '\\' && code[idx+1] == '\"')
                    idx += 2;
                else if (code[idx] == '\"')
                    break;
                else
                    idx++;
            }
            idx++;
            strippedCode += '\"';
        }
        else {
            strippedCode += code[idx];
            idx++;
        }
    }
}/*}}}*/

/*
 * strippedCode will be tokenzed here.
 * Delimiters will be:
 *  whitespace, braces and semicolons
*/
void Parser::tokenize() {/*{{{*/
    int idx = 0;
    int len = (int)strippedCode.size();

    while(idx < len) {
        char ch = strippedCode[idx];

        // dropWhile isSpace strippedCode
        if(isspace(ch)) {
            while(idx < len && isspace(strippedCode[idx]))
                idx++;
        }

        // consider all braces and semicolons as delimiter 
        else if(isBraces(ch) || ch == ';') {
            tokens.push_back("");
            tokens.back() += ch;
            idx++;
        }

        // consider all non-space & non-semicolon characters
        else {
            tokens.push_back("");
            while(idx < len && !isspace(strippedCode[idx]) && 
                  !isBraces(strippedCode[idx]) && strippedCode[idx] != ';') {
                tokens.back() += strippedCode[idx];
                idx++;
            }
        }
    }
}/*}}}*/

/*
 * It will iterate through all tokens.
 * Will maintain depth for each type of bracket.
 *
 * 1. Avoid anything having parentheses-level > 1 or any
 *    other bracket-level > 0.
 *
 * 2. If parentheses-level = 0, then public class is solution.
 *    It will also avoid nested public class.
*/
string Parser::getClassName() {/*{{{*/
    int parentheses    = 0;
    int braces         = 0;
    int squareBrackets = 0;

    int idx = 0;
    int len = (int)tokens.size();
    string ret = "null";

    // Check for public class only.
    while(idx < len) {
        /* 
         * Ideally it should throw error. Should enable some sort of
         * error-reporting if this occurs.
        */
        if(parentheses < 0 || braces < 0 || squareBrackets < 0)
            return ret;

        /*
         * Check for public class. Make sure that parentheses == 0
         * so that it ignores nested class.
        */
        else if( idx+2 < len && parentheses + braces + squareBrackets == 0
              && tokens[idx] == "public" && tokens[idx+1] == "class")
            return tokens[idx+2];

        /*
         *else if( idx+3 < len && parentheses == 1 && braces + squareBrackets == 0
         *         && ((tokens[idx] == "public" && tokens[idx+1] == "static")
         *         || (tokens[idx] == "static" && tokens[idx+1] == "public")))
         *    return ret;
         */

        else if (tokens[idx] == "(")
            parentheses++;
        else if (tokens[idx] == ")")
            parentheses--;
        else if (tokens[idx] == "{")
            braces++;
        else if (tokens[idx] == "}")
            braces--;
        else if (tokens[idx] == "[")
            squareBrackets++;
        else if (tokens[idx] == "]")
            squareBrackets--;

        idx++;
    }

    idx = 0;
    parentheses = braces = squareBrackets = 0;

    // Check for class which have public static void main
    while(idx < len) {
        /*
         *if(debug && idx+9 < len && braces == 1 && parentheses + squareBrackets == 0
         *         && (   (tokens[idx] == "public" && tokens[idx+1] == "static")
         *             || (tokens[idx] == "static" && tokens[idx+1] == "public"))
         *         && tokens[idx+2] == "void" && tokens[idx+3] == "main"
         *         && tokens[idx+4] == "(" && tokens[idx+5] == "String"
         *         && (   (tokens[idx+6] == "[" && tokens[idx+7] == "]")
         *             || (tokens[idx+7] == "[" && tokens[idx+8] == "]"))
         *         && tokens[idx+9] == ")")
         *    printf("%d-%d: %s %s %s %s %s - %s\n\n", idx, braces, tokens[idx].c_str(), tokens[idx+1].c_str()
         *            , tokens[idx+2].c_str(), tokens[idx+3].c_str(), tokens[idx+4].c_str(), ret.c_str());
         */

        /* 
         * Ideally it should throw error. Should enable some sort of
         * error-reporting if this occurs.
        */
        if(parentheses < 0 || braces < 0 || squareBrackets < 0)
            return ret;

        // Check for PSVM and return its parent class.
        else if( idx+9 < len && braces == 1 && parentheses + squareBrackets == 0
                 && (   (tokens[idx] == "public" && tokens[idx+1] == "static")
                     || (tokens[idx] == "static" && tokens[idx+1] == "public"))
                 && tokens[idx+2] == "void" && tokens[idx+3] == "main"
                 && tokens[idx+4] == "(" && tokens[idx+5] == "String"
                 && (   (tokens[idx+6] == "[" && tokens[idx+7] == "]")
                     || (tokens[idx+7] == "[" && tokens[idx+8] == "]"))
                 && tokens[idx+9] == ")")
            return ret;

        else if (tokens[idx] == "(")
            parentheses++;
        else if (tokens[idx] == ")")
            parentheses--;
        else if (tokens[idx] == "{")
            braces++;
        else if (tokens[idx] == "}")
            braces--;
        else if (tokens[idx] == "[")
            squareBrackets++;
        else if (tokens[idx] == "]")
            squareBrackets--;

        // Ignore deeper blocks
        else if (braces > 1)
            ;
        else if(squareBrackets > 0 || parentheses > 0)
            ;

        // Select parent class
        else if (   idx + 2 < len && braces == 0 
                 && tokens[idx] == "class" && tokens[idx+2] == "{")
            ret = tokens[idx+1];

        /*
         *if ( debug &&  idx + 2 < len && braces == 0 
         *         && tokens[idx] == "class" && tokens[idx+2] == "{")
         *    printf("%d-%d: %s %s\n", idx, braces, tokens[idx].c_str(), tokens[idx+1].c_str());
         */

        idx++;
    }

    return ret;
}/*}}}*/

/*
 * Wrapper public function, will serve as interface
*/
string Parser::runParser() {/*{{{*/
    if(debug) {
        cout << "Original contents:\n";
        cout << code << endl;
    }

    stripCommentsAndStrings();
    if(debug) {
        cout << "Stripped contents:\n";
        cout << strippedCode << endl;
    }

    tokenize();
    if(debug) {
        cout << "Tokens:\n";
        for(auto it = (tokens).begin(); it != (tokens).end(); ++it) {
            cout << "|" << *it << "|-";
        }
        cout << endl;
    }

    return getClassName();
}/*}}}*/

int main (int argc, char *argv[])/*{{{*/
{
    /*
     * Parse command line arguments
     */

    if (argc <= 1) {
        cerr << "Usage: " << usage << endl;
        exit(1);
    }

    int    repeat = 1;
    smatch smatches;

    regex  r("--repeat=(\\d+)");
    regex  e("--debug");

    for (int i=2; i < argc; ++i) {
        if (regex_search(string(argv[i]), smatches, r)) {
            repeat = atoi(string(smatches[1]).c_str());
        } else if (regex_search(string(argv[i]), smatches, e)) {
            debug = true;
        }
    }

    string basename, classname;
    auto t0 = Time::now();

    /*
     * Parse the given java source file, and repeat as many times as requested
     */

    for (int i = 0; i < repeat; ++i) {
        string contents = get_file_contents(argv[1]);
        classname = Parser(contents).runParser();

        filesystem::path p(argv[1]);
        basename = filesystem::basename(p);
    }
    auto t1 = Time::now();
    fsec fs = t1 - t0;
    ms d = chrono::duration_cast<ms>(fs);

    /*
     * Compare correctness of the output and show the result. Note that as the
     * output does not really change every run we do not have to do this
     *  inside the loop
     */

    string pass;

    cout << "Test Filename: " << setw(55) << setfill('.') << left << argv[1];

    if (debug) {
        cout << endl;
        cout << "Original ClassName = " << basename  << endl;
        cout << "className Found    = " << classname << endl;
    }

    if (basename == classname) {
        pass = "Passed.";
    } else {
        pass = "FAILED (" + classname + "). ";
    }
    cout << left << setw(20) << setfill('.') << pass;

    if (repeat > 1) {
        cout << "Total Time: " << setw(7) << fixed << right
            << setprecision(5) << fs.count() << " sec. "
            << "Per Iter: " << setw(6) << fixed << right << setprecision(2)
            << d.count()*1000.0/repeat << " nanosec";
    }

    cout << endl;
}/*}}}*/

