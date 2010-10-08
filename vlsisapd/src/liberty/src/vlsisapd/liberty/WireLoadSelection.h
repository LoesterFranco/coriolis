#ifndef __LIB_WIRE_LOAD_SELECTION_H__
#define __LIB_WIRE_LOAD_SELECTION_H__

#include<vector>
#include<fstream>

#include "vlsisapd/liberty/Name.h"
#include "vlsisapd/liberty/WireLoadArea.h"

namespace LIB {

class WireLoadSelection {
    public:
    WireLoadSelection(Name name);

    void addWireLoadArea(double min, double max, Name name);
    void print();
    bool write(std::ofstream& file);

    private:
    Name _name;
    std::vector<WireLoadArea*> _wires_load_area;
};

} // namespace
#endif
