#include <ibus.h>

IBusBus *bus;

int main()
{
	g_type_init ();

	bus = ibus_bus_new ();
}
