// -*-compile-command:"cd ../../../../.. && make"-*-
// Time-stamp: "2010-07-26 16:16:18" - OpenAccessParser.cpp
// x-----------------------------------------------------------------x
// |  This file is part of the hurricaneAMS Software.                |
// |  Copyright (c) UPMC/LIP6 2008-2010, All Rights Reserved         |
// | =============================================================== |
// |  Author      :                 Chistophe Alexandre              |
// |  E-mail      :   Christophe.Alexandre@asim.lip6.fr              |
// x-----------------------------------------------------------------x
// |  Author      :                    Jean-Manuel Caba              |
// |  E-mail      :       Jean-Manuel.Caba@asim.lip6.fr              |
// x-----------------------------------------------------------------x

#include <iostream>
#include <set>
using namespace std;

#include "hurricane/DataBase.h"
#include "hurricane/Technology.h"
#include "hurricane/BasicLayer.h"
#include "hurricane/Library.h"
#include "hurricane/Horizontal.h"
#include "hurricane/Vertical.h"
#include "hurricane/NetExternalComponents.h"
using namespace Hurricane;

#include "OpenAccess.h"
#include "OpenAccessCommon.h"

namespace {

#ifdef HAVE_OPENACCESS
    class OAParser {
        typedef map<oaLib*, Library*> OALib2LibMap;
        typedef map<Name, Library*> Name2LibMap;
        typedef map<oaLayerNum, Layer*> OALayerNum2LayerMap;
        typedef map<Cell*, oaCell*> Cell2OACellMap;

        oaTech* _oaTechnology; // only one technology;
        OALayerNum2LayerMap _oaLayerNum2LayerMap;
        OALib2LibMap _oaLib2LibMap;
        Name2LibMap _name2LibMap;
        Cell2OACellMap _cell2OACellMap;
        set<Cell*> _loadedCells;

        OAParser():
            _oaTechnology(NULL),
            _oaLayerNum2LayerMap(),
            _oaLib2LibMap(),
            _name2LibMap(),
            _cell2OACellMap(),
            _loadedCells(){
            try {
                oaDesignInit(oacAPIMajorRevNumber,
                             oacAPIMinorRevNumber,
                             oacDataModelRevNumber);
            } catch (oaException  &excp) {
                cout << "ERROR: " << excp.getMsg() << endl;
                exit(1);
            }


            DataBase* db = DataBase::getDB();
            if (!db) {
                db = DataBase::create();
            }
            Library* rootLibrary = db->getRootLibrary();
            if (!rootLibrary) {
                rootLibrary = Library::create ( db, "RootLibrary" );
            }
            Library::create(rootLibrary, OACellLibrariesName);
            Library::create(rootLibrary, OADesignLibrariesName);
        }

        ~OAParser() {
            for (OALib2LibMap::iterator lit = _oaLib2LibMap.begin();
                 lit != _oaLib2LibMap.end();
                 lit++) {
                lit->first->close();
            }
        }

        Cell* getCell(const string& cellNameStr) {
            DataBase* db = DataBase::getDB();
            if (!db || !db->getRootLibrary() ) {
                return NULL;
            }
            Name cellName(cellNameStr);
            Cell* cell = findCellInLibraries(getOADesignLibraries(), cellName);
            if (!cell) {
                return NULL;
            }

            set<Cell*>::const_iterator csit = _loadedCells.find(cell);
            if (csit == _loadedCells.end()) {
                Cell2OACellMap::const_iterator cit = _cell2OACellMap.find(cell);
                if (cit == _cell2OACellMap.end()) {
                    cerr << "error : cannot find cell" << endl;
                    exit(8);
                }
                oaCell* oa_Cell = cit->second;
                loadOACellInCell(oa_Cell, cell);
                _loadedCells.insert(cell);
            }
            return cell;
        }

