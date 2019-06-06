using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Drawing;
using System.Text.RegularExpressions;
using System.Windows.Media.Imaging;

namespace WizardDungeon
{
    /// <summary>
    /// This class is used to load and save game levels. For loading
    /// and saving there are two main operations, handling textures and
    /// handling the text file that represents a level.
    /// </summary>
    static class CLevelParser
    {
        /// <summary>
        /// This function imports the game textures into a CGameTexture 
        /// object from the given directory. All textures should be present.
        /// </summary>
        /// <param name="directory">The directory where the textures are stored</param>
        /// <returns>An object storing the loading texteures.</returns>
        public static CGameTextures ImportTextures(string directory)
        {
            CGameTextures ret   = new CGameTextures();

            ret.EnemyIcon       = LockFreeBmpLoad(directory + "\\Enemy.bmp");
            ret.GoalIcon        = LockFreeBmpLoad(directory + "\\Goal.bmp");
            ret.PlayerIcon      = LockFreeBmpLoad(directory + "\\Player.bmp");
            ret.WallTexture     = LockFreeBmpLoad(directory + "\\Wall.bmp");
            ret.FloorTexture    = LockFreeBmpLoad(directory + "\\Floor.bmp");
            ret.FireIcon        = LockFreeBmpLoad(directory + "\\Fire.bmp");

            return ret;
        }


        /// <summary>
        /// This function loads an image from the given directory and 
        /// ensures the file is then closed. This is done so that the program
        /// releases a lock on the image and allows it to be overwritten.
        /// </summary>
        /// <param name="Path">The file path of the image.</param>
        /// <returns>The image loaded from the given file path</returns>
        public static BitmapImage LockFreeBmpLoad(string Path)
        {
            BitmapImage image = new BitmapImage();

            using (FileStream stream = File.OpenRead(Path))
            {
                image.BeginInit();
                image.CacheOption = BitmapCacheOption.OnLoad;
                image.StreamSource = stream;
                image.EndInit();
            }
            
            return image;
        }


        /// <summary>
        /// This function will export the given textures to the given directory.
        /// </summary>
        /// <param name="textures">The textures to export.</param>
        /// <param name="directory">The directory to export the textures to.</param>
        public static void ExportTextures(CGameTextures textures, string directory)
        {
            SaveImage(textures.EnemyIcon,   directory + "\\Enemy.bmp");
            SaveImage(textures.GoalIcon,    directory + "\\Goal.bmp");
            SaveImage(textures.PlayerIcon,  directory + "\\Player.bmp");
            SaveImage(textures.WallTexture, directory + "\\Wall.bmp");
            SaveImage(textures.FloorTexture,directory + "\\Floor.bmp");
            SaveImage(textures.FireIcon, directory    + "\\Fire.bmp");
        }


        /// <summary>
        /// This function saves an image to the given filepath.
        /// </summary>
        /// <param name="img">The iamge to save.</param>
        /// <param name="path">The path to save the image to.</param>
        private static void SaveImage(BitmapImage img, string path)
        {
            PngBitmapEncoder encoder = new PngBitmapEncoder();

            encoder.Frames.Add(BitmapFrame.Create(img));

            using(var filestream = new FileStream(path,FileMode.Create))
            {
                encoder.Save(filestream);
            }
        }


