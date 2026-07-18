#pragma once

#include <memory>
#include <vector>

#include "automations.h"

struct UniversalEntity {
  virtual ~UniversalEntity() = default;

  virtual std::vector<std::shared_ptr<Automation>> update(Camera2D& camera) = 0;
  virtual void draw(const Camera2D& camera) const = 0;
  virtual bool is_removable() const = 0;
};

struct BuildingMarkerUEntity : UniversalEntity {
  BuildingMarkerUEntity(u_int32_t character_id) : character_id(character_id) {
  }

  std::vector<std::shared_ptr<Automation>> update(Camera2D& camera) override;
  void draw(const Camera2D& camera) const override;
  bool is_removable() const override;

 private:
  u_int32_t character_id;
  bool removable{false};
};
