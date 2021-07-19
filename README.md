# MTFileOrganizer
A multithreaded file organizer which utilizes a thread pool to sort files by extension.

I made this program to help me organize the recovered 2TB of lost files from a formatted hard drive. The program uses a generic thread pooling implementation, which can post functions to the thread pool as jobs for the threads to finish.

I relied on a Stack Overflow answer supplied by PhD AP EcE (https://stackoverflow.com/questions/15752659/thread-pooling-in-c11) in order to understand the concept and implementation of a thread pool in C++.
