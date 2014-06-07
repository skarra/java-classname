/*
 * main method lies in second class.
 */

import java.io.*;
import java.util.*;
import java.util.regex.*;
import java.text.*;
import java.math.*;//}}}

class AGWrong
{
}

class AG {
    public static void main(String [] args) throws Exception
    {
        BufferedReader br = new BufferedReader(new InputStreamReader(System.in));

        String [] tmp = br.readLine().split(" ");
        int n = Integer.parseInt(tmp[0]);
        int k = Integer.parseInt(tmp[1]);

        tmp = br.readLine().split(" ");
        Integer [] arr = new Integer[n];
        for(int i = 0; i < n; i++) {
            arr[i] = Integer.parseInt(tmp[i]);
        }
        Arrays.sort(arr);

        int cnt = 0;

        int idx = 0, jdx = 0;

        while(idx < n && jdx < n) {
            if(arr[jdx] - arr[idx] == k) {
                cnt++;
                idx++;
                jdx++;
            }
            else if(arr[jdx] - arr[idx] < k) {
                jdx++;
            }
            else {
                idx++;
            }
        }
        System.out.println(cnt);
    }
}
