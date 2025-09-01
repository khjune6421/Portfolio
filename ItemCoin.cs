using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ItemCoin : MonoBehaviour
{
    [SerializeField] GameObject itemCoinSound;
    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.gameObject.tag == "Player")
        {
            PlayerControl.PLAYER_MONEY += 5;
            GameObject coinSound = Instantiate(itemCoinSound, transform.position, transform.rotation);
            Destroy(gameObject);
        }
    }
}
