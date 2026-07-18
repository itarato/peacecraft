#pragma once

#include <memory>
#include <unordered_map>
#include <variant>
#include <vector>

#include "characters.h"
#include "commands.h"
#include "grid_explorer.h"
#include "group.h"
#include "raylib.h"
#include "resource.h"
#include "world.h"

enum class ResourceAutomationState {
  ReadyToStart,
  MoveToResource,
  Harvest,
  MoveToBase,
  Dump,
};

struct Automation {
  virtual ~Automation() = default;
  virtual void update(World* world) = 0;
  virtual const unsigned int get_character_id() const = 0;
  virtual bool is_removable() const = 0;

  static std::shared_ptr<Automation> from_command(CommandVariant command);
};

struct CharacterCreationAutomation : Automation {
  CharacterCreationAutomation(Vector2 base_pos, int group_id) : base_pos(base_pos), group_id(group_id) {
  }

  void update(World* world) override;
  const unsigned int get_character_id() const override;
  bool is_removable() const;

 private:
  Vector2 base_pos;
  int group_id;
  bool completed{false};
};

struct ResourceAutomation : Automation {
  unsigned int character_id;

  ResourceAutomation(const unsigned int character_id, const unsigned int resource_id, const Vector2 base_pos,
                     int group_id)
      : character_id(character_id), resource_id(resource_id), base_pos(base_pos) {
  }

  const unsigned int get_character_id() const override;
  void update(World* world) override;
  [[nodiscard]] bool is_removable() const override;

 private:
  unsigned int resource_id;
  Vector2 base_pos;
  int group_id;
  ResourceAutomationState state{ResourceAutomationState::ReadyToStart};
  bool removable{false};
};

struct MoveAutomation : Automation {
  MoveAutomation(unsigned int character_id, Vector2 target) : character_id(character_id), target(target) {
  }

  void update(World* world) override;
  const unsigned int get_character_id() const override;
  bool is_removable() const override;

 private:
  unsigned int character_id;
  Vector2 target;
  bool completed{false};
  bool wait_phase{false};
};

struct AutomationSequence : Automation {
  std::vector<std::shared_ptr<Automation>> automations{};

  void update(World* world) override;
  const unsigned int get_character_id() const override;
  bool is_removable() const override;
};

struct BuildingAutomation : Automation {
  BuildingAutomation(Vector2 pos, int group) : pos(pos), group(group) {
  }

  void update(World* world) override;
  const unsigned int get_character_id() const override;
  bool is_removable() const override;

 private:
  Vector2 pos;
  int group;
  bool completed{false};
};

struct BuildingRequestAutomation : Automation {
  BuildingRequestAutomation(unsigned int character_id) : character_id(character_id) {
  }

  void update(World* world) override;
  const unsigned int get_character_id() const override;
  bool is_removable() const override;

 private:
  unsigned int character_id;
  bool completed{false};
};
