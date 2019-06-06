using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MapMaking : MonoBehaviour {

    public int sx = 0;
    public int sz = 0;

    public GameObject[,] tile;

    private List<Node> _path = new List<Node>();
    private Node _start;
    private Node _end;

    private AStarAlgore AStarAl = new AStarAlgore();
    
    private bool MapLoading = false;

    private string maps;
    public void Start()
    {
        string map = "MAPCOORDS";
        Debug.Log(map);
        TextAsset a = (TextAsset)Resources.Load(map, typeof(TextAsset));
        Debug.Log(a.text);

        maps = a.text;
        Load();

    }
    private void OnDrawGizmos()
    {
        Gizmos.color = Color.red;
        if (_path != null)
        {

            foreach (Node n in _path)
            {
                Gizmos.DrawCube(new Vector3(n.X, 1, n.Z), Vector3.one);
                
            }
        }
    }
    public void Load()
    {
        MapLoading = true;

        string[] rows = maps.Split('\n');
        string[] values = rows[0].Split(' ');

        sx = int.Parse(values[0]);
        sz = int.Parse(values[1]);

        tile = new GameObject[sx, sz];

        for (int x = 0; x < sx; x++)
        {
            for (int z = 0; z < sz; z++)
            {
                GameObject TileInstantiation = Instantiate(Resources.Load("Prefab/tile")) as GameObject;
                TileInstantiation.name = "Tile" + x + z;
                TileInstantiation.transform.position = new Vector3((float)x, 0, (float)z);

                Node n = TileInstantiation.GetComponent("Node") as Node;
                n.Visited = false;
                n.G = 0;
                n.H = 0;
                n.Path = false;

                tile[x, z] = TileInstantiation;
            }
        }

        for (int x = 0; x < sx; x++)
        {
            for (int z = 0; z < sz; z++)
            {
                Node n = tile[x, z].GetComponent("Node") as Node;
                if ((x - 1) >= 0 && (z + 1) < sz)
                    n.adjacentNode.Add(tile[x - 1, z + 1].GetComponent("Node") as Node);
                if ((z + 1) < sz)
                    n.adjacentNode.Add(tile[x, z + 1].GetComponent("Node") as Node);
                if ((x + 1) < sx && (z + 1) < sz)
                    n.adjacentNode.Add(tile[x + 1, z + 1].GetComponent("Node") as Node);
                if ((x - 1) >= 0)
                    n.adjacentNode.Add(tile[x - 1, z].GetComponent("Node") as Node);
                if ((x + 1) < sx)
                    n.adjacentNode.Add(tile[x + 1, z].GetComponent("Node") as Node);
                if ((x - 1) >= 0 && (z - 1) >= 0)
                    n.adjacentNode.Add(tile[x - 1, z - 1].GetComponent("Node") as Node);
                if ((z - 1) >= 0)
                    n.adjacentNode.Add(tile[x, z - 1].GetComponent("Node") as Node);
                if ((x + 1) < sx && (z - 1) >= 0)
                    n.adjacentNode.Add(tile[x + 1, z - 1].GetComponent("Node") as Node);
            }
        }
        
        int switchVar = 1;
        foreach (string line in rows)
        {
            values = line.Split(' ');
            Node _obstructed = null;

            switch (switchVar)
            {
                case 1:
                    break;
                case 2:
                    _start = tile[int.Parse(values[0]), int.Parse(values[1])].GetComponent("Node") as Node;
                    _start.Status = Node.StartPoint;
                    break;
                case 3:
                    _end = tile[int.Parse(values[0]), int.Parse(values[1])].GetComponent("Node") as Node;
                    _end.Status = Node.EndPoint;
                    break;
                default:
                    _obstructed = tile[int.Parse(values[0]), int.Parse(values[1])].GetComponent("Node") as Node;
                    _obstructed.Status = Node.Obstruction;
                    break;
            }
            switchVar++;
        }
        MapLoading = false;
    }

    private void Update()
    {
        if (!MapLoading)
        {
            for (int x = 0; x < sx; x++)
            {
                for (int z = 0; z < sz; z++)
                {
                    Node n = tile[x, z].GetComponent<Node>();

                    switch (n.Status)
                    {
                        case Node.EmptyNode:
                            if (n.Path)
                                tile[x, z].GetComponent<Renderer>().material.color = Color.gray;
                            else if (n.Visited)
                                tile[x, z].GetComponent<Renderer>().material.color = Color.Lerp(Color.white, Color.black, 0.5f);
                            else
                                tile[x, z].GetComponent<Renderer>().material.color = Color.Lerp(Color.white, Color.black, 0.7f);
                            break;
                        case Node.StartPoint:
                            tile[x, z].GetComponent<Renderer>().material.color = Color.cyan;
                            break;
                        case Node.EndPoint:
                            tile[x, z].GetComponent<Renderer>().material.color = Color.red;
                            break;
                        case Node.Obstruction:
                            tile[x, z].GetComponent<Renderer>().material.color = Color.black;
                            break;
                    }
                }
            }
            Find();
        }
    }

    public void Find()
    {
        bool found = false;

        found = AStarAl.Find(_start, _end);
        _path = AStarAl.GetPath();
    }


}
