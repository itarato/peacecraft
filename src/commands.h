#pragma once

#include <cstdlib>
#include <memory>
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
  unsigned int building_id;

  CharacterCreationCommand(unsigned int building_id) : building_id(building_id) {
  }

  const char* get_name() const override;
};

struct BuildingCreationRequestCommand : BaseCommand {
  u_int32_t character_id;

  BuildingCreationRequestCommand(u_int32_t character_id) : character_id(character_id) {
  }

  const char* get_name() const override;
};

typedef std::variant<CharacterCreationCommand, BuildingCreationRequestCommand> CommandVariant;

const char* command_get_name(CommandVariant command);

/**
 * Command list to advertise available commands from a source.
 */
struct CommandList {
  std::vector<CommandVariant> commands{};

  CommandList(std::initializer_list<CommandVariant> commands) : commands(commands) {
  }

  void draw(Camera2D const& camera) const;
  std::optional<CommandVariant> just_selected_command(Camera2D& camera);

 private:
  Rectangle get_icon_frame(int index, Camera2D const& camera) const;
};