        void loadOALib(const string& libNameStr, const string& libPathStr, bool asDesignLibrary) {
            Name libName(libNameStr);
            Name2LibMap::const_iterator nit = _name2LibMap.find(libName);
            if (nit != _name2LibMap.end()) {
                Library* library = nit->second;
                //verify that it's the same library : name and path
                cerr << "already loaded" << endl;
                return;
            }

            oaNativeNS oaNS;
            oaString libNameOAStr(libNameStr.c_str());
            oaScalarName libOAName(oaNS, libNameOAStr);

            try {
                oaLib* oaLibrary = oaLib::open(libOAName, libPathStr.c_str());
                if (oaLibrary->isReadable()) {
                    if (!oaLibrary->getAccess(oacReadLibAccess)) {
                        cout << "\n***Quitting. Cannot get LibAccess\n" ;
                        exit(8);
                    }

                    oaTechnology2Technology(oaLibrary);

                    //create Hurricane library
                    Name libraryName = Name(libNameStr);
                    DataBase* db = DataBase::getDB();
                    if (!db) {
                        cerr << "No DataBase" << endl;
                        exit(8);
                    }
                    if (findLibraryByNameInDB(db, libraryName)) {
                        cerr << "ERROR" << endl;
                        exit(8);
                    }

                    Library* library;
                    if (asDesignLibrary) {
                        library = Library::create(getOADesignLibraries(), Name(libNameStr));
                    } else {
                        library = Library::create(getOACellLibraries(), Name(libNameStr));
                    }
                    cerr << library << endl;

                    oaCollection<oaCell, oaLib> cells = oaLibrary->getCells();
                    oaIter<oaCell> cellsIter(cells);

                    while (oaCell* cell = cellsIter.getNext()) {
                        oaScalarName cellOAName;
                        oaString cellNameString;
                        cell->getName(cellOAName);
                        cellOAName.get(cellNameString);

                        cerr << cellNameString << endl;

                        oaDesign* cellDesign = openDesign(oaNS, cell);
                        if (cellDesign != NULL) {
                            Cell* hCell = NULL;
                            //logic part
                            oaModule* module = cellDesign->getTopModule();
                            if (module) {
                                hCell = Cell::create(library, Name(cellNameString));
                                hCell->setTerminal(false);
                                if (!asDesignLibrary) {
                                    hCell->setFlattenLeaf(true);
                                }
                                cerr << hCell << endl;
                                if (asDesignLibrary) {
                                    _cell2OACellMap[hCell] = cell;
                                }
                                oaCollection<oaModTerm, oaModule> oaModTerms = module->getTerms();
                                oaIter<oaModTerm> oaModTermIter(oaModTerms);
                                while (oaModTerm* modTerm = oaModTermIter.getNext()) {
                                    oaString oaModTermStr;
                                    modTerm->getName(oaNS, oaModTermStr);
                                    Net* net = Net::create(hCell, Name(oaModTermStr));
                                    net->setExternal(true);
                                    switch (modTerm->getTermType()) {
                                    case oacInputTermType:
                                        net->setDirection(Net::Direction::IN);
                                        break;
                                    case oacOutputTermType:
                                        net->setDirection(Net::Direction::OUT);
                                        break;
                                    case oacInputOutputTermType:
                                        net->setDirection(Net::Direction::INOUT);
                                        break;
                                    case oacTristateTermType:
                                        net->setDirection(Net::Direction::TRISTATE);
                                        break;
                                    default:
                                        net->setDirection(Net::Direction::UNDEFINED);
                                        break;
                                    }
                                    cerr << net << endl;
                                    oaModNet* oaNet = modTerm->getNet();
                                    if (oaNet->isGlobal()) {
                                        net->setGlobal(true);
                                        cerr << "global" << endl;
                                    }
                                    if (oaNet->getSigType() == oacClockSigType) {
                                        net->setType(Net::Type::CLOCK);
                                    } else if (oaNet->getSigType() == oacPowerSigType) {
                                        net->setGlobal(true);
                                        net->setType(Net::Type::POWER);
                                    } else if (oaNet->getSigType() == oacGroundSigType) {
                                        net->setGlobal(true);
                                        net->setType(Net::Type::GROUND);
                                    } else {
                                        net->setType(Net::Type::LOGICAL);
                                    }


                                }
                            }

                            //physical part
                            oaBlock* block = cellDesign->getTopBlock();
                            if (block && hCell) {
                                oaBox oa_box;
                                block->getBBox(oa_box);
                                cerr << "box values" << endl;
                                cerr << oa_box.lowerLeft().x() << " , " << oa_box.lowerLeft().y() << endl;
                                cerr << oa_box.upperRight().x() << " , " << oa_box.upperRight().y() << endl;
                                cerr << endl;
                                Point lowerLeft(DbU::db(oa_box.lowerLeft().x()), DbU::db(oa_box.lowerLeft().y()));
                                Point upperRight(DbU::db(oa_box.upperRight().x()), DbU::db(oa_box.upperRight().y()));
                                hCell->setAbutmentBox(Box(lowerLeft, upperRight));
                                oaCollection<oaTerm, oaBlock> oaTerms = block->getTerms();
                                oaIter<oaTerm> oaTermIter(oaTerms);
                                while (oaTerm* term = oaTermIter.getNext()) {
                                    oaString oaTermStr;
                                    term->getName(oaNS, oaTermStr);
                                    Net* net = hCell->getNet(Name(oaTermStr));
                                    if (net) {
                                        cerr << "found net : " << net << endl;
                                        oaCollection<oaPin, oaTerm> oaPins(term->getPins());
                                        oaIter<oaPin> oaPinIter(oaPins);
                                        while (oaPin* pin = oaPinIter.getNext()) {
                                            oaString oaPinStr;
                                            pin->getName(oaPinStr);
                                            cerr << "pin : " << oaPinStr << endl;

                                            oaCollection<oaPinFig, oaPin> oaPinFigs(pin->getFigs());
                                            oaIter<oaPinFig> oaPinFigIter(oaPinFigs);
                                            while (oaPinFig* pinFig = oaPinFigIter.getNext()) {
                                                cerr << "pinFig" << endl;
                                                if (pinFig->getType() == oacRectType) {
                                                    oaRect* rect = static_cast<oaRect*>(pinFig);
                                                    OALayerNum2LayerMap::iterator layerMapIt =  _oaLayerNum2LayerMap.find(rect->getLayerNum());
                                                    if (layerMapIt != _oaLayerNum2LayerMap.end()) {
                                                        Layer* layer = layerMapIt->second;
                                                        oaBox pinBBox;
                                                        rect->getBBox(pinBBox);
                                                        DbU::Unit width  = DbU::db( (pinBBox.right() - pinBBox.left()) * (double)convertFactor);
                                                        DbU::Unit height = DbU::db( (pinBBox.top() - pinBBox.bottom()) * (double)convertFactor);
                                                        DbU::Unit cx = DbU::db(pinBBox.right() * (double)convertFactor) + width/2;
                                                        DbU::Unit cy = DbU::db(pinBBox.top() * (double)convertFactor) + height/2;
                                                        if (width > height) {
                                                            Horizontal* horizontal=Horizontal::create(net, layer, cy, height, DbU::db(pinBBox.right() * (double)convertFactor), DbU::db(pinBBox.left() * (double)convertFactor));
                                                            NetExternalComponents::setExternal(horizontal);
                                                        } else {
                                                            Vertical* vertical= Vertical::create(net, layer, cx, width, DbU::db(pinBBox.bottom() * (double)convertFactor), DbU::db(pinBBox.top() * (double)convertFactor));
                                                            NetExternalComponents::setExternal(vertical);
                                                        }
                                                    } else {
                                                        cerr << "cannot find layer..." << endl;
                                                    }

                                                    cerr << "rect " << rect->getLayerNum() << endl;
                                                } else {
                                                    cerr << "no rect" << endl;
                                                }
                                            }
                                        }
                                    } else {
                                        //TODO
                                    }
                                }
                            } else {
                                cerr << "no block" << endl;
                            }
                        }
                    }
                    oaLibrary->releaseAccess();
                    //oaLibrary->close(); do not close !!
                    //FIXME save opened libraries ??
                }
            } catch (oaException  &excp) {
                cout << "ERROR: " << excp.getMsg() << endl;
                exit(1);
            }

        }

