using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Drawing;
using System.Diagnostics;
using System.Threading;
using System.Windows.Threading;

namespace WizardDungeon
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow: Window
    {
        ///////////////////////////////////////////////////////////
        // These variables are all used by the game engine to
        // run the dynamics of the game.

        DispatcherTimer countDown = null;
        DispatcherTimer timer = null;
        Stopwatch watch = new Stopwatch();
        long previous_time = 0;
        int time_ellapsed = 0;


        ///////////////////////////////////////////////////////////
        // The game engine can be used to update the game state. The 
        // game state represents the position and velocities of the
        // player and enemies. Note the enemies do not have a velocity
        // defined as part of the level.

        CGameEngine     gameEngine;
        CGameState      gameState;


        ///////////////////////////////////////////////////////////
        // The level represents the position of walls and floors
        // and contains starting positions of the player and enemy and goal.
        // The game textures stores all the images used to represent
        // different icons.

        CLevel          currentLevel;
        CGameTextures   gameTextures;


        ////////////////////////////////////////////////////////////
        // We keep a reference to these as we need to be able to 
        // update their position in the canvas. We do this by passing
        // a reference to the canvas (e.g. Canvas.SetLeft(enemyIcons[i], Position X);)

        Image[]         enemyIcons;
        Image[]         fireIcons;
        Image           playerIcon;


        /////////////////////////////////////////////////////////////
        // These represent the player and monster speed per frame 
        // (in units of tiles).

        float           player_speed  = 0.15f;
        float           monster_speed = 0.07f;

        public MainWindow()
        {
            InitializeComponent();
        }


        //****************************************************************//
        // This function is used as an event handler for the load click  *//
        // event. This is used to load a new level. In addition to       *//
        // the data required for the level it also ensures the board is  *//
        // displayed.                                                    *//
        //****************************************************************//
        private void btnLoad_Click(object sender,RoutedEventArgs e)
        {

            ////////////////////////////////////////////////////////////
            // clear any existing children from the canvas.

            cvsMainScreen.Children.Clear();

            ///////////////////////////////////////////////////////////
            // Get the directory where the level data is stored and 
            // load the data in. 

            string fileDir = txtLevelDir.Text;

            if (fileDir != "")
            {
                try {
                    currentLevel = CLevelParser.ImportLevel(fileDir);
                    gameTextures = CLevelParser.ImportTextures(fileDir);

                    //Enables buttons, radio buttons, check boxes and textboxes as the level loads
                    btnSave.IsEnabled = true;
                    btnStart.IsEnabled = true;
                    btnTime.IsEnabled = true;

                    rdoEnemy.IsEnabled = true;
                    rdoFire.IsEnabled = true;
                    rdoWall.IsEnabled = true;

                    /*
                    checkFireIcon.IsEnabled = true;
                    checkFloorIcon.IsEnabled = true;
                    checkGhostIcon.IsEnabled = true;
                    checkKnightIcon.IsEnabled = true;
                    checkWallIcon.IsEnabled = true;
                    checkGoalIcon.IsEnabled = true;
                    */

                    txtTime.IsEnabled = true;

                    Render();
                }
                catch
                {
                    lblNoLevel.Visibility = Visibility.Visible;
                }
            }
        }

        void Render()
        {
            ///////////////////////////////////////////////////////////
            // Draw the set of wall and floor tiles for the current
            // level and the goal icon. This is part of the game
            // we do not expect to change as it cannot move.

            DrawLevel();


            //////////////////////////////////////////////////////////
            // Add a game state, this represents the position and velocity
            // of all the enemies and the player. Basically, anything
            // that is dynamic that we expect to move around.

            gameState = new CGameState(currentLevel.EnemyPositions.Count());


            ///////////////////////////////////////////////////////////
            // Set up the player to have the correct .bmp and set it to 
            // its initial starting point. The player's position is stored
            // as a tile index on the Clevel class, this must be converted 
            // to a pixel position on the game state.

            playerIcon = new Image();
            playerIcon.Width = CGameTextures.TILE_SIZE;
            playerIcon.Height = CGameTextures.TILE_SIZE;

            playerIcon.Source = gameTextures.PlayerIcon;

            cvsMainScreen.Children.Add(playerIcon);


            //////////////////////////////////////////////////////////
            // Create instances of the enemies and fires for display. We must do
            // this as each child on a canvas must be a distinct object,
            // we could not simply add the same image multiple times.

            enemyIcons = new Image[currentLevel.EnemyPositions.Count()];

            for (int i = 0; i < currentLevel.EnemyPositions.Count(); i++)
            {
                enemyIcons[i] = new Image();

                enemyIcons[i].Width = CGameTextures.TILE_SIZE;
                enemyIcons[i].Height = CGameTextures.TILE_SIZE;

                enemyIcons[i].Source = gameTextures.EnemyIcon;

                cvsMainScreen.Children.Add(enemyIcons[i]);
            }


            fireIcons = new Image[currentLevel.FirePositions.Count()];

            for (int i = 0; i < currentLevel.FirePositions.Count(); i++)
            {
                fireIcons[i] = new Image();

                fireIcons[i].Width = CGameTextures.TILE_SIZE;
                fireIcons[i].Height = CGameTextures.TILE_SIZE;

                fireIcons[i].Source = gameTextures.FireIcon;

                cvsMainScreen.Children.Add(fireIcons[i]);

                CPoint2i tilePosition = CLevelUtils.GetPixelFromTileCoordinates(new CPoint2i(currentLevel.FirePositions[i].X, currentLevel.FirePositions[i].Y));


                Canvas.SetLeft(fireIcons[i], tilePosition.X);
                Canvas.SetTop(fireIcons[i], tilePosition.Y);
            }


            ////////////////////////////////////////////////////////////
            // Set each instance of a dynamic object to its initial position
            // as defined by the current level object.

            InitialiseGameState();


            ////////////////////////////////////////////////////////////
            // Render the current game state, this will render the player
            // and the enemies in their initial position.

            RenderGameState();
        }


        //****************************************************************//
        // This initilaises the dynamic parts of the game to their initial//
        // positions as specified by the game level.                      //
        //****************************************************************//
        private void InitialiseGameState()
        {
            ////////////////////////////////////////////////////////////
            // Place the player at their initial position.

            gameState.Player.Position = CLevelUtils.GetPixelFromTileCoordinates(currentLevel.StartPosition);

            Random random = new Random();

            for(int i=0; i<currentLevel.EnemyPositions.Count(); i++)
            {
                ////////////////////////////////////////////////////////////
                // Place each enemy at their initial position and give them an
                // initial random direction.

                gameState.Enemies[i].Position = CLevelUtils.GetPixelFromTileCoordinates(currentLevel.EnemyPositions[i]);

                gameState.Enemies[i].TargetPosition.X = gameState.Enemies[i].Position.X;
                gameState.Enemies[i].TargetPosition.Y = gameState.Enemies[i].Position.Y;


                /////////////////////////////////////////////////////////////
                // Create a random direction to walk in.

                int direction = random.Next()%4;

                switch(direction)
                {
                    case 0:
                        gameState.Enemies[i].Velocity.X = monster_speed;
                        gameState.Enemies[i].Velocity.Y = 0.0f;
                        break;

                    case 1:
                        gameState.Enemies[i].Velocity.X =-monster_speed;
                        gameState.Enemies[i].Velocity.Y = 0.0f;
                        break;

                    case 2:
                        gameState.Enemies[i].Velocity.X = 0.0f;
                        gameState.Enemies[i].Velocity.Y = monster_speed;
                        break;

                    case 3:
                    default:
                        gameState.Enemies[i].Velocity.X = 0.0f;
                        gameState.Enemies[i].Velocity.Y =-monster_speed;
                        break;
                }
            }
        }


        //****************************************************************//
        // This function renders the dynamic content of the game to the  *//
        // main canvas. This is done by updating the positions of all    *//
        // the sprite icons in the canvas using the current game state   *//
        // and then invoking the canvas to refresh.                      *//
        //****************************************************************//
        private void RenderGameState()
        {
            Canvas.SetLeft(playerIcon, gameState.Player.Position.X);
            Canvas.SetTop (playerIcon, gameState.Player.Position.Y);


            for(int i=0; i<currentLevel.EnemyPositions.Count(); i++)
            {
                Canvas.SetLeft(enemyIcons[i], gameState.Enemies[i].Position.X);
                Canvas.SetTop (enemyIcons[i], gameState.Enemies[i].Position.Y);
            } 
        }


        //****************************************************************//
        // This function draws the static parts of the level onto the    *//
        // canvas.                                                       *//
        //****************************************************************//
        private void DrawLevel()
        {
            /////////////////////////////////////////////////////////////
            // Compute the width of the canvas, this will be the number
            // of tiles multiplied by the tile size (in pixels).

            int width   = currentLevel.Width*CGameTextures.TILE_SIZE;
            int height  = currentLevel.Height*CGameTextures.TILE_SIZE;

            cvsMainScreen.Width     = width;
            cvsMainScreen.Height    = height;


            /////////////////////////////////////////////////////////////
            // Loop through the level setting each tiled position on the 
            // canvas.

            for(int y=0; y<currentLevel.Height; y++)
            {
                for (int x=0; x<currentLevel.Width; x++)
                {
                    /////////////////////////////////////////////////////////
                    // We must create a new instance of the image as an image
                    // can only be added once to a given canvas.

                    Image texture   = new Image();
                    texture.Width   = CGameTextures.TILE_SIZE;
                    texture.Height  = CGameTextures.TILE_SIZE;


                    //////////////////////////////////////////////////////////
                    // Set the position of the tile, we must convert from tile
                    // coordinates to pixel coordinates.

                    CPoint2i tilePosition = CLevelUtils.GetPixelFromTileCoordinates(new CPoint2i(x,y));

                    
                    Canvas.SetLeft(texture, tilePosition.X);
                    Canvas.SetTop (texture, tilePosition.Y);
                    

                    //////////////////////////////////////////////////////////
                    // Check whether it should be a wall tile or floor tile.

                    if(currentLevel.GetTileType(x,y) == eTileType.Wall)
                    {
                       texture.Source = gameTextures.WallTexture;
                    }
                    else
                    {
                        texture.Source = gameTextures.FloorTexture;
                    }

                    cvsMainScreen.Children.Add(texture);
                }
            }


            ////////////////////////////////////////////////////////////
            // The goal is also static as it does not move so we will
            // also add this now also.

            Image goalImg   = new Image();
            goalImg.Width   = CGameTextures.TILE_SIZE;
            goalImg.Height  = CGameTextures.TILE_SIZE;

            goalImg.Source  = gameTextures.GoalIcon;

            CPoint2i GoalPosition = CLevelUtils.GetPixelFromTileCoordinates(new CPoint2i(currentLevel.GoalPosition.X, currentLevel.GoalPosition.Y));

            Canvas.SetLeft(goalImg, GoalPosition.X);
            Canvas.SetTop (goalImg, GoalPosition.Y);

            cvsMainScreen.Children.Add(goalImg);
        }


        //****************************************************************//
        // This event handler is used to handle a key being pressed.     *//
        // It only takes action if a direction key is pressed.           *//
        //****************************************************************//
        private void tb_KeyDown(object sender, KeyEventArgs args)
        {
            //////////////////////////////////////////////////////////
            // We set the players velocity, this controls which direction
            // it will move in.

            if(Keyboard.IsKeyDown(Key.Up))
            {
                gameState.Player.Velocity.Y=-player_speed;
                gameState.Player.Velocity.X= 0.0f;
            }
            else if(Keyboard.IsKeyDown(Key.Down))
            {
                gameState.Player.Velocity.Y= player_speed;
                gameState.Player.Velocity.X= 0.0f;
            }
            else if(Keyboard.IsKeyDown(Key.Left))
            {
                gameState.Player.Velocity.Y= 0.0f;
                gameState.Player.Velocity.X=-player_speed;
            }
            else if(Keyboard.IsKeyDown(Key.Right))
            {
                gameState.Player.Velocity.Y= 0.0f;
                gameState.Player.Velocity.X= player_speed;
            }
        }

        //****************************************************************//
        // This event handler is used to handle a key being released.    *//
        // We presume the direction key is being released.               *//
        //****************************************************************//
        private void tb_KeyUp(object sender, KeyEventArgs args)
        {
            ///////////////////////////////////////////////////////////////
            // By setting a player's velocity to zero it will not move.
            if(args.Key == Key.Up || args.Key == Key.Down || args.Key == Key.Left || args.Key == Key.Right)
            {
                gameState.Player.Velocity.X = 0.0f;
                gameState.Player.Velocity.Y = 0.0f;
            }
            
        }


        //****************************************************************//
        // This event handler is used to start the game.                 *//
        //****************************************************************//
        private void btnStart_Click(object sender,RoutedEventArgs e)
        {
            lblMsg.Content = "";

            watch.Reset();
            watch.Start();
            previous_time = 0;


            //////////////////////////////////////////////////////////////
            // Create a new game engine to handle the interactions and 
            // register events to handle collisions with enemies or
            // winning the game.


            gameEngine = new CGameEngine(currentLevel);
            gameEngine.OnGoalReached  += EndGame;
            gameEngine.OnPlayerCaught += EndGame;


            //////////////////////////////////////////////////////////////
            // The game is rendered by using a dispatchTimer. This will 
            // trigger the game loop (RunTime) every 40ms.

            if(timer == null)
            {
                timer = new DispatcherTimer();
                timer.Tick += RunGame;
                //timer.Interval = new TimeSpan(0,0,0,0,40);

                countDown = new DispatcherTimer();
                countDown.Tick += UpdateTime;
                countDown.Interval = new TimeSpan(0,0,0,1,0);
            }
            time_ellapsed=0;
            countDown.Start();
            timer.Start();
            


            /////////////////////////////////////////////////////////////
            // Make some of the elements on screen disabled so that the
            // user can't restart mid game play.

            txtLevelDir.IsEnabled   = false;
            btnStart.IsEnabled      = false;
            btnLoad.IsEnabled       = false;
            Keyboard.Focus(cvsMainScreen);

            ////////////////////////////////////////////////////////////
            // Set each instance of a dynamic object to its initial position.

            InitialiseGameState();


            ////////////////////////////////////////////////////////////
            // Render the current game state, this will render the player
            // and the enemies in their initial position.

            RenderGameState();
        }


        private void UpdateTime(object sender, object o)
        {
            time_ellapsed++;

            if (time_ellapsed < currentLevel.Time)
                lblMsg.Content = "Time Remaining: " + (currentLevel.Time - time_ellapsed);
            else
                EndGame(this, "You lose, you ran out of time!");

        }

        //****************************************************************//
        // This function represents the main game loop. It computes the   //
        // time between this and the previous call and then updates the   //
        // game state. It then requests the new game state to be          //
        // rendered.                                                      //
        //****************************************************************//
        private void RunGame(object sender, object o)
        {
            //////////////////////////////////////////////////////////////
            // Compute the difference in time between two consecutive 
            // calls.

            long current_time = watch.ElapsedMilliseconds;
            long time_delta = current_time - previous_time;
            previous_time = current_time;


            //////////////////////////////////////////////////////////////
            // Update and render the game.

            gameEngine.UpdateVelocities(gameState, (float)time_delta);
            gameEngine.UpdatePositions(gameState,  (float)time_delta);

            RenderGameState();
        }


        //****************************************************************//
        // This function will be registered to be triggered when the game //
        // finishes. It re-enables any buttons and displays a message to  //
        // indicate the end result of the game.                           //
        //****************************************************************//
        public void EndGame(object sender, string message)
        {
            countDown.Stop();
            lblMsg.Content = message;
            timer.Stop();
            txtLevelDir.IsEnabled   = true;
            btnStart.IsEnabled      = true;
            btnLoad.IsEnabled       = true;
        }

        private void cvsMainScreen_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            //Get the cursor location on the map
            Point P = e.GetPosition(cvsMainScreen);
            CPoint2i pixelPos = new CPoint2i((int)P.X, (int)P.Y);
            CPoint2i mapPos = CLevelUtils.GetTileCoordinatesFromPixel(pixelPos);

            //Change the texture to the selected texture
            //BitmapImage SelectedImage = SetTexture();

            //If rdo wall is ticked change the wall to floor and vice versa
            if (rdoWall.IsChecked == true)
            {
                if (currentLevel.GetTileType(mapPos.X, mapPos.Y) == eTileType.Wall)
                {
                    //gameTextures.FloorTexture = SelectedImage;
                    currentLevel.SetTileType(mapPos.X, mapPos.Y, eTileType.Floor);
                }
                else
                {
                    //gameTextures.WallTexture = SelectedImage;
                    currentLevel.SetTileType(mapPos.X, mapPos.Y, eTileType.Wall);
                }
            }

            //If rdo Enemy is ticked place and delete enemies from the map
            if (rdoEnemy.IsChecked == true)
            {
                bool enemyExists = false;
                int enemyPos = 0;
                for (enemyPos = 0; enemyPos < currentLevel.EnemyPositions.Count; enemyPos++)
                {
                    if (currentLevel.EnemyPositions[enemyPos].X == mapPos.X && currentLevel.EnemyPositions[enemyPos].Y == mapPos.Y)
                    {
                        enemyExists = true;
                        break;
                    }
                }

                //if an enemy exists at mapPos remove it, if not add an enemy.
                if (enemyExists)
                    currentLevel.EnemyPositions.RemoveAt(enemyPos);
                else
                    currentLevel.EnemyPositions.Add(mapPos);
            }

            //If rdo wall is ticked place and delete fire from the map
            if (rdoFire.IsChecked == true)
            {
                bool fireExists = false;
                int firePos = 0;
                for (firePos = 0; firePos < currentLevel.FirePositions.Count; firePos++)
                {
                    if (currentLevel.FirePositions[firePos].X == mapPos.X && currentLevel.FirePositions[firePos].Y == mapPos.Y)
                    {
                        fireExists = true;
                        break;
                    }
                }

                //if fire exists at mapPos remove it, if not add fire.
                if (fireExists)
                    currentLevel.FirePositions.RemoveAt(firePos);
                else
                    currentLevel.FirePositions.Add(mapPos);
            }

            Render();
            // Render the scene upon clicking
        }

        public BitmapImage SetTexture()
        {
            //Change Knight Texture
            if (checkKnightIcon.IsChecked == true)
                return gameTextures.PlayerIcon;

            //Change Fire Texture
            else if (checkFireIcon.IsChecked == true)
                return gameTextures.FireIcon;

            //Change Enemy Texture
            else if (checkGhostIcon.IsChecked == true)
                return gameTextures.EnemyIcon;

            //Change Goal Texture
            else if (checkGoalIcon.IsChecked == true)
                return gameTextures.GoalIcon;

            //Change Floor Texture
            else if (checkFloorIcon.IsChecked == true)
                return gameTextures.FloorTexture;

            //Change Wall Texture
            else if (checkWallIcon.IsChecked == true)
                return gameTextures.WallTexture;

            //If Nothing is chosen don't change texture
            else
                return null;
        }

        private void btnSave_Click(object sender, RoutedEventArgs e)
        {
            // Saves the game under a new level
            CLevelParser.ExportLevel(currentLevel, txtLevelDir.Text);
        }
        
        private void btnTime_Click(object sender, RoutedEventArgs e)
        {
            // Gets the value of txtTime and converts into an int for the currentLevel to use
            string time = txtTime.Text;
            if (time != "")
            {
                int valueTime = int.Parse(time);
                if (valueTime > 0)
                {
                    lblTimeError.Visibility = Visibility.Hidden;
                    currentLevel.Time = valueTime;
                }
                else
                    lblTimeError.Visibility = Visibility.Visible;
            }
        }
    }
}
