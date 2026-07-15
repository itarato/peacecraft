#pragma once

#include "resource.h"

struct Group {
  int id;
  int resource_amounts[RESOURCE_COUNT] = {};

  Group(int id) : id(id) {
    resource_amounts[RESOURCE_WOOD] = 120;
    resource_amounts[RESOURCE_MINERAL] = 210;
  }
};
