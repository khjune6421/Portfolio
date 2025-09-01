using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using TMPro;
using Unity.Mathematics;

public class PlayerControl : MonoBehaviour
{
    [SerializeField] GameObject Bullet;
    [SerializeField] GameObject PirceBullet;
    [SerializeField] GameObject Blood;
    [SerializeField] GameObject GunshotSound;
    public GameObject HpSlider;

    public static float PLAYER_HP = 100.0f;
    public static float PLAYER_MAX_HP = 100.0f;
    public static int PLAYER_AMMO = 20;
    public static int MAX_AMMO_AMOUNT = 50;
    public static float PLAYER_SPEED = 2.0f;
    public static float FIRERATE = 0.35f;
    public static int PLAYER_MONEY = 0;
    public static float PLAYER_BULLET_DAMAGE = 30.0f;
    public static float BULLET_SPEED = 300.0f;
    public static float countDownTime = 0.0f;
    public static int PLAYER_KILL_COUNT = 0;
    public AudioClip[] arrAudio;
    bool isAudioPlaying = false;
    public TMP_Text ammoText;
    public TMP_Text coinText;
    public TMP_Text timer;
    public TMP_Text levelText;
    public TMP_Text playerKillCountText;

    float timeSinceLastShot = 0.0f;
    bool isAiming = false;
    bool isShooting = false;
    int isSpeedUp = 1;
    float speedUpTime = 0.0f;
    bool isInvincible = false;
    float InvincibleTime = 0.0f;
    bool isPiercingBullet = false;
    float piercingBulletTime = 0.0f;
    private Animator playerAnimator;

    void Start()
    {
        isAudioPlaying = false;
        playerAnimator = GetComponent<Animator>();
        countDownTime = (MazeAlgorithm.LEVEL - 3) * 4;
        AudioClip audio = arrAudio[0];
        GetComponent<AudioSource>().Stop();
        GetComponent<AudioSource>().PlayOneShot(audio, 0.15f);
        if (MazeAlgorithm.LEVEL == 8)
        {
            PLAYER_HP = 100.0f;
        }
    }

    void Update()
    {
        countDownTime -= Time.deltaTime;
        if (countDownTime <= 0)
        {
            PLAYER_HP -= Time.deltaTime * (MazeAlgorithm.LEVEL - 7);
            if (isAudioPlaying == false)
            {
                isAudioPlaying = true;
                AudioClip audio = arrAudio[1];
                GetComponent<AudioSource>().Stop();
                GetComponent<AudioSource>().PlayOneShot(audio, 0.2f);
            }
        }
        if (PLAYER_HP <= 0)
        {
            MazeAlgorithm.ROWS = 4;
            MazeAlgorithm.COLUMS = 4;
            MazeAlgorithm.LEVEL = 8;
            SceneManager.LoadScene("Title Scene");
        }
        RenderUI();
        RenderLine();
        InstantPower();
    }

    void FixedUpdate()
    {
        PlayerAimAndShoot();
        PlayerMovement();
    }

    void InstantPower()
    {
        if (Input.GetKeyDown(KeyCode.Alpha1))
        {
            if (PLAYER_KILL_COUNT >= 5)
            {
                PLAYER_KILL_COUNT -= 5;
                isSpeedUp = 2;
                speedUpTime = 5.0f;
            }
        }
        if (speedUpTime > 0.0f)
        {
            speedUpTime -= Time.deltaTime;
            if (speedUpTime <= 0.0f)
            {
                isSpeedUp = 1;
            }
        }
        if (Input.GetKeyDown(KeyCode.Alpha2))
        {
            if (PLAYER_KILL_COUNT >= 10)
            {
                PLAYER_KILL_COUNT -= 10;
                isInvincible = true;
                InvincibleTime = 5.0f;
            }
        }
        if (InvincibleTime > 0.0f)
        {
            InvincibleTime -= Time.deltaTime;
            if (InvincibleTime <= 0.0f)
            {
                isInvincible = false;
            }
        }
        if (Input.GetKeyDown(KeyCode.Alpha3))
        {
            if (PLAYER_KILL_COUNT >= 20)
            {
                PLAYER_KILL_COUNT -= 20;
                isPiercingBullet = true;
                piercingBulletTime = 10.0f;
            }
        }
        if (piercingBulletTime > 0.0f)
        {
            piercingBulletTime -= Time.deltaTime;
            if (piercingBulletTime <= 0.0f)
            {
                isPiercingBullet = false;
            }
        }
    }
    void RenderLine()
    {
        LineRenderer lineRenderer = GetComponent<LineRenderer>();
        if (isAiming)
        {
            lineRenderer.SetPosition(0, transform.position);
            if (isPiercingBullet)
            {
                RaycastHit2D hit = Physics2D.Raycast(transform.position, transform.right, math.INFINITY, LayerMask.GetMask("Enemy", "World Edge"));
                lineRenderer.SetPosition(1, new Vector3(hit.point.x, hit.point.y, 0));
                if (hit.collider.gameObject.tag == "Enemy")
                {
                    lineRenderer.startColor = Color.red;
                    lineRenderer.endColor = Color.red;
                }
                else
                {
                    lineRenderer.startColor = Color.green;
                    lineRenderer.endColor = Color.green;
                }
            }
            else
            {
                RaycastHit2D hit = Physics2D.Raycast(transform.position, transform.right, math.INFINITY, LayerMask.GetMask("Wall", "Enemy"));
                lineRenderer.SetPosition(1, new Vector3(hit.point.x, hit.point.y, 0));
                if (hit.collider.gameObject.tag == "Wall")
                {
                    lineRenderer.startColor = Color.green;
                    lineRenderer.endColor = Color.green;
                }
                else if (hit.collider.gameObject.tag == "Enemy")
                {
                    lineRenderer.startColor = Color.red;
                    lineRenderer.endColor = Color.red;
                }
            }
            lineRenderer.enabled = true;
            lineRenderer.startWidth = 0.01f;
            lineRenderer.endWidth = 0.01f;
        }
        else
        {
            lineRenderer.enabled = false;
        }
    }

