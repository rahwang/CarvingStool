using UnityEngine;
using System.Collections;

public class DodgeDetectMoon : MonoBehaviour {

	private DodgeGameManager game_manager;

	// Use this for initialization
	void Start () {
		game_manager = GameObject.Find ("HUD").GetComponent<DodgeGameManager> ();
	}

	// Update is called once per frame
	void Update () {

	}

	void OnCollisionEnter2D (Collision2D coll) {
		if(coll.collider.CompareTag("Ball")){
			Destroy (coll.gameObject);
			game_manager.Score (gameObject.name);
		}
	}
}
