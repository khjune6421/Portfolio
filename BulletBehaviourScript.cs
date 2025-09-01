using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PirceBulletBehaviourScript : MonoBehaviour
{
    Rigidbody2D m_Rigidbody;

    void Start()
    {
        m_Rigidbody = GetComponent<Rigidbody2D>();
    }

    void Update()
    {
        Destroy(gameObject, 10.0f);
    }

    private void FixedUpdate()
    {
        m_Rigidbody.velocity = transform.right * PlayerControl.BULLET_SPEED * Time.fixedDeltaTime;
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.gameObject.tag == "Wall")
        {
            Destroy(gameObject);
        }
        if (collision.gameObject.tag == "Enemy")
        {
            Destroy(gameObject);
        }
    }
}