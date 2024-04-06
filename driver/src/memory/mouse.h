#pragma once
#include "../utils/defs.h"

namespace mouse {
	NTSTATUS init(MOUSE_OBJECT* mouse_obj);

	void move(MOUSE_OBJECT mouse_obj, int x, int y,char button_flag);
}