
// -*- C++ -*-


# ifndef  __LAYER_PALETTE_ENTRY_H__
#   define  __LAYER_PALETTE_ENTRY_H__


class QCheckBox;

# include  "hurricane/viewer/HPaletteEntry.h"


namespace Hurricane {


  class LayerPaletteEntry : public HPaletteEntry {
      Q_OBJECT;

    // Constructor.
    public:
      static  LayerPaletteEntry* create              ( HPalette* palette, BasicLayer* basicLayer );

    // Methods.
    public:
      virtual bool               isGroup             () const;
      virtual bool               isBasicLayer        () const;
      virtual const Name&        getName             () const;
      virtual BasicLayer*        getBasicLayer       ();
      virtual bool               isChecked           () const;
      virtual void               setChecked          ( bool state );

    // Slots.
    public slots:
      virtual void               toggle              ();

    // Internal - Attributes.
    protected:
              BasicLayer*        _basicLayer;
              QCheckBox*         _checkBox;

    // Internal - Constructor.
                                 LayerPaletteEntry  ( HPalette* palette, BasicLayer* basicLayer );
                                 LayerPaletteEntry  ( const LayerPaletteEntry& );
              LayerPaletteEntry& operator=          ( const LayerPaletteEntry& );
      virtual void               _postCreate        ();

  };


} // End of Hurricane namespace.


# endif
