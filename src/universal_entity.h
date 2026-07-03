#pragma once

#include <vector>

#include "commands.h"

struct UniversalEntity {
  virtual ~UniversalEntity() = default;

  virtual std::vector<CommandVariant> update(Camera2D& camera) = 0;
  virtual void draw(const Camera2D& camera) const = 0;
  virtual bool is_removable() const = 0;
};

struct BuildingMarkerUEntity : UniversalEntity {
  BuildingMarkerUEntity(u_int32_t character_id) : character_id(character_id) {
  }

  std::vector<CommandVariant> update(Camera2D& camera) override {
    if (IsMouseButtonPressed(0)) {
      removable = true;
      Vector2 target = GetScreenToWorld2D(GetMousePosition(), camera);
      return {BuildingCreationCommand{target}, CharacterMoveCommand{target, character_id}};
    }

    return {};
  }

  void draw(const Camera2D& camera) const override {
    DrawCircleLinesEx(GetScreenToWorld2D(GetMousePosition(), camera), 32.0f, 4, LIME);
  }

  bool is_removable() const override {
    return removable;
  }

 private:
  u_int32_t character_id;
  bool removable{false};
};
