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
#include "UILoginWait.h"

#include "../Components/MapleButton.h"

#include "../../Net/Session.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UILoginWait::UILoginWait() : UILoginWait([]() {}) {}

	UILoginWait::UILoginWait(std::function<void()> okhandler) : okhandler(okhandler)
	{
		nl::node Loading = nl::nx::UI["Login.img"]["Notice"]["Loading"];
		nl::node backgrnd = Loading["backgrnd"];

		Point<int16_t> startpos = Point<int16_t>(0,0);
		Sprite sizedbackgrnd = Sprite(backgrnd);
		//sprites.emplace_back(backgrnd, (startpos,1.0f,1.2f));
		
		//loadingBar = Loading["bar"];

		//sprites.emplace_back(Loading["circle"], startpos + Point<int16_t>(sizedbackgrnd.width()/2, sizedbackgrnd.height() - 20));

		//buttons[Buttons::BtCancel] = std::make_unique<MapleButton>(Loading["BtCancel"], startpos + Point<int16_t>(sizedbackgrnd.width() / 2, sizedbackgrnd.height()*1.2f - 0));

		position = Point<int16_t>(276, 229);
		dimension = Texture(backgrnd).get_dimensions();
	}

	void UILoginWait::update()
	{
		loadingBar.update(1);
	}

	void UILoginWait::draw(float inter)
	{
		loadingBar.draw(position, inter);
	}

	UIElement::Type UILoginWait::get_type() const
	{
		return TYPE;
	}

	void UILoginWait::close()
	{
		deactivate();
		okhandler();
	}

	std::function<void()> UILoginWait::get_handler()
	{
		return okhandler;
	}

	Button::State UILoginWait::button_pressed(uint16_t id)
	{
		Session::get().reconnect();

		close();

		return Button::State::NORMAL;
	}
}