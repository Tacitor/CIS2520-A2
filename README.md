# CIS*2520 F23 Assignment 2

## Name 

    Lukas Krampitz

## Student Number

    1192170

## Resource Used for Assistance

    - Made use of provided code from llloadonly_main.c provided in the repository for this assignment
    - Reused some code for freeing and managing Linked Lists and memory from my solution for A1

## State of Implementation

    Complete

## Summary of Learned Outcomes

    There does not seem to be a significate difference between the times needed for arraydouble and the llheadtail, as both can index the last element in the structure on O(1) time.
    What is interesting to note is how much incredibly slower llheadonly is to run. When running the program with 5 repititions with the file located at /home/courses/cis2520/data/uniprot_sprot-100000.fasta it took 767.123 seconds with an average of 2 min and 33.425 seconds. This, is orders of magnitude slower than the other data structures. This does however make sense because iterating over the list takes O(n) time. Also worthy of note is that one can watch the progress grind to a halt as the program runs. Each successive '.' that gets printed to stdout indicating the progress of the program takes longer and longer than the previous one as it takes longer to iterate over the LL and reach the end.
