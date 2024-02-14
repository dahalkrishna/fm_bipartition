Implementation of the Fiduccia-Mattheyeses (FM) algorithm to improve the network partitions. The program reads the ISCAS85 (*.bench) files. The algorithm makes use of linked list and bucket list data structures.The code was implemented as a part of coursework.
The results for the various ISCAS85 circuits are:

|Circuit|Initial Cut|Best Cut|
|:---:|:---:|:---:|
|c17|5|2|
|c880|58|35|
|c1908|136|83|
|c3540|303|105|
|c5315|402|181|
|c6288|179|49|
|c7552|497|274|

Compile the code using **make** command and execute the code using **./project c17.bench**.
