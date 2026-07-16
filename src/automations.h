#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "characters.h"
#include "group.h"
#include "raylib.h"
#include "resource.h"

enum class ResourceAutomationState {
  ReadyToStart,
  MoveToResource,
  Harvest,
  MoveToBase,
  Dump,
};

struct Automation {
  virtual ~Automation() = default;
  virtual void update(std::unordered_map<unsigned int, Character>& characters,
                      std::unordered_map<unsigned int, Resource>& resources, std::vector<Group>& groups) = 0;
  virtual const unsigned int get_character_id() const = 0;
  virtual bool is_removable() const = 0;
};

struct ResourceAutomation : Automation {
  unsigned int character_id;

  ResourceAutomation(const unsigned int character_id, const unsigned int resource_id, const Vector2 base_pos,
                     int group_id)
      : character_id(character_id), resource_id(resource_id), base_pos(base_pos) {
  }

  const unsigned int get_character_id() const override {
    return character_id;
  }

  void update(std::unordered_map<unsigned int, Character>& characters,
              std::unordered_map<unsigned int, Resource>& resources, std::vector<Group>& groups) override {
    if (!characters.contains(character_id)) removable = true;
    if (!resources.contains(resource_id)) removable = true;
    if (removable) return;

    Character& owner = characters.at(character_id);
    Resource& resource = resources.at(resource_id);

    switch (state) {
      case ResourceAutomationState::ReadyToStart:
        owner.set_move_target(resource.pos);
        state = ResourceAutomationState::MoveToResource;
        break;
      case ResourceAutomationState::MoveToResource:
        if (owner.pos == resource.pos) state = ResourceAutomationState::Harvest;
        break;
      case ResourceAutomationState::Harvest: {
        const int amount = resource.harvest(10);
        owner.receive_resource(amount, resource.kind);
        owner.set_move_target(base_pos);
        state = ResourceAutomationState::MoveToBase;
        break;
      }
      case ResourceAutomationState::MoveToBase:
        if (owner.pos == base_pos) state = ResourceAutomationState::Dump;
        break;
      case ResourceAutomationState::Dump:
        for (int i = 0; i < RESOURCE_COUNT; i++) groups[group_id].resource_amounts[i] += owner.resource_amount(i);
        owner.empty_resources();
        state = ResourceAutomationState::ReadyToStart;
        break;
      default:
        bail("Unexpected");
    }
  }

  [[nodiscard]] bool is_removable() const override {
    return removable;
  }

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

  void update(std::unordered_map<unsigned int, Character>& characters,
              std::unordered_map<unsigned int, Resource>& resources, std::vector<Group>& groups) override {
    if (!characters.contains(character_id)) removable = true;
    if (removable) return;

    if (wait_phase) {
      if (characters.at(character_id).pos == target) removable = true;
    } else {
      characters.at(character_id).set_move_target(target);
      wait_phase = true;
    }
  }

  const unsigned int get_character_id() const override {
    return character_id;
  }

  bool is_removable() const override {
    return removable;
  }

 private:
  unsigned int character_id;
  Vector2 target;
  bool removable{false};
  bool wait_phase{false};
};

struct AutomationSequence : Automation {
  void update(std::unordered_map<unsigned int, Character>& characters,
              std::unordered_map<unsigned int, Resource>& resources, std::vector<Group>& groups) override {
    if (automations.empty()) return;

    if (automations[0]->is_removable()) {
      automations.erase(automations.begin());
      return;
    }

    automations[0]->update(characters, resources, groups);
  }

  const unsigned int get_character_id() const override {
    if (automations.empty()) bail("No more automations");

    return automations[0]->get_character_id();
  }

  bool is_removable() const override {
    return automations.empty();
  }

 private:
  std::vector<std::shared_ptr<Automation>> automations{};
};

/**

- have a type (data: character + resource)
- update
  - tell character to go to position
  - make character harvest resource
    - detect when to finish: character has enough resource OR resource is exhausted
  - tell character to go to a base (find the closest)
  - [optional] wait
  - repeat

**/
