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
#include <chrono>
#include <iostream>
#include <iomanip>
#include <cerrno>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
using namespace std;
using namespace boost;

string usage = "Usage: ./fix file.java";

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

    int repeat = 1;
    smatch smatches;
    regex r("--repeat=(\\d+)");
    if (argc >= 3 && regex_search(string(argv[2]), smatches, r)) {
        repeat = atoi(string(smatches[1]).c_str());
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

    cout << "Test Filename: " << setw(25) << setfill('.') << left << argv[1] << "\n";
    string pass;
    cout << "Original ClassName = " << basename  << "\n";
    cout << "className Found    = " << classname << "\n";
    if (basename == classname) {
        pass = "Passed.";
    } else {
        regex r("^NotParsed");
        if (regex_search(classname, smatches, r) &&
            regex_search(basename, smatches, r)) {
            pass = "Passed.";
        } else {
            pass = "FAILED (" + classname + "). ";
        }
    }
    cout << left << setw(15) << setfill(' ') << pass;
	cout << "\n";

    // cout << endl << "basenmae: " << basename << "; class: " << classname;

    if (repeat > 0) {
        cout << "Total Time: " << setw(7) << fixed << right << setprecision(5)
             << fs.count() << " sec. "
             << "Per Iter: " << setw(6) << fixed << right << setprecision(2)
             << d.count()*1000.0/repeat << " nanosec";
    }

    cout <<  "\n" << endl;
}
