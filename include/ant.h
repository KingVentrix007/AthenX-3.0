#ifndef ANT_H
#define ANT_H

#include <stdio.h>
// #include <stdlib.h>

#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

typedef long antval_t;

struct ant {
  const char *buf, *pc, *eof;
  int tok;                   
  antval_t val;              
  antval_t vars['z' - 'a'];  
  char err[20];              
};

#define ANT_INITIALIZER \
  { 0, 0, 0, 0, 0, {0}, "" }

static inline int ant_next(struct ant *ant);
static inline void ant_swallow(struct ant *ant);
static inline int ant_isnext(struct ant *ant, int t1, int t2);
static inline void ant_err(struct ant *ant, const char *fmt, ...);
static inline int ant_checktok(struct ant *ant, int t1, int t2);
static inline antval_t ant_num_or_var(struct ant *ant);
static inline antval_t ant_expr(struct ant *ant);
static inline void ant_jump(struct ant *ant);
static inline void ant_stmt_list(struct ant *ant, int etok);
antval_t ant_eval(struct ant *ant, const char *str);

#endif /* ANT_H */
