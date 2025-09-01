using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerCameraMovement : MonoBehaviour
{
    private GameObject playerGameObject = null;
    float cameraTime1 = 0.0f;
    float cameraTime2 = 0.0f;
    Vector3 targetPos;
    Vector3 playerPos;
    void Start()
    {
        playerGameObject = GameObject.Find("Player");
        playerPos = new Vector3(playerGameObject.transform.position.x, playerGameObject.transform.position.y, -10.0f);
        targetPos = playerPos;
    }

    void Update()
    {
        playerPos = new Vector3(playerGameObject.transform.position.x, playerGameObject.transform.position.y, -10.0f);
        if (Input.GetKey(KeyCode.Mouse1))
        {
            cameraTime1 += Time.deltaTime;
            Vector3 mousePos = Input.mousePosition;
            mousePos.z = 0.0f;

            Vector3 objectPos = Camera.main.WorldToScreenPoint(transform.position);
            mousePos.x -= objectPos.x;
            mousePos.y -= objectPos.y;
            targetPos = playerPos + (mousePos / Mathf.Pow(2, 8));
            transform.position = Vector3.Lerp(transform.position, targetPos, Mathf.Pow(cameraTime1, 1.5f) * 5.0f);
            cameraTime2 = 0.0f;
        }
        else
        {
            cameraTime2 += Time.deltaTime;
            transform.position = Vector3.Lerp(targetPos, playerPos, Mathf.Pow(cameraTime2, 1.5f) * 5.0f);
            cameraTime1 = 0.0f;
        }
    }
}
