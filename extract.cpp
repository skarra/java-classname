/*
 * Author: Sriram Karra <sriram@hackerrank.com>
 *
 * Copyright (C) 2014, Interviewstreet Inc.
 * All Rights Reserved
 */

/*
 * In the specified java source find the main class name. The main class name
 * is either (a) the one marked as public, or (b) the class containing main 
 */

#include <fstream>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <cerrno>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
using namespace std;
using namespace boost;

string usage = "Usage: ./extract file.java [--repeat=<n>] [--debug]";

typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::milliseconds ms;
typedef std::chrono::duration<float> fsec;

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

string parse_for_classname (string& contents)
{
    static const regex STR_STR("\\\"[^\\R]*?\\\"");
    static const regex COMMENTS_STR("(//.*?$)|(/\\*.*?\\*/)");
    static const regex PUBLIC_CLASS_STR("public\\s+class\\s+(\\w+)");
    static const regex PUBLIC_STATIC_MAIN_STR(
        "class\\s+(\\w+)((?!class).)*public\\s+static\\s+void\\s+main");
    static const regex STATIC_PUBLIC_MAIN_STR(
        "class\\s+(\\w+)((?!class).)*static\\s+public\\s+void\\s+main");

    // Strip all the quoted strings
    contents = regex_replace(contents, STR_STR, "", format_all);

    // Strip all the comments
    contents = regex_replace(contents, COMMENTS_STR, "", format_all);

    // cout << contents;
    // exit(1);

    // Now look for public class names. Just find the first one; if there
    // are more than one there is going to be a compile error any way.
    smatch smatches;
    if (regex_search(contents, smatches, PUBLIC_CLASS_STR)) {
        return smatches[1];
    }

    // Now look for any class containing a public static void main
    if (regex_search(contents, smatches, PUBLIC_STATIC_MAIN_STR)) {
        return smatches[1];
    } else if (regex_search(contents, smatches, STATIC_PUBLIC_MAIN_STR)) {
        return smatches[1];
    }


    return "null";
}

int main (int argc, char *argv[])
{
    /*
     * Parse command line arguments
     */

    if (argc <= 1) {
        cerr << "Usage: " << usage << endl;
        exit(1);
    }

    int    repeat = 1;
    int    debug  = false;
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
        classname = parse_for_classname(contents);

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

    cout << "Test Filename: " << setw(35) << setfill('.') << left << argv[1];

    if (debug) {
        cout << endl;
        cout << "Original ClassName = " << basename  << endl;
        cout << "className Found    = " << classname << endl;
    }

    if (basename == classname) {
        pass = "Passed.";
    } else {
        regex r("^null");
        if (regex_search(classname, smatches, r) &&
            regex_search(basename, smatches, r)) {
            pass = "Passed.";
        } else {
            pass = "FAILED (" + classname + "). ";
        }
    }
    cout << left << setw(20) << setfill('.') << pass;

    if (repeat > 1) {
        cout << "Total Time: " << setw(7) << fixed << right
             << setprecision(5) << fs.count() << " sec. "
             << "Per Iter: " << setw(6) << fixed << right << setprecision(2)
             << d.count()*1000.0/repeat << " nanosec";
    }

    cout << endl;
}
