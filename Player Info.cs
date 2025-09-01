using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using TMPro;

public class PlayerInfo : MonoBehaviour
{
    public static int MAX_HP_UPGRADE_PRICE = 1;
    public static int SPEED_UPGRADE_PRICE = 1;
    public static int MAX_AMMO_UPGRADE_PRICE = 1;
    public static int FIRERATE_UPGRADE_PRICE = 1;
    public static int BULLET_DAMAGE_UPGRADE_PRICE = 1;
    public static int BULLET_SPEED_UPGRADE_PRICE = 1;
    public AudioClip[] arrAudio;

    public GameObject HpSlider;
    public TMP_Text ammoText;
    public TMP_Text coinText;
    public TMP_Text maxHpText;
    public TMP_Text playerSpeedText;
    public TMP_Text playerMaxAmmoText;
    public TMP_Text fireRateText;
    public TMP_Text playerBulletDamageText;
    public TMP_Text bulletSpeedText;
    public TMP_Text maxHpUpgradePriceText;
    public TMP_Text speedUpgradePriceText;
    public TMP_Text maxAmmoUpgradePriceText;
    public TMP_Text fireRateUpgradePriceText;
    public TMP_Text bulletDamageUpgradePriceText;
    public TMP_Text bulletSpeedUpgradePriceText;
    public TMP_Text levelText;

    void Update()
    {
        RenderUI();
    }
    private void RenderUI()
    {
        var slider = HpSlider.GetComponent<UnityEngine.UI.Slider>();
        slider.value = PlayerControl.PLAYER_HP / PlayerControl.PLAYER_MAX_HP;
        ammoText.text = "AMMO: " + PlayerControl.PLAYER_AMMO.ToString();
        coinText.text = "COIN: " + PlayerControl.PLAYER_MONEY.ToString();
        maxHpText.text = "MAX HP: " + PlayerControl.PLAYER_MAX_HP.ToString();
        playerSpeedText.text = "SPEED: " + PlayerControl.PLAYER_SPEED.ToString();
        playerMaxAmmoText.text = "MAX AMMO: " + PlayerControl.MAX_AMMO_AMOUNT.ToString();
        fireRateText.text = "FIRERATE: " + PlayerControl.FIRERATE.ToString();
        playerBulletDamageText.text = "DAMAGE: " + PlayerControl.PLAYER_BULLET_DAMAGE.ToString();
        bulletSpeedText.text = "BULLET SPEED: " + PlayerControl.BULLET_SPEED.ToString();
        maxHpUpgradePriceText.text = "MAX HP UPGRADE PRICE: " + MAX_HP_UPGRADE_PRICE.ToString();
        speedUpgradePriceText.text = "SPEED UPGRADE PRICE: " + SPEED_UPGRADE_PRICE.ToString();
        maxAmmoUpgradePriceText.text = "MAX AMMO UPGRADE PRICE: " + MAX_AMMO_UPGRADE_PRICE.ToString();
        fireRateUpgradePriceText.text = "FIRERATE UPGRADE PRICE: " + FIRERATE_UPGRADE_PRICE.ToString();
        bulletDamageUpgradePriceText.text = "BULLET DAMAGE UPGRADE PRICE: " + BULLET_DAMAGE_UPGRADE_PRICE.ToString();
        bulletSpeedUpgradePriceText.text = "BULLET SPEED UPGRADE PRICE: " + BULLET_SPEED_UPGRADE_PRICE.ToString();
        levelText.text = "LEVEL: " + (MazeAlgorithm.LEVEL - 7).ToString();
    }

