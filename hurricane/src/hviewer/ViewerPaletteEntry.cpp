
// -*- C++ -*-


# include  <QCheckBox>
# include  <QHBoxLayout>

# include  "BasicLayer.h"

# include  "Graphics.h"
# include  "ViewerPaletteEntry.h"
# include  "Palette.h"


namespace Hurricane {


  ViewerPaletteEntry::ViewerPaletteEntry ( Palette* entry, const Name& name )
    : PaletteEntry(entry)
    , _name(name)
  {
  }


  ViewerPaletteEntry* ViewerPaletteEntry::create ( Palette* palette, const Name& name )
  {
    ViewerPaletteEntry* entry = new ViewerPaletteEntry ( palette, name );

    entry->_postCreate ();

    return entry;
  }


  void  ViewerPaletteEntry::_postCreate ()
  {
    QHBoxLayout* layout = new QHBoxLayout ();
    layout->setContentsMargins ( 0, 0, 0, 0 );

    layout->addWidget ( new PaletteSample(this) );

    _checkBox = new QCheckBox ( this );
    _checkBox->setChecked ( true );
    _checkBox->setText    ( getString(getName()).c_str() );
    layout->addWidget ( _checkBox );

    setLayout ( layout );

    connect ( _checkBox, SIGNAL(clicked()), this, SLOT(toggle()) );
  }


  bool ViewerPaletteEntry::isGroup () const
  {
    return false;
  }


  bool ViewerPaletteEntry::isBasicLayer () const
  {
    return false;
  }


  const Name& ViewerPaletteEntry::getName () const
  {
    return _name;
  }


  BasicLayer* ViewerPaletteEntry::getBasicLayer ()
  {
    return NULL;
  }


  bool  ViewerPaletteEntry::isChecked () const
  {
    return _checkBox->isChecked ();
  }


  void  ViewerPaletteEntry::setChecked ( bool state )
  {
    _checkBox->setChecked ( state );
  }


  void  ViewerPaletteEntry::toggle ()
  {
    _palette->redrawCellWidget();
  }


} // End of Hurricane namespace.