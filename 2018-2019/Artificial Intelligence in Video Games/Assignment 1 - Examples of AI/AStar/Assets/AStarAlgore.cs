using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AStarAlgore : MonoBehaviour {

    private List<Node> _path = new List<Node>();
    private List<Node> _order = new List<Node>();

    private List<Node> _open = new List<Node>();
    private HashSet<Node> _close = new HashSet<Node>();
    
    private int Distance(Node _start, Node _end)
    {
        int x = Mathf.Abs(_start.X - _end.X);
        int z = Mathf.Abs(_start.Z - _end.Z);
        int s = (x < z) ? x : z;
        return (s * 14) + ((int)Mathf.Abs((float)(x - z)) * 10);
    }

    void Retrace(Node a, Node b)
    {
        List<Node> path = new List<Node>();

        Node currentNode = b;
        while(currentNode != a)
        {
            path.Add(currentNode);
            currentNode = currentNode.parent;
        }
        path.Reverse();
        _path = path;
    }

    public bool Find(Node _start, Node _end)
    {
        _start.Visited = true;
        _start.H = Distance(_start, _end);
        _start.G = 0;
        _open.Add(_start);
        while (_open.Count > 0)
        {
            Node c = _open[0];
            _open.RemoveAt(0);
            c.Visited = true;

            if(c.Status == Node.EndPoint) //dis gud
            {
                Retrace(_start, c);
                return true;
            }
            _close.Add(c);

            foreach(Node n in c.adjacentNode)
            {
                if (n.Status == Node.Obstruction || _close.Contains(n)) continue;
                int newMoveCost = c.G + Distance(c, n);
                if(newMoveCost < n.G || !_open.Contains(n))
                {
                    n.G = newMoveCost;
                    n.H = Distance(n, _end);

                    n.parent = c;
                    if (!_open.Contains(n))
                    {
                        _open.Add(n);
                    }
                }
            }
        }
        return false;
    }

    
    public List<Node> GetPath()
    {
        return _path;
    }
    public List <Node> GetOrder()
    {
        return _order;
    }
}
