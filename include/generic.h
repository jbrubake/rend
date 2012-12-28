/*
    Contains generic component structures and managers.
 */

#ifndef _GENERIC_H_
#define _GENERIC_H_

#define NAME_SIZE 64

typedef struct name_s {
    component_t base;

    char str[NAME_SIZE];
} name_t;

typedef struct pos_s {
    component_t base;

    coord_t pos;
} pos_t;

typedef struct symbol_s {
    component_t base;

    u8 symbol;
    u8 color;
} symbol_t;

extern manager_t cpt_name;
extern manager_t cpt_pos;
extern manager_t cpt_symbol;

#endif