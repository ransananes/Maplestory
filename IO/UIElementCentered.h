#pragma once
#include "./UIElement.h"

namespace ms
{
	class UIElementCentered : public UIElement
	{
		public:
			virtual ~UIElementCentered() {}
			UIElementCentered() : UIElement(Point<int16_t>(0,0), Point<int16_t>(VIEWSIZE.x(), VIEWSIZE.y())) {
				position = Point<int16_t>(VIEWSIZE.x() / 2, VIEWSIZE.y() / 2);

			};
	
	};
}

