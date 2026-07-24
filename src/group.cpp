#include "group.h"

constexpr int PAYABLE_COSTS[2][2] = {
    // Building:
    {100, 50},
    // Character:
    {80, 60},
};

[[nodiscard]] bool Group::can_pay_for(Payable payable) const {
  int i = 0;
  for (auto const& amount : PAYABLE_COSTS[static_cast<int>(payable)]) {
    if (resource_amounts[i] < amount) return false;
    i++;
  }

  return true;
}

void Group::pay_for(Payable payable) {
  int i = 0;
  for (auto const& amount : PAYABLE_COSTS[static_cast<int>(payable)]) {
    resource_amounts[i] -= amount;
    i++;
  }
}
