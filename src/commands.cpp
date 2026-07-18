#include "commands.h"

#include <cstdlib>
#include <memory>
#include <variant>
#include <vector>

#include "common.h"
#include "raylib.h"

const char* CharacterCreationCommand::get_name() const {
  return "Make character";
}

const char* BuildingCreationRequestCommand::get_name() const {
  return "Make building";
}

const char* command_get_name(CommandVariant command) {
  if (std::holds_alternative<CharacterCreationCommand>(command)) {
    return std::get<CharacterCreationCommand>(command).get_name();
  } else if (std::holds_alternative<BuildingCreationRequestCommand>(command)) {
    return std::get<BuildingCreationRequestCommand>(command).get_name();
  } else {
    bail("Unexpected");
  }
}

void CommandList::draw(Camera2D const& camera) const {
  for (unsigned int i = 0; i < commands.size(); i++) {
    Rectangle icon_frame = get_icon_frame(i, camera);
    DrawRectangleRec(icon_frame, GOLD);
    DrawText(command_get_name(commands[i]), icon_frame.x, icon_frame.y + COMMAND_ICON_SIZE + 2, 10, BLACK);
  }
}

std::optional<CommandVariant> CommandList::just_selected_command(Camera2D& camera) {
  for (unsigned int i = 0; i < commands.size(); i++) {
    Rectangle icon_frame = get_icon_frame(i, camera);
    if (IsMouseButtonPressed(0) && CheckCollisionPointRec(GetScreenToWorld2D(GetMousePosition(), camera), icon_frame)) {
      return commands[i];
    }
  }

  return std::nullopt;
}

Rectangle CommandList::get_icon_frame(int index, Camera2D const& camera) const {
  Vector2 frame_pos = GetScreenToWorld2D(
      Vector2(COMMAND_ICON_PADDING, 60 + (COMMAND_ICON_PADDING + COMMAND_ICON_SIZE) * index), camera);
  return Rectangle(frame_pos.x, frame_pos.y, COMMAND_ICON_SIZE, COMMAND_ICON_SIZE);
}
