/*
 * Author: Sriram Karra <sriram@hackerrank.com>
 *
 * Copyright (C) 2014, Interviewstreet Inc.
 * All Rights Reserved
 */

/*
 * Usage: ./fix file.java
 *
 * In the specified java source find the main class name. The main class name
 * is either (a) the one marked as public, or (b) the class containing main 
 */

#include <fstream>
#include <iostream>
#include <cerrno>
#include <boost/regex.hpp>
using namespace std;

string usage = "Usage: ./fix file.java";

string get_file_contents(const char *filename)
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
}

/*
 * 
 */
string parse_for_classname (string& contents)
{
    static const boost::regex PUBLIC_MAIN_STR(
        "(public\\s+static|static\\s+public)\\s+void\\s+main");
    static const boost::regex PUBLIC_CLASS_STR(
        "public\\s+class\\s+(\\w+)");
    static const boost::regex COMMENTS_STR(
        "(//.*?$)|(/\\*.*?\\*/)", boost::regex::perl|boost::regex::icase);

    // First strip all the comments
    contents = boost::regex_replace(contents, COMMENTS_STR, "",
                                    boost::format_all);

    // cout << "----" << endl;
    // cout << contents;
    // cout << "----" << endl;
        
    // First look for public class names. Just find the first one; if there
    // are more than one there is going to be a compile error any way.

    boost::smatch smatches;
    if (boost::regex_search(contents, smatches, PUBLIC_CLASS_STR)) {
        return smatches[1];
    }

    return "Not parsed";
}

int main (int argc, char *argv[])
{
    if (argc <= 1) {
        cerr << "Usage: " << usage << endl;
        exit(1);
    }

    string contents = get_file_contents(argv[1]);
    string classname = parse_for_classname(contents);

    cout << "Test Filename: " << argv[1];
    cout << "; Main classname is: " << classname << endl;
}
