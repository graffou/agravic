#include <vector>
#include "gmodule.h"
std::vector<gmodule*> gmodule::module_list;
gmodule* gmodule::out_of_hier = new gmodule;
unsigned long long int gmodule::vcd_time;

