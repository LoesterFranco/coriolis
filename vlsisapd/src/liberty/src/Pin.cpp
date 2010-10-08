#include<iostream>
#include<fstream>
using namespace std;

#include "vlsisapd/liberty/Name.h"
#include "vlsisapd/liberty/Attribute.h"
#include "vlsisapd/liberty/Pin.h"

namespace LIB {
Pin::Pin(Name name): _name(name), _attributes(), _timings() {};

void Pin::addAttribute(Name attrName, Attribute::Type attrType, string& attrValue) {
    Attribute* attr = new Attribute(attrName, attrType, attrValue);
    map<Name, Attribute*>::iterator it = _attributes.find(attrName);
    if (it != _attributes.end()) {
        cerr << "[ERROR] Cannot define Pin " << _name.getString() << " with several attributes with the same name: " << attrName.getString() << endl;
        exit(1);
    }
    _attributes[attrName] = attr;
}

void Pin::addTiming() {
    Timing* timing = new Timing();
    _timings.push_back(timing);
}

void Pin::print() {
    cout << "|       Pin name= " << _name.getString() << endl;

    // Pin's attributes
    cout << "|         Attributes :" << endl;
    for(map<Name, Attribute*>::const_iterator it=_attributes.begin() ; it!=_attributes.end() ; ++it) {
        cout << "|           name= " << (*it).first.getString()
             << ", type= " << (*it).second->typeToString((*it).second->getType())
             << ", value= " << (*it).second->valueAsString() << endl;
    }
    // Timing
    for (size_t i = 0 ; i < _timings.size() ; i++) {
        _timings[i]->print();
    }
}

bool Pin::write(ofstream &file) {
    file << "        pin(" << _name.getString() << ") {" << endl;
    //Pin's attributes
    for(map<Name, Attribute*>::const_iterator it=_attributes.begin() ; it!=_attributes.end() ; ++it) {
        file << "            ";
        (*it).second->write(file);
    }
    // Timing
    for (size_t i = 0 ; i < _timings.size() ; i++)
        _timings[i]->write(file);
    file << "        }" << endl;
    return true;
}

} // namespace
