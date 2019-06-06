using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WizardDungeon
{
    /// <summary>
    /// This class can be used to represent a 2d position in floating precision.
    /// </summary>
    class CPoint2f
    {
        public float X {get; set; }
        public float Y {get; set; }

        public CPoint2f(float x, float y)
        {
            X = x;
            Y = y;
        }

        public CPoint2f()
        {
            X = 0.0f;
            Y = 0.0f;
        }


        /// <summary>
        /// This function allows the compiler to implcitly cast an integer point
        /// position to a floating point position. This is as no precision will be lost.
        /// </summary>
        /// <param name="point">The interger precision point to cast.</param>
        public static implicit operator CPoint2f(CPoint2i point)
        {
            return new CPoint2f((float) point.X, (float) point.Y);
        }


        /// <summary>
        /// This writes the position out in string format.
        /// </summary>
        /// <returns>The string representation of the object.</returns>
        public override string ToString()
        {
            return "{" + X.ToString() + ", " + Y.ToString() + "}";
        }
    }


    /// <summary>
    /// This class can be used to represent a 2d position in integer precision.
    /// </summary>
    class CPoint2i
    {
        public int X {get; set; }
        public int Y {get; set; }

        public CPoint2i(int x, int y)
        {
            X = x;
            Y = y;
        }

        public CPoint2i()
        {
            X = 0;
            Y = 0;
        }


        /// <summary>
        /// This cast operator allows a floating point position to be cast to
        /// an integer position. however, this cast must be explcitly specified
        /// by the programmer as there could be a loss in precision.
        /// </summary>
        /// <param name="point"></param>
        public static explicit operator CPoint2i(CPoint2f point)
        {
            return new CPoint2i((int) point.X, (int) point.Y);
        }


        /// <summary>
        /// This writes the position out in string format.
        /// </summary>
        /// <returns>The string representation of the object.</returns>
        public override string ToString()
        {
            return "{" + X.ToString() + ", " + Y.ToString() + "}";
        }
    }
}
