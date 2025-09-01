using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ItemHeal : MonoBehaviour
{
    [SerializeField] GameObject itemHealSound;
    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.gameObject.tag == "Player")
        {
            PlayerControl.PLAYER_HP += PlayerControl.PLAYER_MAX_HP / 10.0f;
            if (PlayerControl.PLAYER_HP > PlayerControl.PLAYER_MAX_HP)
            {
                PlayerControl.PLAYER_HP = PlayerControl.PLAYER_MAX_HP;
            }
            GameObject healSound = Instantiate(itemHealSound, transform.position, transform.rotation);
            Destroy(gameObject);
        }
    }
}
