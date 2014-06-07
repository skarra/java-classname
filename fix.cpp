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
#include <boost/filesystem.hpp>
using namespace std;
using namespace boost;

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
    static const regex PUBLIC_MAIN_STR(
        "class\\s+(\\w+)((?!class).)*public\\s+static\\s+void\\s+main",
        regex::perl|regex::icase);
    static const regex PUBLIC_CLASS_STR(
        "public\\s+class\\s+(\\w+)", regex::perl|regex::icase);
    static const regex COMMENTS_STR(
        "(//.*?$)|(/\\*.*?\\*/)", regex::perl|regex::icase);

    // First strip all the comments
    contents = regex_replace(contents, COMMENTS_STR, "",
                                    format_all);

    // Now look for public class names. Just find the first one; if there
    // are more than one there is going to be a compile error any way.
    smatch smatches;
    if (regex_search(contents, smatches, PUBLIC_CLASS_STR)) {
        return smatches[1];
    }

    // Now look for any class containing a public static void main
    if (regex_search(contents, smatches, PUBLIC_MAIN_STR)) {
        return smatches[1];
    }    

    return "NotParsed";
}

int main (int argc, char *argv[])
{
    if (argc <= 1) {
        cerr << "Usage: " << usage << endl;
        exit(1);
    }

    string contents = get_file_contents(argv[1]);
    string classname = parse_for_classname(contents);

    filesystem::path p(argv[1]);
    string basename = filesystem::basename(p);

    cout << "Test Filename: " << argv[1] << "; ";

    string pass;
    if (basename == classname) {
        pass = "Passed";
    } else {
        smatch smatches;
        regex r("^NotParsed");
        if (regex_search(classname, smatches, r) &&
            regex_search(basename, smatches, r)) {
            pass = "Passed";
        } else {
            pass = ".......Failed (" + classname + ")";
        }
    }

    cout << pass << endl;
}
