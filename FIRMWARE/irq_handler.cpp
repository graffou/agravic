//#include "misc_extern.h"
void timer_irq_handler() __attribute__ ((interrupt ("user")));

void timer_irq_handler()
{
	IT_DISABLE(1<<MTIME_IT)
	return;
}