        /// <summary>
        /// This function imports the level from the given directory. It is assumed
        /// the text file containing the level is called Level.txt.
        /// </summary>
        /// <param name="directory">The directory the level is stored in.</param>
        /// <returns>The loaded level.</returns>
        public static CLevel ImportLevel(string directory)
        {
            //////////////////////////////////////////////////////
            // Create an instance of a level to populate.

            CLevel ret = new CLevel();

            using (StreamReader fs = new StreamReader(directory + "\\Level.txt"))
            {
                ///////////////////////////////////////////////////////////
                // Read in the dimensions of the level.

                string strDimY = Regex.Match(fs.ReadLine(), @"\d+").Value;
                int DimY = int.Parse(strDimY);

                string strDimX = Regex.Match(fs.ReadLine(), @"\d+").Value;
                int DimX = int.Parse(strDimX);


                ///////////////////////////////////////////////////////////
                // Size the level.

                ret.Resize(DimX, DimY);


                ///////////////////////////////////////////////////////////
                // Read in each tile type.

                for (int y = 0; y < DimY; y++)
                {
                    string strRow = fs.ReadLine();

                    for (int x = 0; x < DimX; x++)
                    {
                        if (strRow[x] == 'F')
                        {
                            ret.SetTileType(x, y, eTileType.Floor);
                        }
                        else
                        {
                            ret.SetTileType(x, y, eTileType.Wall);
                        }
                    }
                }


                /////////////////////////////////////////////////////////////
                // Get the player start position.

                string strIn = fs.ReadLine();
                string[] bits = strIn.Split(',');
                ret.StartPosition = new CPoint2i(int.Parse(Regex.Match(bits[0], @"\d+").Value),
                                                int.Parse(Regex.Match(bits[1], @"\d+").Value));


                //////////////////////////////////////////////////////////////
                // Get the goal position.

                strIn = fs.ReadLine();
                bits = strIn.Split(',');
                ret.GoalPosition = new CPoint2i(int.Parse(Regex.Match(bits[0], @"\d+").Value),
                                               int.Parse(Regex.Match(bits[1], @"\d+").Value));


                //////////////////////////////////////////////////////////////
                // Get the time available.

                ret.Time = int.Parse(Regex.Match(fs.ReadLine(), @"\d+").Value);


                //////////////////////////////////////////////////////////////
                // Get the number of enemies in the level.

                int numEnemies = int.Parse(Regex.Match(fs.ReadLine(), @"\d+").Value);

                ret.EnemyPositions.Capacity = numEnemies;


                ///////////////////////////////////////////////////////////////
                // Read in each enemy from the level.

                for (int i = 0; i < numEnemies; i++)
                {
                    strIn = fs.ReadLine();
                    bits = strIn.Split(',');
                    ret.EnemyPositions.Add(new CPoint2i(int.Parse(Regex.Match(bits[0], @"\d+").Value),
                                                        int.Parse(Regex.Match(bits[1], @"\d+").Value)));
                }


                //////////////////////////////////////////////////////////////
                // Read in the fire locations.

                int numFire = int.Parse(Regex.Match(fs.ReadLine(), @"\d+").Value);

                ret.FirePositions.Capacity = numFire;

                for (int i = 0; i < numFire; i++)
                {
                    strIn = fs.ReadLine();
                    bits = strIn.Split(',');
                    ret.FirePositions.Add(new CPoint2i(int.Parse(Regex.Match(bits[0], @"\d+").Value),
                                                        int.Parse(Regex.Match(bits[1], @"\d+").Value)));
                }
            }
            /////////////////////////////////////////////////////////
            // return the level.

            return ret;
        }


        /// <summary>
        /// This function exports the given level to the given directory.
        /// If there is already a level at the given directory it will be 
        /// overwritten.
        /// </summary>
        /// <param name="levelToExport">The level to export.</param>
        /// <param name="directory">The directory to save the level.</param>
        public static void ExportLevel(CLevel levelToExport, string directory)
        {
             /////////////////////////////////////////////////////
            // Save out level:

            Directory.CreateDirectory(directory);

            using(StreamWriter fs = new StreamWriter(directory + "\\Level.txt"))
            {
                //////////////////////////////////////////////////////
                // Write the number of tiles in the height and width.

                fs.WriteLine("Height=" + levelToExport.Height.ToString());
                fs.WriteLine("Width=" + levelToExport.Width.ToString());


                //////////////////////////////////////////////////////
                // Write out the layout of the tiles.

                for(int y=0; y<levelToExport.Height; y++)
                { 
                    for(int x=0; x<levelToExport.Width; x++)
                    {
                        if(levelToExport.GetTileType(x,y) == eTileType.Floor)
                        {
                            fs.Write('F');
                        }
                        else
                        {
                            fs.Write('W');
                        }
                    }
                    fs.WriteLine();
                }


                //////////////////////////////////////////////////////
                // Save the start position of the player and the goal
                // position.

                fs.WriteLine("StartPosition=" + levelToExport.StartPosition.ToString());
                fs.WriteLine("GoalPosition=" + levelToExport.GoalPosition.ToString());


                fs.WriteLine("TimeToComplete=" + levelToExport.Time.ToString());

                //////////////////////////////////////////////////////
                // Write out the number of enemies and the position of
                // each.

                fs.WriteLine("NumEnemies=" + levelToExport.EnemyPositions.Count.ToString());

                for(int i=0; i<levelToExport.EnemyPositions.Count(); i++)
                {
                    fs.WriteLine(levelToExport.EnemyPositions[i].ToString());
                }


                fs.WriteLine("NumFire=" + levelToExport.FirePositions.Count.ToString());

                for(int i=0; i<levelToExport.FirePositions.Count(); i++)
                {
                    fs.WriteLine(levelToExport.FirePositions[i].ToString());
                }
            }
        }
    }
}
