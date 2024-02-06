#ifndef __PAGING__H
#define __PAGING__H
extern void loadPageDirectory(unsigned int*);
extern void enablePaging();
void create_blank_pageing_dictionary();
void create_first_page();
void enable();
#endif