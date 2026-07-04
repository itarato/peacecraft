#pragma once

#include <unordered_map>

#include "characters.h"
#include "raylib.h"
#include "resource.h"

enum class ResourceAutomationState {
  ReadyToStart,
  MoveToResource,
  Harvest,
  MoveToBase,
};

struct ResourceAutomation {
  unsigned int character_id;

  ResourceAutomation(const unsigned int character_id, const Vector2 resource_pos, const Vector2 base_pos)
      : character_id(character_id), resource_pos(resource_pos), base_pos(base_pos) {
  }

  void update(std::unordered_map<unsigned int, Character>& characters, std::vector<Resource>& resources) {
    Character& owner = characters.at(character_id);
    // Resource& resource = resources.at();

    switch (state) {
      case ResourceAutomationState::ReadyToStart:
        owner.set_move_target(resource_pos);
        state = ResourceAutomationState::MoveToResource;
        break;
      case ResourceAutomationState::MoveToResource:
        // How to check resources if they are really there?
        if (owner.pos == resource_pos) {
          state = ResourceAutomationState::Harvest;
        }
        break;
      case ResourceAutomationState::Harvest:
        owner.set_move_target(base_pos);
        state = ResourceAutomationState::MoveToBase;
        break;
      case ResourceAutomationState::MoveToBase:
        if (owner.pos == base_pos) {
          state = ResourceAutomationState::ReadyToStart;
        }
        break;
      default:
        bail("Unexpected");
    }
  }

 private:
  Vector2 resource_pos;
  Vector2 base_pos;
  ResourceAutomationState state{ResourceAutomationState::ReadyToStart};
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
