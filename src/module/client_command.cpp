#include <std_include.hpp>
#include <loader/module_loader.hpp>
#include <utils/string.hpp>

#include "game/game.hpp"
#include "command.hpp"

class client_command final : public module
{
public:
	void post_load() override
	{
		if (game::is_mp())
		{
			add_mp_client_commands();
		}
	}

private:
	static void send_msg_to_client(int client_num, game::native::svscmd_type type, const char* string)
	{
		if (game::is_dedi())
		{
			game::native::SV_SendServerCommand(&game::native::dedi::svs_clients[client_num],
				type, string);
		}
		else
		{
			game::native::SV_GameSendServerCommand(client_num, type, string);
		}
	}

	// I know this is supposed to check sv_cheats but it's not even a registered dvar so why bother
	static bool cheats_ok(game::native::gentity_s* ent)
	{
		if (ent->health < 1)
		{
			send_msg_to_client(ent->s.number, game::native::SV_CMD_CAN_IGNORE,
				utils::string::va("%c \"GAME_MUSTBEALIVECOMMAND\"", 0x65));
			return false;
		}

		return true;
	}

	static void add_mp_client_commands()
	{
		command::add_sv("noclip", [](game::native::gentity_s* ent, [[maybe_unused]] const command::params_sv& params)
		{
			if (!cheats_ok(ent))
				return;

			ent->client->flags ^= 1;

			send_msg_to_client(ent->s.number, game::native::SV_CMD_CAN_IGNORE,
				utils::string::va("%c \"%s\"", 0x65, (ent->client->flags & 1) ? "GAME_NOCLIPON" : "GAME_NOCLIPOFF"));
		});

		command::add_sv("ufo", [](game::native::gentity_s* ent, [[maybe_unused]] const command::params_sv& params)
		{
			if (!cheats_ok(ent))
				return;

			ent->client->flags ^= 2;

			send_msg_to_client(ent->s.number, game::native::SV_CMD_CAN_IGNORE,
				utils::string::va("%c \"%s\"", 0x65, (ent->client->flags & 2) ? "GAME_UFOON" : "GAME_UFOOFF"));
		});

		command::add_sv("god", [](game::native::gentity_s* ent, [[maybe_unused]] const command::params_sv& params)
		{
			if (!cheats_ok(ent))
				return;

			ent->flags ^= game::native::FL_GODMODE;

			send_msg_to_client(ent->s.number, game::native::SV_CMD_CAN_IGNORE,
				utils::string::va("%c \"%s\"", 0x65, (ent->flags & game::native::FL_GODMODE)
					? "GAME_GODMODE_ON" : "GAME_GODMODE_OFF"));
		});

		command::add_sv("demigod", [](game::native::gentity_s* ent, [[maybe_unused]] const command::params_sv& params)
		{
			if (!cheats_ok(ent))
				return;

			ent->flags ^= game::native::FL_DEMI_GODMODE;

			send_msg_to_client(ent->s.number, game::native::SV_CMD_CAN_IGNORE,
				utils::string::va("%c \"%s\"", 0x65, (ent->flags & game::native::FL_DEMI_GODMODE)
					? "GAME_DEMI_GODMODE_ON" : "GAME_DEMI_GODMODE_OFF"));
		});

		command::add_sv("notarget", [](game::native::gentity_s* ent, [[maybe_unused]] const command::params_sv& params)
		{
			if (!cheats_ok(ent))
				return;

			ent->flags ^= game::native::FL_NOTARGET;

			send_msg_to_client(ent->s.number, game::native::SV_CMD_CAN_IGNORE,
				utils::string::va("%c \"%s\"", 0x65, (ent->flags & game::native::FL_NOTARGET)
					? "GAME_NOTARGETON" : "GAME_NOTARGETOFF"));
		});

		command::add_sv("setviewpos", [](game::native::gentity_s* ent, [[maybe_unused]] const command::params_sv& params)
		{
			if (!cheats_ok(ent))
				return;

			game::native::vec3_t origin, angles{0.f, 0.f, 0.f};

			if (params.size() < 4 || params.size() > 6)
			{
				send_msg_to_client(ent->s.number, game::native::SV_CMD_CAN_IGNORE,
					utils::string::va("%c \"GAME_USAGE\x15: setviewpos x y z [yaw] [pitch]\"", 0x65));
				return;
			}

			for (auto i = 0; i < 3; i++)
			{
				origin[i] = std::strtof(params.get(i + 1), nullptr);
			}

			if (params.size() > 4)
			{
				angles[1] = std::strtof(params.get(4), nullptr); // Yaw
			}

			if (params.size() == 6u)
			{
				angles[0] = std::strtof(params.get(5), nullptr); // Pitch
			}

			game::native::TeleportPlayer(ent, origin, angles);
		});
	}
};

REGISTER_MODULE(client_command);