using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class TitleStartGame : MonoBehaviour
{
    void Start()
    {
        MazeAlgorithm.ROWS = 4;
        MazeAlgorithm.COLUMS = 4;
        MazeAlgorithm.LEVEL = 8;
        PlayerControl.PLAYER_HP = 100.0f;
        PlayerControl.PLAYER_MAX_HP = 100.0f;
        PlayerControl.PLAYER_AMMO = 20;
        PlayerControl.PLAYER_SPEED = 2.0f;
        PlayerControl.FIRERATE = 0.35f;
        PlayerControl.PLAYER_MONEY = 0;
        PlayerControl.PLAYER_BULLET_DAMAGE = 30.0f;
        PlayerControl.BULLET_SPEED = 300.0f;
        PlayerControl.MAX_AMMO_AMOUNT = 50;
        PlayerInfo.MAX_HP_UPGRADE_PRICE = 1;
        PlayerInfo.SPEED_UPGRADE_PRICE = 1;
        PlayerInfo.MAX_AMMO_UPGRADE_PRICE = 1;
        PlayerInfo.FIRERATE_UPGRADE_PRICE = 1;
        PlayerInfo.BULLET_DAMAGE_UPGRADE_PRICE = 1;
        PlayerInfo.BULLET_SPEED_UPGRADE_PRICE = 1;
        PlayerControl.PLAYER_KILL_COUNT = 0;
    }
    public void StartGame()
    {
        SceneManager.LoadScene("Play Maze Scene");
    }
}