    public void MaxHpUpgrade()
    {
        if (PlayerControl.PLAYER_MONEY >= MAX_HP_UPGRADE_PRICE)
        {
            PlayerControl.PLAYER_MAX_HP += 20.0f;
            PlayerControl.PLAYER_MONEY -= MAX_HP_UPGRADE_PRICE;
            MAX_HP_UPGRADE_PRICE += 1;
            GetComponent<AudioSource>().PlayOneShot(arrAudio[0], 0.3f);
        }
        else
        {
            GetComponent<AudioSource>().PlayOneShot(arrAudio[1]);
        }
    }
    public void SpeedUpgrade()
    {
        if (PlayerControl.PLAYER_MONEY >= SPEED_UPGRADE_PRICE)
        {
            PlayerControl.PLAYER_SPEED += 0.5f / SPEED_UPGRADE_PRICE;
            PlayerControl.PLAYER_MONEY -= SPEED_UPGRADE_PRICE;
            SPEED_UPGRADE_PRICE += 1;
            GetComponent<AudioSource>().PlayOneShot(arrAudio[0], 0.3f);
        }
        else
        {
            GetComponent<AudioSource>().PlayOneShot(arrAudio[1]);
        }
    }
    public void MaxAmmoUpgrade()
    {
        if (PlayerControl.PLAYER_MONEY >= MAX_AMMO_UPGRADE_PRICE)
        {
            PlayerControl.MAX_AMMO_AMOUNT += 10;
            PlayerControl.PLAYER_MONEY -= MAX_AMMO_UPGRADE_PRICE;
            MAX_AMMO_UPGRADE_PRICE += 1;
            GetComponent<AudioSource>().PlayOneShot(arrAudio[0], 0.3f);
        }
        else
        {
            GetComponent<AudioSource>().PlayOneShot(arrAudio[1]);
        }
    }
    public void FireRateUpgrade()
    {
        if (PlayerControl.PLAYER_MONEY >= FIRERATE_UPGRADE_PRICE)
        {
            PlayerControl.FIRERATE -= 0.08f / FIRERATE_UPGRADE_PRICE;
            PlayerControl.PLAYER_MONEY -= FIRERATE_UPGRADE_PRICE;
            FIRERATE_UPGRADE_PRICE += 1;
            GetComponent<AudioSource>().PlayOneShot(arrAudio[0], 0.3f);
        }
        else
        {
            GetComponent<AudioSource>().PlayOneShot(arrAudio[1]);
        }
    }
    public void DamageUpgrade()
    {
        if (PlayerControl.PLAYER_MONEY >= BULLET_DAMAGE_UPGRADE_PRICE)
        {
            PlayerControl.PLAYER_BULLET_DAMAGE += 5.0f;
            PlayerControl.PLAYER_MONEY -= BULLET_DAMAGE_UPGRADE_PRICE;
            BULLET_DAMAGE_UPGRADE_PRICE += 1;
            GetComponent<AudioSource>().PlayOneShot(arrAudio[0], 0.3f);
        }
        else
        {
            GetComponent<AudioSource>().PlayOneShot(arrAudio[1]);
        }
    }
    public void BulletSpeedUpgrade()
    {
        if (PlayerControl.PLAYER_MONEY >= BULLET_SPEED_UPGRADE_PRICE)
        {
            PlayerControl.BULLET_SPEED += 100.0f / BULLET_SPEED_UPGRADE_PRICE;
            PlayerControl.PLAYER_MONEY -= BULLET_SPEED_UPGRADE_PRICE;
            BULLET_SPEED_UPGRADE_PRICE += 1;
            GetComponent<AudioSource>().PlayOneShot(arrAudio[0], 0.3f);
        }
        else
        {
            GetComponent<AudioSource>().PlayOneShot(arrAudio[1]);
        }
    }
    public void BuyHp()
    {
        if (PlayerControl.PLAYER_MONEY >= 3)
        {
            PlayerControl.PLAYER_HP += 10.0f;
            PlayerControl.PLAYER_MONEY -= 3;
            GetComponent<AudioSource>().PlayOneShot(arrAudio[0], 0.3f);
        }
        else
        {
            GetComponent<AudioSource>().PlayOneShot(arrAudio[1]);
        }
    }
    public void BuyAmmo()
    {
        if (PlayerControl.PLAYER_MONEY >= 1)
        {
            PlayerControl.PLAYER_AMMO += 10;
            PlayerControl.PLAYER_MONEY -= 1;
            GetComponent<AudioSource>().PlayOneShot(arrAudio[0], 0.3f);
        }
        else
        {
            GetComponent<AudioSource>().PlayOneShot(arrAudio[1]);
        }
    }
    public void StartMazeGame()
    {
        SceneManager.LoadScene("Play Maze Scene");
    }
}
