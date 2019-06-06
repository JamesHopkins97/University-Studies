using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;

namespace WizardDungeon
{
    /// <summary>
    /// This class is used to store the position and 
    /// velocity of a dynamic entity. Note that
    /// both the position and velocity are in units of
    /// pixels.
    /// </summary>
    class CSprite
    {
        public CPoint2f Position = new CPoint2f();
        public CPoint2f Velocity = new CPoint2f();
    }

    /// <summary>
    /// An enemy also has inaddition to a position and
    /// velocity a target position. This is the position
    /// the enemy is moving towards.
    /// </summary>
    class CEnemy : CSprite
    {
        public CPoint2f TargetPosition = new CPoint2f();
    }


    /// <summary>
    /// This class represents a state of the game.
    /// It has two main members, the Player which
    /// represents the position and velocity of the
    /// user and an array of Enemy states. There is
    /// one entry in the array for each enemy in 
    /// the game. It is important to note that the
    /// number of enemies must be known when the
    /// object is constructed.
    /// </summary>
    class CGameState
    {
        public CSprite Player = new CSprite();
        public CEnemy[] Enemies;


        public CGameState(int num_enemies)
        {
            if(num_enemies > 0) Enemies = new CEnemy[num_enemies];

            for(int i=0; i<num_enemies; i++)
            {
                Enemies[i] = new CEnemy();
            }
        }
    }
}
