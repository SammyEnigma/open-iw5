#pragma once
#include "html/html_window.hpp"

class launcher final
{
public:
	enum mode
	{
		none,
		singleplayer,
		multiplayer,
		server,
	};

	launcher();

	mode run() const;

private:
	mode mode_ = none;

	html_window main_window_;
	html_window settings_window_;

	void select_mode(mode mode);

	void create_main_menu();
	void create_settings_menu();

	static std::string load_content(int res);
};