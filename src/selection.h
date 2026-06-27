#pragma once

#include "raylib.h"

struct Selectable {
  virtual Rectangle selection_frame() const;
  virtual void select();
};

struct Selector {};
