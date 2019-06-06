using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WizardDungeon
{
    class CLevelUtils
    {
        //***********************************************************************//
        // This function can be used to compute a tile position from a given     //
        // pixel position.                                                       //
        //***********************************************************************//
        public static CPoint2i GetTileCoordinatesFromPixel(CPoint2i PixelPosition)
        {
            CPoint2i ret = new CPoint2i();
            ret.X = PixelPosition.X/CGameTextures.TILE_SIZE;
            ret.Y = PixelPosition.Y/CGameTextures.TILE_SIZE;
            return ret;
        }


        //***********************************************************************//
        // This function can be used to compute a pixel position from a given    //
        // tile position.                                                        //
        //***********************************************************************//
        public static CPoint2i GetPixelFromTileCoordinates(CPoint2i TileCoordinate)
        {
            CPoint2i ret = new CPoint2i();
            ret.X = TileCoordinate.X*CGameTextures.TILE_SIZE;
            ret.Y = TileCoordinate.Y*CGameTextures.TILE_SIZE;
            return ret;
        }
    }
}