        void oaTechnology2Technology(oaLib* oaLibrary) {
            try {
                oaTech* tech = oaTech::open(oaLibrary);

                if (!tech) {
                    cout << "ERROR" << endl;
                    exit(1);
                }
                if (_oaTechnology && tech != _oaTechnology) {
                    cout << "ERROR" << endl;
                    exit(1);
                }
                if (_oaTechnology) {
                    return;
                }
                _oaTechnology = tech;

                DataBase* db = DataBase::getDB();
                if (!db) {
                    cerr << "No DataBase" << endl;
                    exit(8);
                }
                Technology* technology = db->getTechnology();
                if (!technology) {
                    technology = Technology::create(db, "OpenAccess");
                }

                oaCollection<oaLayer, oaTech> oaLayers(_oaTechnology->getLayers());
                oaIter<oaLayer> oaLayerIt(oaLayers);
                while (oaLayer* oaLayer = oaLayerIt.getNext()) {
                    oaString oaLayerStr;
                    oaLayer->getName(oaLayerStr);
                    Name layerName(oaLayerStr);
                    Layer* layer = technology->getLayer(layerName);
                    if (!layer) {
                        layer = BasicLayer::create(technology, layerName, BasicLayer::Material::other, oaLayer->getNumber());
                    }
                    cerr << layer << endl;
                    _oaLayerNum2LayerMap[oaLayer->getNumber()] = layer;
                }
            } catch (oaException  &excp) {
                cout << "ERROR: " << excp.getMsg() << endl;
                exit(1);
            }

        }

        void getDesigns(set<Cell*>& designCellSet) {
            getAllCells(getOADesignLibraries(), designCellSet);
        }
    };//OAParser class
#endif
}//namespace

namespace CRL {
    Cell* OpenAccess::oaCellParser(oaCell* cell){
#ifdef HAVE_OPENACCESS
        cerr << "\nto implement ... " << endl;
#else
        cerr << "\nDummy OpenAccess driver call for " << endl;
#endif
    }
    
    Library* OpenAccess::oaLibParser(oaLib* lib){
#ifdef HAVE_OPENACCESS
        cerr << "\nto implement ... " << endl;
#else
        cerr << "\nDummy OpenAccess driver call for " << path << endl;
#endif
    }

}