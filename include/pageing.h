#ifndef __PAGING__H
#define __PAGING__H
// struct page_directory {
// 	uint32_t physical; // physical address of page tables (cr3)
// 	// stores mount point of page directory in this directory
// 	uint32_t* directory;
// 	uint32_t* pages;
// };
// typedef struct page_directory page_directory_t;
extern void loadPageDirectory(unsigned int*);
extern void enablePaging();
extern void enablePaging2();
void create_blank_pageing_dictionary();
void create_first_page();
void enable();
#endif