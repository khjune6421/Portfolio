using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class EnemyPathfinding : MonoBehaviour
{
    [SerializeField] GameObject enemyBullet;
    [SerializeField] GameObject Blood;
    [SerializeField] GameObject GunshotSound;
    [SerializeField] GameObject DeathSound1;
    [SerializeField] GameObject DeathSound2;
    MazeAlgorithm.Room currentNode = null;
    Vector3[] pathArr = new Vector3[MazeAlgorithm.COLUMS * MazeAlgorithm.ROWS];


    bool isPathfound = false;
    bool isMoving = false;
    bool deathSound = false;
    int pathIndexCount = 0;
    int pathMoveIndex = 0;
    float timer = 0.0f;
    float randomTime = 0.0f;
    float movedistance = 0.0f;
    public float enemyHp = 0.0f;
    private RaycastHit hit;
    float timeSinceLastShot = 0.0f;
    private Animator enemyrAnimator;
    float ENEMY_MOVE_SPEED = 1.0f;

    void Start()
    {
        enemyrAnimator = GetComponent<Animator>();
        randomTime = Random.Range(1.0f, 3.0f);
        if (randomTime < 2.0f)
        {
            deathSound = true;
        }
        enemyHp = 100.0f + ((MazeAlgorithm.LEVEL - 7) * 10);
        ENEMY_MOVE_SPEED = 1.0f + ((float)(MazeAlgorithm.LEVEL - 7) / 10.0f);
    }

    void Update()
    {
        if (currentNode != null && currentNode.isPlayerOn)
        {
            isMoving = false;
        }
        else
        {
            PathfindTimeRandomiser();
        }
        if (IsPlayerOnSight())
        {
            enemyrAnimator.Play("Player Aim Animation");
            if ((timeSinceLastShot += Time.deltaTime) > 0.5f)
            {
                GameObject bullet = Instantiate(enemyBullet, transform.position, transform.rotation);
                GameObject gunshotSound = Instantiate(GunshotSound, transform.position, transform.rotation);
                timeSinceLastShot = 0.0f;
            }
        }
        if (isMoving)
        {
            enemyrAnimator.Play("Player Walk Animation");
            PathArrMove();
        }
    }
    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.gameObject.tag == "Pathfinding Node")
        {
            currentNode = collision.gameObject.GetComponent<PathfindingNode>().roomInfo;
        }
        if (collision.gameObject.tag == "Bullet")
        {
            BulletBehaviourScript bullet = collision.gameObject.GetComponent<BulletBehaviourScript>();
            enemyHp -= PlayerControl.PLAYER_BULLET_DAMAGE;
            Vector3 hitLocation = collision.gameObject.transform.position;
            GameObject blood = Instantiate(Blood, hitLocation, Quaternion.identity);
            if (enemyHp <= 0)
            {
                if (deathSound)
                {
                    GameObject sound1 = Instantiate(DeathSound1, transform.position, transform.rotation);
                }
                else
                {
                    GameObject sound2 = Instantiate(DeathSound2, transform.position, transform.rotation);
                }
                PlayerControl.PLAYER_KILL_COUNT++;
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
            transform.position += ENEMY_MOVE_SPEED * Time.deltaTime * transform.right;
            movedistance -= ENEMY_MOVE_SPEED * Time.deltaTime;
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

    void PathfindTimeRandomiser()
    {
        timer += Time.deltaTime;
        if (timer > randomTime)
        {
            timer -= randomTime;
            Pathfinding(currentNode);
            isPathfound = false;
            pathIndexCount = 0;
            pathMoveIndex = 0;
            isMoving = true;
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

        if (pathfindingNode.isPlayerOn)
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

    bool IsPlayerOnSight()
    {
        Vector3 playerPos = GameObject.FindGameObjectWithTag("Player").transform.position;
        if (Vector3.Distance(playerPos, transform.position) < 50.0f)
        {
            transform.right = playerPos - transform.position;
            RaycastHit2D hit = Physics2D.Raycast(transform.position, transform.right, 50.0f, LayerMask.GetMask("Default", "Wall"));
            if (hit.collider != null)
            {
                if (hit.collider.gameObject.tag == "Player")
                {
                    return true;
                }
                else if (hit.collider.gameObject.tag == "Wall")
                {
                    return false;
                }
            }
        }
        return false;
    }
}
