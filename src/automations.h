#pragma once

#include "characters.h"
#include "raylib.h"

enum class ResourceAutomationState {
  ReadyToStart,
  MoveToResource,
  Harvest,
  MoveToBase,
};

struct ResourceAutomation {
  ResourceAutomation(unsigned int character_id, Vector2 resorce_pos, Vector2 base_pos)
      : character_id(character_id), resorce_pos(resorce_pos), base_pos(base_pos) {
  }

  void update(Character& owner) {
    switch (state) {
      case ResourceAutomationState::ReadyToStart:
        owner.set_move_target(resorce_pos);
        state = ResourceAutomationState::MoveToResource;
        break;
      case ResourceAutomationState::MoveToResource:
        // How to check resources if they are really there?
        if (owner.pos == resorce_pos) {
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
        UNEXPECTED;
    }
  }

 private:
  unsigned int character_id;
  Vector2 resorce_pos;
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
