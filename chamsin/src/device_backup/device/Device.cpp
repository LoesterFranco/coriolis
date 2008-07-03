// ****************************************************************************************************
// File: Device.cpp
// Authors: Wu YiFei
// Date   : 21/12/2006 
// ****************************************************************************************************


#include "Device.h"

#include "Transformation.h"
#include "Point.h"
#include "Instance.h"
#include "Box.h"
#include "Error.h"

#include "Cells.h"
#include "DtrAccess.h"
using namespace Hurricane;

// ****************************************************************************************************
// Static data function 
// ****************************************************************************************************

static Instance * refins = NULL;


static set<Cell*> cellSet;


static void getAllCells(Cell* cell)
// ********************************
{
   cellSet.insert(cell);
   
   if(!(cell->isLeaf())){
       for_each_instance(instance, cell->getInstances())
	 Cell * mastercell = instance->getMasterCell();
         getAllCells(mastercell); 
       end_for	 
   }  
}



namespace DEVICE {


// ****************************************************************************************************
// Device implementation
// ****************************************************************************************************

Device::Device(Library* library, const Name& name)
// **************************************************************************
:	Inherit(library, name)
{
}


void Device::_postCreate() {
   Inherit::_postCreate();

   //CDataBase* database = getCDataBase();
   //CCatal* ccatal = database->getCCatal();
  
   //CCatal::State* state = ccatal->getState(getName(), true);
   //state->SetFlags(CCatal::State::LOGICAL|CCatal::State::PHYSICAL|CCatal::State::IN_MEMORY|CCatal::State::GDS, true);
   //state->SetCell(this);
   //state->SetLibrary(getLibrary());

}


void Device::SaveLogicalView()
// ***************************
{
   cellSet.clear();
   getAllCells(this);

   //CDataBase * db = getCDataBase();

  // set<Cell*>::iterator i = cellSet.begin(), j = cellSet.end();
  // 
  // while(i!=j) {
  //   db->SaveCell(*i, CCatal::State::LOGICAL ); 
  //   i++;
  // }
}  

void Device::_Place(Instance* ins, const Transformation::Orientation& orientation, const Point& point)
// **************************************************************************************************  
{
    if(!ins) {
       throw Error("Can't Place Instance : ins is NULL");
    } 

    if(ins->isPlaced()) {
       throw Error("Can't Place " + getString(ins) + " : it has already been placed");
    }

    Transformation transformation(Point(0,0), orientation);
    Box orientedmastercellbox = transformation.getBox(ins->getMasterCell()->getAbutmentBox());
    
    Point translation( point.getX() - orientedmastercellbox.getXMin()
	             , point.getY() - orientedmastercellbox.getYMin() );

    Transformation transformation_ins = Transformation(translation, orientation);
    
    ins->setTransformation(transformation_ins);
    ins->setPlacementStatus(Instance::PlacementStatus::PLACED);
}


void Device::_setRefIns(Instance* ins) const
// *****************************************
{
   if(!ins) {
       throw Error("Can't SetRefIns : ref instance is NULL");
   }
  
   if(ins->isUnplaced()) {
       throw Error("Can't SetRefIns : ref instance has't been placed");
   }
   
   refins = ins;
}  


void Device::_PlaceRight(Instance* ins, const Transformation::Orientation& orientation, const Point& offset)
// ********************************************************************************************************  
{
    if(!ins) {
       throw Error("Can't PlaceRight Instance : ins is NULL");
    } 

    if(ins->isPlaced()) {
       throw Error("Can't PlaceRight " + getString(ins) + " : it has already been placed");
    }


    if(!refins) {
      throw Error("Can't Place Right " + getString(ins) + " :  can't find refins");
    }
  
    Box refinsbox = refins->getAbutmentBox();
    
    Transformation transformation(Point(0,0), orientation);
    Box orientedmastercellbox = transformation.getBox(ins->getMasterCell()->getAbutmentBox());
    
    Point translation( refinsbox.getXMax() - orientedmastercellbox.getXMin() + offset.getX()
	             , refinsbox.getYMin() - orientedmastercellbox.getYMin() + offset.getY() );

    Transformation transformation_ins = Transformation(translation, orientation);
    
    ins->setTransformation(transformation_ins);
    ins->setPlacementStatus(Instance::PlacementStatus::PLACED);

    refins = ins;
}


} // end namespace Device
