using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Node : MonoBehaviour {
    public const int EmptyNode = 0;
    public const int Obstruction = 1;
    public const int StartPoint = 2;
    public const int EndPoint = 3;
    public const int DrawnPath = 4;

    public int H = 0;
    public int G = 0;
    public int F
    {
        get
        {
            return G + H;
        }
    }

    public bool Visited = false;
    public bool Path = false;
    public Node parent = null;

    public List<Node> adjacentNode = null;

    private int _status;

    public int Status
    {
        get
        {
            return _status;
        }

        set
        {
            if (value >= 0 && value <= 5)
            {
                _status = value;
            }
        }
    }

    public int X
    {
        get { return (int)gameObject.transform.position.x; }
    }
    public int Z
    {
        get { return (int)gameObject.transform.position.z; }
    }

    public bool IsValid()
    {
        return !Visited && (_status != Node.Obstruction);
    }

    public int CompareTo(Node n)
    {
        if (n == null)
            return 1;
        return this.H.CompareTo(n.H);
    }
}
