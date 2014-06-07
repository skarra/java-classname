# Extracting Name of Main Java Class

The file ```extract.cpp``` contains logic to read a java source file and then extract the name of the primary class name. This is required if we get a java source submission from the client and we need to store it to disk and compile the code. As Java has strict requirements on how the files should be named based on what the file contains, it is important for us to extract the primary class name before the file can be processed by our code checker.

# In The Box

This repo contains a simple C++ file that reads in a filename from its cmd line, extracts the primary class name, compares it against the actual filename to determine if the extraction was successful or not, and prints an approprite message

We also have a bunch of test cases and a Makefile to make it easy to run the code against the test cases.

# How to Use It

### Build the parser
```
make clean && make build
```

### Execute all test cases
```
make
```

### Execute all test cases 1000 times
```
make REPEAT=1000
```

### Execute a test cases of your own
```
make build
./extract test_file.java
./extract test_file.java --repeat=1000
```

### Sample run output

```
Srirams-MacBook-Pro:java-class sriramkarra$ make clean && make REPEAT=1000
rm -f *~ ./extract *.o
g++ -O2 -std=c++11 -Wall -Wno-c++11-extensions extract.cpp -lboost_regex -lboost_filesystem -lboost_system -o ./extract

== Repeating every test 1000 times ==

Test Filename: tests/AA.java............Passed.        Total Time: 0.01787 sec. Per Iter:  17.00 nanosec
Test Filename: tests/AB.java............Passed.        Total Time: 0.02064 sec. Per Iter:  20.00 nanosec
Test Filename: tests/AC.java............Passed.        Total Time: 0.02719 sec. Per Iter:  27.00 nanosec
Test Filename: tests/AD.java............Passed.        Total Time: 0.02742 sec. Per Iter:  27.00 nanosec
Test Filename: tests/AE.java............Passed.        Total Time: 0.02044 sec. Per Iter:  20.00 nanosec
Test Filename: tests/AF.java............Passed.        Total Time: 0.15858 sec. Per Iter: 158.00 nanosec
Test Filename: tests/AG.java............Passed.        Total Time: 0.16985 sec. Per Iter: 169.00 nanosec
Test Filename: tests/AH.java............Passed.        Total Time: 0.04696 sec. Per Iter:  46.00 nanosec
Test Filename: tests/NotParsed-01.java..Passed.        Total Time: 0.02807 sec. Per Iter:  28.00 nanosec

Srirams-MacBook-Pro:java-class sriramkarra$ 
```
# Instructions on ading new test cases

Create a new file in the ```tests``` directory. Name the file in in serial order (like AZ.java if it is contains a valid public class name. If the test file does not contain a valid filename, or if it is otherwise a negative test case, give it a name of the form ```NotParsed-22.java```.  That way the Makefile can be used to automatcially run all tests cases and check for success.