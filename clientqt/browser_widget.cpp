#include "stdafx.h"
#include "browser_widget.h"

void browser_widget::web_view_load_finished(bool b)
{
	if (scene && painter && image)
	{
		scene->render(painter);
		image->save("c:/a.png");
	}
}
