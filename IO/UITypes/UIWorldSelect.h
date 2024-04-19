//////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton						//
//																				//
//	This program is free software: you can redistribute it and/or modify		//
//	it under the terms of the GNU Affero General Public License as published by	//
//	the Free Software Foundation, either version 3 of the License, or			//
//	(at your option) any later version.											//
//																				//
//	This program is distributed in the hope that it will be useful,				//
//	but WITHOUT ANY WARRANTY; without even the implied warranty of				//
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the				//
//	GNU Affero General Public License for more details.							//
//																				//
//	You should have received a copy of the GNU Affero General Public License	//
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.		//
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIElementCentered.h"

#include "../Components/ChatBalloon.h"
#include "../Components/Gauge.h"

#include "../../Net/Login.h"

#include "../../Graphics/Geometry.h"

namespace ms
{
	class UIWorldSelect : public UIElementCentered
	{
	public:
		static constexpr Type TYPE = UIElement::Type::WORLDSELECT;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = false;

		UIWorldSelect();

		void draw(float alpha) const override;

		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;

		UIElement::Type get_type() const override;

		void draw_world();
		void add_world(World world);
		void change_world(World selectedWorld);
		void remove_selected();
		//uint16_t get_worldbyid(uint16_t worldid);

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		static constexpr uint8_t COLUMNS = 5U;
		static constexpr uint8_t FLAG_SIZE = 3U;
		// This is from the server in ServerConstants
		static constexpr int32_t CHANNEL_LOAD = 100;

		void enter_world();
		void clear_selected_world();
		//uint16_t get_next_world(uint16_t id, bool upward);

		enum Buttons : uint16_t
		{
			BtWorld0,
			BtWorld1,
			BtChannel0,
			BtExit
		};

		/// If ever changing order, check the WZ file.
		enum Worlds : uint16_t
		{
			BERA,
			SCANIA,
			BROA,
			WINDIA,
		};

		Texture worlds_background;
		Texture channels_background;


		uint8_t worldid;
		uint8_t channelid;
		uint8_t worldcount;

		std::vector<World> worlds;
		std::vector<Texture> world_textures;
		std::map<uint16_t, uint16_t> world_map;

		std::vector<Sprite> flag_sprites;
		//Gauge channel_gauge[Buttons::BtWorld0 - Buttons::BtChannel0];

		bool world_selected;

		nl::node worldsrc;
		nl::node channelsrc;

		// background for undefined bg
		ColorBox background;
	};
}