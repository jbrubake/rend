#include "game.h"

manager_t cpt_name   = {.type = CPT_NAME,   .size = sizeof(name_t)};
manager_t cpt_pos    = {.type = CPT_POS,    .size = sizeof(pos_t)};
manager_t cpt_symbol = {.type = CPT_SYMBOL, .size = sizeof(symbol_t)};
