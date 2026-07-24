#pragma once

#include "resource.h"

enum class Payable {
  Building,
  Character,
};

struct Group {
  int id;
  int resource_amounts[RESOURCE_COUNT] = {};

  Group(int id) : id(id) {
    resource_amounts[RESOURCE_WOOD] = 120;
    resource_amounts[RESOURCE_MINERAL] = 210;
  }

  [[nodiscard]] bool can_pay_for(Payable payable) const;
  void pay_for(Payable payable);
};
