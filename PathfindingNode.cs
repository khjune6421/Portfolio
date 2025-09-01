using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class PathfindingNode : MonoBehaviour
{
    public MazeAlgorithm.Room roomInfo = null;
    [SerializeField] GameObject enemy;
    [SerializeField] GameObject itemMap;
    [SerializeField] GameObject itemAmmo;
    [SerializeField] GameObject itemHeal;
    [SerializeField] GameObject itemCoin;

    void Start()
    {
        if (roomInfo.isExit)
        {
            gameObject.GetComponent<SpriteRenderer>().sprite = Resources.Load<Sprite>("Sprites/terrain");
        }
        else if (roomInfo.position != new Vector3(0.0f, 0.0f, 0.0f))
        {
            int randomSpawn = Random.Range(0, 100);
            if (randomSpawn < 7)
            {
                GameObject newEnemy = Instantiate(enemy, transform.position, transform.rotation);
            }
            else if (randomSpawn < 13)
            {
                GameObject newMap = Instantiate(itemMap, transform.position, transform.rotation);
                ItemMap setMap = newMap.GetComponent<ItemMap>();
                if (setMap != null)
                {
                    setMap.currentNode = roomInfo;
                }
            }
            else if (randomSpawn < 20)
            {
                GameObject newAmmo = Instantiate(itemAmmo, transform.position, transform.rotation);
            }
            else if (randomSpawn < 25)
            {
                GameObject newHeal = Instantiate(itemHeal, transform.position, transform.rotation);
            }
            else if (randomSpawn < 35)
            {
                GameObject newCoin = Instantiate(itemCoin, transform.position, transform.rotation);
            }
        }
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.gameObject.tag == "Player")
        {
            roomInfo.isPlayerOn = true;
            if (roomInfo.isExit)
            {
                if (MazeAlgorithm.LEVEL <= 64)
                {
                    MazeAlgorithm.LEVEL++;
                }
                MazeAlgorithm.ROWS = Random.Range(MazeAlgorithm.LEVEL / 2, MazeAlgorithm.LEVEL);
                MazeAlgorithm.COLUMS = Random.Range(MazeAlgorithm.LEVEL / 2, MazeAlgorithm.LEVEL);
                if (PlayerControl.countDownTime >= 0.0f)
                {
                    PlayerControl.PLAYER_MONEY += (int)PlayerControl.countDownTime;
                }
                SceneManager.LoadScene("Shop Scene");
            }
        }
    }
    private void OnTriggerExit2D(Collider2D collision)
    {
        if (collision.gameObject.tag == "Player")
        {
            roomInfo.isPlayerOn = false;
        }
    }
}
