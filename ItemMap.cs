using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ItemMap : MonoBehaviour
{
    public MazeAlgorithm.Room currentNode = null;
    Vector3[] pathArr = new Vector3[MazeAlgorithm.COLUMS * MazeAlgorithm.ROWS];
    float moveSpeed = 3.0f;
    int pathIndexCount = 0;
    bool isPathfound = false;
    float movedistance = 0.0f;
    int pathMoveIndex = 0;
    bool isMoving = false;
    void Start()
    {
        moveSpeed = 3.0f + PlayerControl.PLAYER_SPEED;
    }

    void Update()
    {
        if (isMoving)
        {
            PathArrMove();
        }
        transform.rotation = Quaternion.Euler(0, 0, 0);
    }
    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.gameObject.tag == "Player")
        {
            Pathfinding(currentNode);
            isMoving = true;
            GetComponent<AudioSource>().Play();
        }
        if (collision.gameObject.tag == "Pathfinding Node")
        {
            if (collision.gameObject.GetComponent<PathfindingNode>().roomInfo.isExit)
            {
                Destroy(gameObject);
            }
        }
    }
    void PathArrMove()
    {
        transform.right = pathArr[pathMoveIndex + 1] - transform.position;
        movedistance = Vector3.Distance(pathArr[pathMoveIndex + 1], transform.position);

        if (movedistance > 0.1f)
        {
            transform.position += moveSpeed * Time.deltaTime * transform.right;
            movedistance -= moveSpeed * Time.deltaTime;
        }
        else
        {
            pathMoveIndex++;
            if (pathMoveIndex + 1 >= pathArr.Length)
            {
                pathMoveIndex = 0;
            }
        }
    }
    void Pathfinding(MazeAlgorithm.Room pathfindingNode)
    {
        int recursionDept = pathIndexCount;
        pathIndexCount++;
        if (pathfindingNode == null)
        {
            pathIndexCount--;
            return;
        }

        if (pathfindingNode.isVisited == false)
        {
            pathIndexCount--;
            return;
        }

        pathfindingNode.isVisited = false;

        if (pathfindingNode.isExit)
        {
            pathfindingNode.isVisited = true;
            isPathfound = true;
            pathArr[recursionDept] = pathfindingNode.position;
            pathIndexCount--;
            return;
        }

        for (int i = 0; i < 4; i++)
        {
            switch (i)
            {
                case 0:
                    if (pathfindingNode.leftWall.isCarved)
                    {
                        Pathfinding(pathfindingNode.leftWall.front);
                    }
                    break;
                case 1:
                    if (pathfindingNode.rightWall.isCarved)
                    {
                        Pathfinding(pathfindingNode.rightWall.behind);
                    }
                    break;
                case 2:
                    if (pathfindingNode.downWall.isCarved)
                    {
                        Pathfinding(pathfindingNode.downWall.front);
                    }
                    break;
                case 3:
                    if (pathfindingNode.upWall.isCarved)
                    {
                        Pathfinding(pathfindingNode.upWall.behind);
                    }
                    break;
            }
            if (isPathfound)
            {
                pathfindingNode.isVisited = true;
                pathArr[recursionDept] = pathfindingNode.position;
                pathIndexCount--;
                return;
            }
        }
        pathfindingNode.isVisited = true;
        pathIndexCount--;
        return;
    }
}
