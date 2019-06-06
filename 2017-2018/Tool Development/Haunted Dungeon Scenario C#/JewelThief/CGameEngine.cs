using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WizardDungeon
{
    class CGameEngine
    {
        /// <summary>
        /// These tolerances are squared for efficiency (to save a square root)
        /// they are used to check if the player is in collision with a wall or
        /// player, or have reached the tile centre.
        /// </summary>

        private const int COLLISION_TOLERANCE_SQ = 700;
        private const int AT_TARGET_TOLERANCE_SQ = 25;

        private CLevel m_level; 
        private int[]  m_boardDims = new int[2];

        private Random m_randomGenerator = new Random();


        ////////////////////////////////////////////////////
        // Event handler delegate used for sending an event
        // that either the player was Caught or the Goal
        // was reached.

        public delegate void CGameEngineEventHandler(object sender, string msg);

        public event CGameEngineEventHandler OnPlayerCaught;
        public event CGameEngineEventHandler OnGoalReached;


        //****************************************************************//
        // This constructors constructs an instance of the game engine. It//
        // requires to know the level and the size of each tile.          //
        //****************************************************************//
        public CGameEngine(CLevel level)
        {
            m_level     = level;


            //////////////////////////////////////////////////
            // The board dimensions are in units of pixels.

            m_boardDims[0] = m_level.Width*CGameTextures.TILE_SIZE;
            m_boardDims[1] = m_level.Height*CGameTextures.TILE_SIZE;
        }


        //****************************************************************//
        // This function updates the velocity of each enemy in the game   //
        // depending on whether it is in collision with a wall or an      //
        // alternative move is available (e.g. turn left or right). It    //
        // will only move backwards if no other move is available and a   //
        // dead end has been encountered.                                 //
        //****************************************************************//
        public void UpdateVelocities(CGameState gameState, float time_delta)
        {
            /////////////////////////////////////////////////
            // We only update the velocities of the enemies, 
            // the player is done through keyboard events.

            /////////////////////////////////////////////////////////
            // Each enemy has a targetPosition aswell as a Position.
            // the target position is the centre of the square the enemy
            // is moving towards. this is done so that a monster always
            // walks along the centre of a corridor.
            // When we compute the velocity we are allowing the enemy to
            // change direction if he is close to a square centre.
            // If he is within 5 pixels of the target square we take the following steps:
            // 1. Update his position to be at the target position.
            // 2. Test if there are any available squares in any direction (forwards, left, right).
            // 3. If there are randomly select a direction and update velocity.
            // 4. Set the target as one complete tile in the new direction.
            // 5. If no squares are available set velocity to be backwards and target square the
            // next square backwards.
        

            foreach(CEnemy enemy in gameState.Enemies)
            {
                if(Math.Pow(enemy.TargetPosition.X-enemy.Position.X, 2.0) + Math.Pow(enemy.TargetPosition.Y-enemy.Position.Y, 2.0) <= AT_TARGET_TOLERANCE_SQ)
                {
                    /////////////////////////////////////////////////////////
                    // Move the enemy to the target position.

                    enemy.Position.X = enemy.TargetPosition.X;
                    enemy.Position.Y = enemy.TargetPosition.Y;

                    float xVel = 0.0f;
                    float yVel = 0.0f;


                    ////////////////////////////////////////////////////////
                    // Set the velocity to be a complete tile in the
                    // current direction.

                    if(enemy.Velocity.X != 0.0f)
                    {
                        xVel = Math.Sign(enemy.Velocity.X)*CGameTextures.TILE_SIZE;
                    }
                    else
                    {
                        yVel = Math.Sign(enemy.Velocity.Y)*CGameTextures.TILE_SIZE; 
                    }

                    bool[] ValidMoves = new bool[3] { false, false, false};
                    bool valid_move_found = false;

                    CPoint2f testPos = new CPoint2f();


                    ////////////////////////////////////////////////////////
                    // At each central tile position we test if the monster
                    // can move in any direction other than backwards.

                    testPos.X = enemy.Position.X+xVel;
                    testPos.Y = enemy.Position.Y+yVel;

                    if(!IsInCollisionWithWall(testPos) && !IsInCollisionWithFire(testPos))
                    {
                        ValidMoves[0]       = true;
                        valid_move_found    = true;
                    }

                    testPos.X = enemy.Position.X+yVel;
                    testPos.Y = enemy.Position.Y+xVel;

                    if(!IsInCollisionWithWall(testPos) && !IsInCollisionWithFire(testPos))
                    {
                        ValidMoves[1]       = true;
                        valid_move_found    = true;
                    }

                    testPos.X = enemy.Position.X-yVel;
                    testPos.Y = enemy.Position.Y-xVel;

                    if(!IsInCollisionWithWall(testPos) && !IsInCollisionWithFire(testPos))
                    {
                        ValidMoves[2]       = true;
                        valid_move_found    = true;
                    }


                    /////////////////////////////////////////////////////////
                    // If we've found a valid move we randomly select a new
                    // direction. Otherwise we move backwards (since this must
                    // be valid).

                    if(valid_move_found)
                    {
                        //////////////////////////////////////////////////////
                        // Find a valid random move (this is a little inefficient).

                        int i = m_randomGenerator.Next()%3;
                        while(!ValidMoves[i]) i= m_randomGenerator.Next()%3;

                        switch(i)
                        {
                            case 0:
                                enemy.TargetPosition.X = enemy.Position.X+xVel;
                                enemy.TargetPosition.Y = enemy.Position.Y+yVel;
                           
                                enemy.Velocity.X = enemy.Velocity.X;
                                enemy.Velocity.Y = enemy.Velocity.Y;
                                break;
                            case 1:
                                enemy.TargetPosition.X = enemy.Position.X+yVel;
                                enemy.TargetPosition.Y = enemy.Position.Y+xVel;
                           
                                float temp = enemy.Velocity.X;
                                enemy.Velocity.X = enemy.Velocity.Y;
                                enemy.Velocity.Y = temp;
                                break;
                            default:
                            case 2:
                                enemy.TargetPosition.X = enemy.Position.X-yVel;
                                enemy.TargetPosition.Y = enemy.Position.Y-xVel;
                           
                                temp = enemy.Velocity.X;
                                enemy.Velocity.X = -enemy.Velocity.Y;
                                enemy.Velocity.Y = -temp;
                                break;
                        }

                    }
                    else
                    {
                        ////////////////////////////////////////////////////////////
                        // No move is available so we will move backwards.

                        enemy.TargetPosition.X = enemy.Position.X-xVel;
                        enemy.TargetPosition.Y = enemy.Position.Y-yVel;

                        enemy.Velocity.X = -enemy.Velocity.X;
                        enemy.Velocity.Y = -enemy.Velocity.Y;
                    }
                }
            }
        }


        //****************************************************************//
        // This function updates the position of each enemy in the game   //
        // and the player, it then checks if there are collisions between //
        // the enemy or the player and raises a PlayerCaught event.       //
        // It also checks if the player has reached the end goal.         //
        //****************************************************************//
        public void UpdatePositions(CGameState gameState, float time_delta)
        {

            ///////////////////////////////////////////////////////////////////
            // First we update the positions of the players and enemies.

            foreach(CSprite sprite in gameState.Enemies)
            {
                UpdatePosition(sprite, time_delta);
            }

            UpdatePosition(gameState.Player, time_delta);


            ///////////////////////////////////////////////////////////////////
            // Next we check if our player has reached the goal position.

            CPoint2f goalPositionPixels = CLevelUtils.GetPixelFromTileCoordinates(m_level.GoalPosition);

            if(IsInCollision(gameState.Player.Position, goalPositionPixels))
            {
                OnGoalReached(this, "You Won!! Goal Reached!!");
                return;
            }
            

            ///////////////////////////////////////////////////////////////////
            // Next we check each enemy in turn to see if the user has been 
            // caught.

            foreach(CSprite sprite in gameState.Enemies)
            {
                if(IsInCollision(gameState.Player.Position, sprite.Position))
                {
                    OnPlayerCaught(this, "You Lose!! Player Caught!!");
                    break;
                }
            }
        }


        //****************************************************************//
        // This function updates the position of each enemy in the game.  //
        // If it would be in collision with a wall the position is not    //
        // changed.                                                       //
        //****************************************************************//
        private void UpdatePosition(CSprite sprite, float time_delta)
        {
            //////////////////////////////////////////////////////////////////
            // Update a sprite to be in a new position using it's velocity.
            // If it is in a collision, we leave it in current location.
            // however, if it is an enemy we move it to it's target position
            // ready to find a new direction to move into.

            CPoint2f newPos = new CPoint2f();

            newPos.X = sprite.Position.X + time_delta*sprite.Velocity.X;
            newPos.Y = sprite.Position.Y + time_delta*sprite.Velocity.Y;

            bool collisionFire = false;

            if(sprite is CEnemy)
            {
                collisionFire = IsInCollisionWithFire(newPos);
            }

            if(!IsInCollisionWithWall(newPos) && !collisionFire)
            {
                sprite.Position.X = newPos.X;
                sprite.Position.Y = newPos.Y;
            }
            else
            {
                ///////////////////////////////////////////////////////////////
                // See if sprite is an enemy, if so update it's position to be
                // the target position. NOTE: this could make odd behaviour if
                // enemy walks too far!

                if(sprite is CEnemy)
                {
                    CEnemy enemy = (CEnemy)sprite;

                    enemy.Position.X = enemy.TargetPosition.X;
                    enemy.Position.Y = enemy.TargetPosition.Y;
                }
            }
        }

        //****************************************************************//
        // This function tests if a given position is in collision with a //
        // wall, if so it will return true.                               //
        //****************************************************************//
        private bool IsInCollisionWithWall(CPoint2f pos)
        {
            /////////////////////////////////////////////////////////////////
            // First we check if we've moved outside of the board this will also
            // return true.

            if(pos.X < 0 || pos.Y < 0 || pos.X+CGameTextures.TILE_SIZE > m_boardDims[0] || pos.Y+CGameTextures.TILE_SIZE > m_boardDims[1])
            {
                return true;
            }


            //////////////////////////////////////////////////////////////////
            // Next we calculate the centre of the tile and see if this is in
            // collision. This will allow the player to walk a little bit (50%) 
            // into walls.

            CPoint2i centre = new CPoint2i();

            centre.X = (int)pos.X + CGameTextures.TILE_SIZE/2;
            centre.Y = (int)pos.Y + CGameTextures.TILE_SIZE/2;

            CPoint2i tilePos = CLevelUtils.GetTileCoordinatesFromPixel(centre);

            return (m_level.GetTileType(tilePos.X,tilePos.Y) == eTileType.Wall);
        }

        //****************************************************************//
        // This function tests if a given position is in collision with a //
        // fire, if so it will return true.                               //
        //****************************************************************//
        private bool IsInCollisionWithFire(CPoint2f pos)
        {
            /////////////////////////////////////////////////////////////////
            // First we check if we've moved outside of the board this will also
            // return true.

            if(pos.X < 0 || pos.Y < 0 || pos.X+CGameTextures.TILE_SIZE > m_boardDims[0] || pos.Y+CGameTextures.TILE_SIZE > m_boardDims[1])
            {
                return true;
            }


            //////////////////////////////////////////////////////////////////
            // Next we calculate the centre of the tile and see if this is in
            // collision. This will allow the player to walk a little bit (50%) 
            // into walls.

            CPoint2i centre = new CPoint2i();

            centre.X = (int)pos.X + CGameTextures.TILE_SIZE/2;
            centre.Y = (int)pos.Y + CGameTextures.TILE_SIZE/2;

            CPoint2i tilePos = CLevelUtils.GetTileCoordinatesFromPixel(centre);

            foreach(CPoint2f posFire in m_level.FirePositions)
            {
                if((Math.Pow(tilePos.X-posFire.X, 2.0) + Math.Pow(tilePos.Y-posFire.Y, 2.0)) <= 0.1)
                {
                    return true;
                }
            }

            return false;
        }


        //****************************************************************//
        // This function tests if two given positions are within a        //
        // tolerance, if so it will return true.                          //
        //****************************************************************//
        private bool IsInCollision(CPoint2f posA, CPoint2f posB)
        {
            return (Math.Pow(posA.X-posB.X, 2.0) + Math.Pow(posA.Y-posB.Y, 2.0)) <= COLLISION_TOLERANCE_SQ;
        }
    }
}
