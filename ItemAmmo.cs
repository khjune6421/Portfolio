using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ItemAmmo : MonoBehaviour
{
    [SerializeField] GameObject itemAmmoSound;
    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.gameObject.tag == "Player")
        {
            PlayerControl.PLAYER_AMMO += PlayerControl.MAX_AMMO_AMOUNT / 5;
            if (PlayerControl.PLAYER_AMMO > PlayerControl.MAX_AMMO_AMOUNT)
            {
                PlayerControl.PLAYER_AMMO = PlayerControl.MAX_AMMO_AMOUNT;
            }
            GameObject ammoSound = Instantiate(itemAmmoSound, transform.position, transform.rotation);
            Destroy(gameObject);
        }
    }
}
