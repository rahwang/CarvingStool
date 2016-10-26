using UnityEngine;
using System.Collections;

public class PlayerControllers : MonoBehaviour {

	public float speed = 1.0f;
	public GameObject player1;
	public GameObject player2;
	protected Rigidbody2D rb1;
	protected Rigidbody2D rb2;
	private bool soloGame = false;

	// Use this for initialization
	void Start () {
		if (PlayerPrefs.GetString ("moonshot_num_players") == "solo") {
			soloGame = true;
		}
		rb1 = player1.GetComponent<Rigidbody2D>();
		rb2 = player2.GetComponent<Rigidbody2D>();
	}
	
	// Update is called once per frame
	void Update () {
		Vector2 player1_direction = new Vector2 (0.0f, 0.0f);
		Vector2 player2_direction = new Vector2 (0.0f, 0.0f);

		if (Input.GetKey ("up"))
			player1_direction += (new Vector2(0.0f, 1.0f));
		if (Input.GetKey ("down"))
			player1_direction += (new Vector2(0.0f, -1.0f));
		if (Input.GetKey ("left"))
			player1_direction += (new Vector2(-1.0f, 0.0f));
		if (Input.GetKey ("right"))
			player1_direction += (new Vector2(1.0f, 0.0f));

		if (!soloGame) {
			if (Input.GetKey ("w"))
				player2_direction += (new Vector2 (0.0f, 1.0f));
			if (Input.GetKey ("s"))
				player2_direction += (new Vector2 (0.0f, -1.0f));
			if (Input.GetKey ("a"))
				player2_direction += (new Vector2 (-1.0f, 0.0f));
			if (Input.GetKey ("d"))
				player2_direction += (new Vector2 (1.0f, 0.0f));
		}

		rb1.velocity = player1_direction.normalized * speed;
		rb2.velocity = player2_direction.normalized * speed;
	}
}
