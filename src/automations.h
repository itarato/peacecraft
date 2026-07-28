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
  // TODO: We can make this dynamic. For example resource collection has less the priority when it's at the end of
  //       the iteration limit.
  virtual float priority() const = 0;

  static std::shared_ptr<Automation> from_command(CommandVariant command);
};

struct CharacterCreationAutomation : Automation {
  CharacterCreationAutomation(unsigned int building_id) : building_id(building_id) {
  }

  void update(World* world) override;
  const unsigned int get_character_id() const override;
  bool is_removable() const override;
  float priority() const override;

 private:
  unsigned int building_id;
  bool completed{false};
};

struct ResourceAutomation : Automation {
  unsigned int character_id;

  ResourceAutomation(const unsigned int character_id, const unsigned int resource_id, const unsigned int building_id,
                     int group, int iterations_left)
      : character_id(character_id),
        resource_id(resource_id),
        building_id(building_id),
        group(group),
        iterations_left(iterations_left) {
  }

  const unsigned int get_character_id() const override;
  void update(World* world) override;
  [[nodiscard]] bool is_removable() const override;
  float priority() const override;

 private:
  unsigned int resource_id;
  unsigned int building_id;
  int group;
  int iterations_left;
  ResourceAutomationState state{ResourceAutomationState::ReadyToStart};
  bool completed{false};
};

struct MoveAutomation : Automation {
  MoveAutomation(unsigned int character_id, Vector2 target) : character_id(character_id), target(target) {
  }

  void update(World* world) override;
  const unsigned int get_character_id() const override;
  bool is_removable() const override;
  float priority() const override;

 private:
  unsigned int character_id;
  Vector2 target;
  bool completed{false};
  bool wait_phase{false};
};

struct AutomationSequence : Automation {
  AutomationSequence() : character_id(INVALID_ID) {
  }

  AutomationSequence(unsigned int character_id) : character_id(character_id) {
  }

  std::vector<std::shared_ptr<Automation>> automations{};

  void update(World* world) override;
  const unsigned int get_character_id() const override;
  bool is_removable() const override;
  float priority() const override;

 private:
  unsigned int character_id;
};

struct BuildingAutomation : Automation {
  BuildingAutomation(Vector2 pos, int group) : pos(pos), group(group) {
  }

  void update(World* world) override;
  const unsigned int get_character_id() const override;
  bool is_removable() const override;
  float priority() const override;

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
  float priority() const override;

 private:
  unsigned int character_id;
  bool completed{false};
};

struct WaitForBuildingToBeReadyAutomation : Automation {
  WaitForBuildingToBeReadyAutomation(int group) : building_id(-1), group(group) {
  }

  WaitForBuildingToBeReadyAutomation(unsigned int building_id) : building_id(building_id), group(-1) {
  }

  [[nodiscard]] const unsigned int get_character_id() const override;
  bool is_removable() const override;
  void update(World* world) override;
  float priority() const override;

 private:
  bool completed{false};
  int building_id;
  int group;

  int last_building_id(World* world) const;
};

struct ChaseAutomation : Automation {
  ChaseAutomation(unsigned int character_id, unsigned int target) : character_id(character_id), target(target) {
  }

  [[nodiscard]] const unsigned int get_character_id() const override;
  [[nodiscard]] bool is_removable() const override;
  void update(World* world) override;
  float priority() const override;

 private:
  unsigned int character_id;
  unsigned int target;
  bool completed{false};
};
