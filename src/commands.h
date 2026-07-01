#pragma once

#include <cstdlib>
#include <variant>
#include <vector>

#include "common.h"
#include "raylib.h"

enum class GameCommandType {
  CharacterCreation,
  BuildingCreation,
};

struct BaseCommand {
  virtual ~BaseCommand() = default;
  virtual const char* get_name() const = 0;
};

struct CharacterCreationCommand : BaseCommand {
  Vector2 base_pos;

  CharacterCreationCommand(Vector2 base_pos) : base_pos(base_pos) {
  }

  const char* get_name() const override {
    return "Make character";
  }
};

struct BuildingCreationRequestCommand : BaseCommand {
  u_int32_t character_id;

  BuildingCreationRequestCommand(u_int32_t character_id) : character_id(character_id) {
  }

  const char* get_name() const override {
    return "Make building";
  }
};

struct BuildingCreationCommand {
  Vector2 pos;
};

typedef std::variant<CharacterCreationCommand, BuildingCreationRequestCommand, BuildingCreationCommand> CommandVariant;

const char* command_get_name(CommandVariant command) {
  if (std::holds_alternative<CharacterCreationCommand>(command)) {
    return std::get<CharacterCreationCommand>(command).get_name();
  } else if (std::holds_alternative<BuildingCreationRequestCommand>(command)) {
    return std::get<BuildingCreationRequestCommand>(command).get_name();
  } else {
    UNEXPECTED;
  }
}

/**
 * Command list to advertise available commands from a source.
 */
struct CommandList {
  std::vector<CommandVariant> commands{};

  CommandList(std::initializer_list<CommandVariant> commands) : commands(commands) {
  }

  void draw(Camera2D const& camera) const {
    for (unsigned int i = 0; i < commands.size(); i++) {
      Rectangle icon_frame = get_icon_frame(i, camera);
      DrawRectangleRec(icon_frame, GOLD);
      DrawText(command_get_name(commands[i]), icon_frame.x, icon_frame.y + COMMAND_ICON_SIZE + 2, 10, BLACK);
    }
  }

  std::optional<CommandVariant> just_selected_command(Camera2D& camera) {
    for (unsigned int i = 0; i < commands.size(); i++) {
      Rectangle icon_frame = get_icon_frame(i, camera);
      if (IsMouseButtonPressed(0) &&
          CheckCollisionPointRec(GetScreenToWorld2D(GetMousePosition(), camera), icon_frame)) {
        return commands[i];
      }
    }

    return std::nullopt;
  }

 private:
  Rectangle get_icon_frame(int index, Camera2D const& camera) const {
    Vector2 frame_pos = GetScreenToWorld2D(
        Vector2(COMMAND_ICON_PADDING, COMMAND_ICON_PADDING + (COMMAND_ICON_PADDING + COMMAND_ICON_SIZE) * index),
        camera);
    return Rectangle(frame_pos.x, frame_pos.y, COMMAND_ICON_SIZE, COMMAND_ICON_SIZE);
  }
};