    private void OnCollisionStay2D(Collision2D collision)
    {
        if (collision.gameObject.tag == "Enemy" && isInvincible == false)
        {
            PLAYER_HP -= (1.0f + ((float)(MazeAlgorithm.LEVEL - 7) / 14.0f)) / 10.0f;
        }
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.gameObject.tag == "Enemy Bullet" && isInvincible == false)
        {
            PLAYER_HP -= (10.0f + ((MazeAlgorithm.LEVEL - 7) / 1.4f)) / 10.0f;
            Vector3 hitLocation = collision.gameObject.transform.position;
            GameObject blood = Instantiate(Blood, hitLocation, Quaternion.identity);
        }
    }

    private void PlayerMovement()
    {
        float moveHorizontal = (Input.GetAxisRaw("Horizontal") + Input.GetAxis("Horizontal")) / 2;
        float moveVertical = (Input.GetAxisRaw("Vertical") + Input.GetAxis("Vertical")) / 2;
        Vector3 movement = new Vector3(moveHorizontal, moveVertical, 0.0f);
        if (movement.magnitude > 1.0f)
        {
            movement.Normalize();
        }
        if (isAiming || isShooting)
        {
            transform.position += movement * ((PLAYER_SPEED * isSpeedUp) / 2) * Time.deltaTime;
            playerAnimator.Play("Player Aim Animation");
        }
        else
        {
            transform.position += movement * (PLAYER_SPEED * isSpeedUp) * Time.deltaTime;
            if (movement.magnitude > 0.0f)
            {
                transform.rotation = Quaternion.Euler(0, 0, Mathf.Atan2(movement.y, movement.x) * Mathf.Rad2Deg);
                playerAnimator.Play("Player Walk Animation");
            }
            else
            {
                playerAnimator.Play("Player Idle Animation");
            }
        }
    }

    private void PlayerAimAndShoot()
    {
        Vector3 mousePos = Input.mousePosition;
        mousePos.z = 0.0f;

        Vector3 objectPos = Camera.main.WorldToScreenPoint(transform.position);
        mousePos.x -= objectPos.x;
        mousePos.y -= objectPos.y;

        float angle = Mathf.Atan2(mousePos.y, mousePos.x) * Mathf.Rad2Deg;
        if (Input.GetKey(KeyCode.Mouse0) && PLAYER_AMMO > 0)
        {
            isShooting = true;
            timeSinceLastShot += Time.deltaTime;
            transform.rotation = Quaternion.Euler(new Vector3(0, 0, angle));
            if (timeSinceLastShot > FIRERATE)
            {
                if (isPiercingBullet)
                {
                    GameObject bullet = Instantiate(PirceBullet, transform.position, transform.rotation);
                }
                else
                {
                    GameObject bullet = Instantiate(Bullet, transform.position, transform.rotation);
                }
                GameObject gunshotSound = Instantiate(GunshotSound, transform.position, transform.rotation);
                PLAYER_AMMO--;
                timeSinceLastShot = 0.0f;
            }
        }
        else
        {
            isShooting = false;
            timeSinceLastShot = 0.0f;
        }
        if (Input.GetKey(KeyCode.Mouse1))
        {
            isAiming = true;
            timeSinceLastShot += Time.deltaTime / 4;
            transform.rotation = Quaternion.Euler(new Vector3(0, 0, angle));
        }
        else
        {
            isAiming = false;
        }
    }

    private void RenderUI()
    {
        var slider = HpSlider.GetComponent<UnityEngine.UI.Slider>();
        slider.value = PLAYER_HP / PLAYER_MAX_HP;
        ammoText.text ="AMMO: " + PLAYER_AMMO.ToString();
        coinText.text = "COIN: " + PLAYER_MONEY.ToString();
        timer.text = "TIME: " + Mathf.Round(countDownTime).ToString();
        levelText.text = "LEVEL: " + (MazeAlgorithm.LEVEL - 7).ToString();
        playerKillCountText.text = "KILL: " + PLAYER_KILL_COUNT.ToString();
    }
}
