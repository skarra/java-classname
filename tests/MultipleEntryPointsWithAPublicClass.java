//Author: Abhiranjan Kumar//{{{
//Problem Curator, Developer @ HackerRank

import java.io.*;
import java.util.*;
import java.util.regex.*;
import java.text.*;
import java.math.*;//}}}

public class MultipleEntryPointsWithAPublicClass
{
    public static void main(String [] args) throws Exception
    {
        String str = "abc";
        System.out.println("abc");

    }

}

class abc {
    public static void main(String [] args) {
        System.out.println("bcd");
    }
}
