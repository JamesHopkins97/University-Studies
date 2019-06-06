using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media.Imaging;

namespace WizardDungeon
{
    /// <summary>
    /// This class is used to store all textures used
    /// in the game.
    /// </summary>
    class CGameTextures
    {
        public const int TILE_SIZE = 32;

        public BitmapImage PlayerIcon { get; set; }
        public BitmapImage EnemyIcon { get; set; }
        public BitmapImage GoalIcon { get; set; }
        public BitmapImage WallTexture { get; set; }
        public BitmapImage FloorTexture { get; set; }
        public BitmapImage FireIcon { get; set; }


        /// <summary>
        /// This function checks that all icons have been set. If they have the function will return true.
        /// </summary>
        /// <returns> Bool if all textures are set.</returns>
        public bool IsSet()
        {
            return (PlayerIcon != null && EnemyIcon != null && GoalIcon != null && WallTexture != null && FloorTexture != null && FireIcon != null);
        }
    }
}
