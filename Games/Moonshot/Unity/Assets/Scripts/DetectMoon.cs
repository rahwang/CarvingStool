using UnityEngine;
using System.Collections;

public class DetectMoon : MonoBehaviour {

	private GameManager game_manager;

	// Use this for initialization
	void Start () {
		game_manager = GameObject.Find ("HUD").GetComponent<GameManager> ();
	}
	
	// Update is called once per frame
	void Update () {
	
	}

	void OnCollisionEnter2D (Collision2D coll) {
		if(coll.collider.CompareTag("Ball")){
			Debug.Log ("score-hit");
			ExplosionSpawn.Instance.Explosion (transform.position, transform);
			game_manager.Score (gameObject.name);
		}
	}
}
