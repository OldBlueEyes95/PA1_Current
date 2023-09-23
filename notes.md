Current tests state:
```
vboxuser@CSCE-Ubuntu:~/Documents/PA1_Local$ bash pa1-all-tests.sh 
To remove colour from tests, set COLOUR to 1 in sh file


Verify the server runs as child

  Test One Passed

Test cases for single datapoint transfers

Testing :: ./client -p 12 -t 0.016 -e 1

  Test Two Passed

Testing :: ./client -p 5 -t 59.996 -e 2

  Failed

Testing :: ./client -p 9; diff -sqwB <(head -n 1000 BIMDC/9.csv) received/x1.csv

  Test Four Passed

Testing :: ./client -p 5; diff -sqwB <(head -n 1000 BIMDC/5.csv) received/x1.csv

  Test Five Passed

Testing :: ./client -f 8.csv; diff BIMDC/8.csv received/8.csv

  Test Six Passed

Testing :: ./client -f 2.csv; diff BIMDC/2.csv received/2.csv

  Test Seven Passed

Test cases for CSV file transfers (with varying buffer capacity)

Testing :: ./client -f 11.csv -m 512; diff BIMDC/11.csv received/11.csv

  No 11.csv in received/ directory

Testing :: truncate -s 256K BIMDC/test.bin; ./client -f test.bin; diff BIMDC/test.bin received/test.bin

  Test Nine Passed

Testing :: truncate -s 256K BIMDC/test.bin; ./client -f test.bin -m 512; diff BIMDC/test.bin received/test.bin

  No test.bin in received/ directory

Testing :: ./client -c -p 12 -t 0.016 -e 1

  No new channel opened

Testing :: ./client -c -f 10.csv -m 1024; diff BIMDC/10.csv received/10.csv

  No 10.csv in received/ directory

Failed to close FIFORequestChannel
  Failed

SCORE: 44/80
```