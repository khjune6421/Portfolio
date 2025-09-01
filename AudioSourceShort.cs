using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AudioSourceShort : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        GetComponent<AudioSource>().time = 0.5f;
        GetComponent<AudioSource>().Play();
        Destroy(gameObject, 0.77f);
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
