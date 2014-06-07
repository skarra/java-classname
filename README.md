# Extracting Name of Main Java Class

The file fix.cpp contains logic to read a java source file and then extract the name of the primary class name. This is required if we get a java source submission from the client and we need to store it to disk and compile the code. As Java has strict requirements on how the files should be named based on what the file contains, it is important for us to extract the primary class name before the file can be processed by our code checker.

# In The Box

This repo contains a simple C++ file that reads in a filename from its cmd line, extracts the primary class name, compares it against the actual filename to determine if the extraction was successful or not, and prints an approprite message

We also have a bunch of test cases and a Makefile to make it easy to run the code against the test cases.

# How to Use It

### Build the parser
```
make clean
make build
```

### Execute all test cases
```
make
```

### Execute a test cases of your own
```
make build
fix <test_file.java>
```
# Instructions on ading new test cases

Create a new file in the ```tests``` directory. Name the file in in serial order (like AZ.java if it is contains a valid public class name. If the test file does not contain a valid filename, or if it is otherwise a negative test case, give it a name of the form ```NotParsed-22.java```.  That way the Makefile can be used to automatcially run all tests cases and check for success.