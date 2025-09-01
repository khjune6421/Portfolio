First solo/unity game Random Infinite Maze

## Key Features

- Procedurally generated infinite maze
- node-based generation and pathfinding system

## Sample code (Recursive Backtracking Maze generation algorithm)
```csharp
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MazeAlgorithm : MonoBehaviour
{
    public class Room
    {
        public Vector3 position;
        public bool isVisited = false;
        public bool isPlayerOn = false;
        public bool isExit = false;
        public Wall leftWall;
        public Wall rightWall;
        public Wall downWall;
        public Wall upWall;

        public void CarveMaze()
        {
            isVisited = true;
            int randomPath = Random.Range(0, 3);
            Room[] possibleRooms = new Room[4];
            Wall[] walls = new Wall[4];

            if (leftWall != null)
            {
                walls[0] = leftWall;
                if (leftWall.front != null)
                {
                    possibleRooms[0] = leftWall.front;
                }
            }
            if (rightWall != null)
            {
                walls[1] = rightWall;
                if (rightWall.behind != null)
                {
                    possibleRooms[1] = rightWall.behind;
                }
            }
            if (downWall != null)
            {
                walls[2] = downWall;
                if (downWall.front != null)
                {
                    possibleRooms[2] = downWall.front;
                }
            }
            if (upWall != null)
            {
                walls[3] = upWall;
                if (upWall.behind != null)
                {
                    possibleRooms[3] = upWall.behind;
                }
            }
            for (int i = 0; i < 4; i++)
            {
                int path = (randomPath + i) % 4;
                if (possibleRooms[path] != null)
                {
                    if (possibleRooms[path].isVisited == false)
                    {
                        walls[path].isCarved = true;
                        possibleRooms[path].CarveMaze();
                    }
                }
            }
            return;
        }
    }

    public class Wall
    {
        public bool isCarved = false;
        public Room front;
        public Room behind;
    }

    public static int ROWS = 4;
    public static int COLUMS = 4;
    public static int LEVEL = 8;

    int exitRoomRows = 0;
    int exitRoomColums = 0;
    int randomExitRoomRows = 0;
    int randomExitRoomColums = 0;
    [SerializeField] GameObject mazeRoom;
    [SerializeField] GameObject mazeWall;

    public Room[,] rooms = new Room[ROWS, COLUMS];
    public Wall[,] wallsVerticals = new Wall[ROWS + 1, COLUMS];
    public Wall[,] wallsHorizontals = new Wall[ROWS, COLUMS + 1];

    void Start()
    {
        randomExitRoomRows = Random.Range((ROWS / 2) + 1, ROWS);
        randomExitRoomColums = Random.Range((COLUMS / 2) + 1, COLUMS);
        MakeDefaltMaze();
        Room maxeCarveStartingRoom = rooms[Random.Range(0, ROWS), Random.Range(0, COLUMS)];
        maxeCarveStartingRoom.CarveMaze();
        GenerateMaze();
    }

    void MakeDefaltMaze()
    {
        for (int i = 0; i <= ROWS; i++)
        {
            for (int j = 0; j < COLUMS; j++)
            {
                wallsVerticals[i, j] = new Wall();
            }
        }
        for (int i = 0; i < ROWS; i++)
        {
            for (int j = 0; j <= COLUMS; j++)
            {
                wallsHorizontals[i, j] = new Wall();
            }
        }
        for (int i = 0; i < ROWS; i++)
        {
            exitRoomRows++;
            exitRoomColums = 0;
            for (int j = 0; j < COLUMS; j++)
            {
                exitRoomColums++;
                rooms[i, j] = new Room();
                rooms[i, j].position = new Vector3(i, j, 0.0f);
                rooms[i, j].leftWall = wallsVerticals[i, j];
                rooms[i, j].rightWall = wallsVerticals[i + 1, j];
                rooms[i, j].downWall = wallsHorizontals[i, j];
                rooms[i, j].upWall = wallsHorizontals[i, j + 1];
                if (exitRoomRows == randomExitRoomRows && exitRoomColums == randomExitRoomColums)
                {
                    rooms[i, j].isExit = true;
                }

                if (i > 0)
                {
                    wallsVerticals[i, j].front = rooms[i - 1, j];
                }
                else
                {
                    wallsVerticals[i, j].front = null;
                }
                if (i < ROWS)
                {
                    wallsVerticals[i, j].behind = rooms[i, j];
                }
                else
                {
                    wallsVerticals[i + 1, j].front = rooms[i, j];
                    wallsVerticals[i + 1, j].behind = null;
                }
                if (j > 0)
                {
                    wallsHorizontals[i, j].front = rooms[i, j - 1];
                }
                else
                {
                    wallsHorizontals[i, j].front = null;
                }
                if (j < COLUMS)
                {
                    wallsHorizontals[i, j].behind = rooms[i, j];
                }
                else
                {
                    wallsHorizontals[i, j + 1].front = rooms[i, j];
                    wallsHorizontals[i, j + 1].behind = null;
                }
            }
        }
    }

    void GenerateMaze()
    {
        Quaternion roomRotation = Quaternion.Euler(new Vector3(0.0f, 0.0f, 0.0f));
        Quaternion wallHorizontalRotation = Quaternion.Euler(new Vector3(0.0f, 0.0f, 90.0f));
        Vector3 wallVerticalVector = new Vector3(0.5f, 0.0f, 0.0f);
        Vector3 wallHorizontalVector = new Vector3(0.0f, 0.5f, 0.0f);
        for (int i = 0; i < ROWS; i++)
        {
            for (int j = 0; j < COLUMS; j++)
            {
                GameObject newRoom = Instantiate(mazeRoom, rooms[i, j].position, roomRotation);
                PathfindingNode setPathfindingNode = newRoom.GetComponent<PathfindingNode>();
                if (setPathfindingNode != null)
                {
                    setPathfindingNode.roomInfo = rooms[i, j];
                }
                if (i + 1 < ROWS == false)
                {
                    if (wallsVerticals[i + 1, j].isCarved == false)
                    {
                        GameObject newEndWallVertical = Instantiate(mazeWall, rooms[i, j].position + wallVerticalVector, roomRotation);
                    }
                }
                if (wallsVerticals[i, j].isCarved == false)
                {
                    GameObject newWallVertical = Instantiate(mazeWall, rooms[i, j].position - wallVerticalVector, roomRotation);
                }
                if (j + 1 < COLUMS == false)
                {
                    if (wallsHorizontals[i, j + 1].isCarved == false)
                    {
                        GameObject newEndWallHorizontal = Instantiate(mazeWall, rooms[i, j].position + wallHorizontalVector, wallHorizontalRotation);
                    }
                }
                if (wallsHorizontals[i, j].isCarved == false)
                {
                    GameObject newWallHorizontal = Instantiate(mazeWall, rooms[i, j].position - wallHorizontalVector, wallHorizontalRotation);
                }
            }
        }
    }
}
```

## Game Play Image
<img width="2525" height="883" alt="Image" src="https://github.com/user-attachments/assets/1c6b001c-2fcd-4ee1-a558-2a6d517e1e41" />

## Trivia

was super fun to make, learned a lot about procedural generation and pathfinding algorithms
but maze generation and algorithms are very limited.
Still a lot of room for improvement.