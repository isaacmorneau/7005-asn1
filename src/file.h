/*
 * HEADER FILE: file.h - Kernel level file descriptor copying
 *
 * PROGRAM: 70050Asn1
 *
 * DATE: Oct 6, 2017
 *
 * FUNCTIONS:
 *  int kernel_copy(int infd, int outfd);
 *
 * DESIGNER: Isaac Morneau
 *
 * PROGRAMMER: Isaac Morneau
 */
#ifndef FILE_H
#define FILE_H

int kernel_copy(int infd, int outfd);

#endif
