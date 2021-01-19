
#include "glinternal.h"

static void terminate_(void)
{
	int i;

	while (opg.windowListHead_)
		gluDestoryWindow((GLUwindow*)opg.windowListHead_);

	platformTerminate();

	opg.initialized_ = false;

	base::destoryTls(&opg.contextSlot_);
	memset(&opg, 0, sizeof(opg));
}

bool gluInitWindow()
{
	if (opg.initialized_) return true;
	if (!platformInit())
	{
		terminate_();
		return false;
	}

	opg.initialized_ = true;
	opg.timer_.offset_ = PlatformGetTimerValue();

	defaultWindowHints();
}

void gluTerminate()
{
	if (!opg.initialized_)
		return;
	terminate_();
}
