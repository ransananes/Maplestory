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
#include "UIWorldSelect.h"

#include "UILoginNotice.h"
#include "UILoginWait.h"
#include "UIRegion.h"

#include "../UI.h"

#include "../Components/MapleButton.h"
#include "../Components/TwoSpriteButton.h"

#include "../../Audio/Audio.h"
#include "../../Util/Randomizer.h"

#include "../../Net/Packets/LoginPackets.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIWorldSelect::UIWorldSelect() : UIElementCentered(), worldcount(0), world_selected(false)
	{

		worldid = Setting<DefaultWorld>::get().load();
		channelid = Setting<DefaultChannel>::get().load();

		nl::node Login = nl::nx::UI["Login.img"];
		nl::node Common = Login["Common"];
		nl::node WorldSelect = Login["WorldSelect"];
		worldsrc = WorldSelect["BtWorld"];
		channelsrc = WorldSelect["BtChannel"];

		nl::node WorldSelectMap = nl::nx::Map["Obj"]["login.img"]["WorldSelect"];
		nl::node WorldSelectBG = nl::nx::Map["Back"]["login.img"]["back"];
		sprites.emplace_back(WorldSelect["wsBackgrn"], Point<int16_t>(-VIEWSIZE.x()/2, -VIEWSIZE.y()/2),true,true);
		sprites.emplace_back(WorldSelectMap["signboard"]["2"]["0"], Point<int16_t>(0, -VIEWSIZE.y() / 3));
		sprites.emplace_back(WorldSelectMap["signboard"]["0"]["0"], Point<int16_t>(0, -VIEWSIZE.y() / 3));

		buttons[Buttons::BtExit] = std::make_unique<MapleButton>(Common["BtExit"], Point<int16_t>(-VIEWSIZE.x()/2, VIEWSIZE.y()/2.5));

		// TODO: add channel selection
		//channels_background = WorldSelect["chBackgrn"];

		for (uint16_t i = Buttons::BtWorld0; i < Buttons::BtChannel0; i++)
		{
			std::string world = std::to_string(world_map[i]);
			//world_textures.emplace_back(channelsrc["release"]["layer:" + world]);

			nl::node worldbtn = worldsrc[world];
			buttons[Buttons::BtWorld0 + i] = std::make_unique<TwoSpriteButton>(worldbtn["normal"]["0"], worldbtn["mouseOver"]["2"],Point<int16_t>(-VIEWSIZE.x()/6.1 + i*i, -VIEWSIZE.y() / 4));
			buttons[Buttons::BtWorld0 + i]->set_active(false);
		}
		background = ColorBox(dimension.x(), dimension.y(), Color::Name::BLACK, 1.0f);

	}

	void UIWorldSelect::draw(float alpha) const
	{
		background.draw(Point<int16_t>(0, 0));
		UIElement::draw_sprites(alpha);
		UIElement::draw_buttons(alpha);

	}

	Cursor::State UIWorldSelect::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		Rectangle<int16_t> channels_bounds = Rectangle<int16_t>(
			position,
			position + channels_background.get_dimensions()
			);

		Rectangle<int16_t> worlds_bounds = Rectangle<int16_t>(
			position,
			position + worlds_background.get_dimensions()
			);

		if (world_selected && !channels_bounds.contains(cursorpos) && !worlds_bounds.contains(cursorpos))
		{
			if (clicked)
			{
				world_selected = false;
				clear_selected_world();
			}
		}

		Cursor::State ret = clicked ? Cursor::State::CLICKING : Cursor::State::IDLE;

		for (auto& btit : buttons)
		{
			if (btit.second->is_active() && btit.second->bounds(position).contains(cursorpos))
			{
				if (btit.second->get_state() == Button::State::NORMAL)
				{
					Sound(Sound::Name::BUTTONOVER).play();

					btit.second->set_state(Button::State::MOUSEOVER);
					ret = Cursor::State::CANCLICK;
				}
				else if (btit.second->get_state() == Button::State::PRESSED)
				{
					if (clicked)
					{
						Sound(Sound::Name::BUTTONCLICK).play();

						btit.second->set_state(button_pressed(btit.first));

						ret = Cursor::State::IDLE;
					}
					else
					{
						ret = Cursor::State::CANCLICK;
					}
				}
				else if (btit.second->get_state() == Button::State::MOUSEOVER)
				{
					if (clicked)
					{
						Sound(Sound::Name::BUTTONCLICK).play();

						btit.second->set_state(button_pressed(btit.first));

						ret = Cursor::State::IDLE;
					}
					else
					{
						ret = Cursor::State::CANCLICK;
					}
				}
			}
			else if (btit.second->get_state() == Button::State::MOUSEOVER)
			{
				btit.second->set_state(Button::State::NORMAL);
			}
		}

		return ret;
	}

	void UIWorldSelect::send_key(int32_t keycode, bool pressed, bool escape)
	{
		// TODO
	}

	UIElement::Type UIWorldSelect::get_type() const
	{
		return TYPE;
	}

	void UIWorldSelect::draw_world()
	{
		if(worldcount <= 0)
			return; // TODO: Send the user back to the login screen? Otherwise, I think the screen will be blank with no worlds, or throw a UILoginNotice up with failed to communite to server?

		for (auto world : worlds)
		{
			buttons[Buttons::BtWorld0 + world.id]->set_active(true);
		}
	}

	void UIWorldSelect::add_world(World world)
	{
		worlds.emplace_back(std::move(world));
		worldcount++;
	}

	void UIWorldSelect::change_world(World selectedWorld)
	{

	}

	void UIWorldSelect::remove_selected()
	{
		deactivate();

		Sound(Sound::Name::SCROLLUP).play();

		world_selected = false;

		clear_selected_world();
	}





	Button::State UIWorldSelect::button_pressed(uint16_t id)
	{
		if (id == Buttons::BtWorld0)
		{
			enter_world();

			return Button::State::NORMAL;
		}
		else if (id == Buttons::BtExit)
		{
		UI::get().emplace<UIQuitConfirm>();

		return Button::State::NORMAL;
		}
	}

	void UIWorldSelect::enter_world()
	{
		channelid = 1;
		Configuration::get().set_worldid(worldid);
		Configuration::get().set_channelid(channelid);

		UI::get().emplace<UILoginWait>();
		auto loginwait = UI::get().get_element<UILoginWait>();

		if (loginwait && loginwait->is_active())
			CharlistRequestPacket(worldid, channelid).dispatch();
	}

	void UIWorldSelect::clear_selected_world()
	{
		// TODO
	}

}