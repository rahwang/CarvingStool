using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class DodgeGameManager : MonoBehaviour {

	public static int player1_score;
	public static int player2_score;
	public List<GameObject> moons;
	public int num_moons = 5;
	public float max_speed = 20.0f;

	public GUISkin layout;
	public GameObject moon_prefab; 
	private static GameObject planet1;
	private static GameObject planet2;
	public static float timer = 0.0f;
	private AudioSource explosionfx;
	private bool isSoloGame = false;
	private bool timerGo = true;

	// Use this for initialization
	void Start () {
		timer = 0.0f;
		explosionfx = gameObject.GetComponent<AudioSource> ();
		planet1 = GameObject.Find ("Planet1");
		planet2 = GameObject.Find ("Planet2");

		// Check if it's a solo game. If so, disable player 1.
		if (PlayerPrefs.GetString ("moonshot_num_players") == "solo") {
			planet1.SetActive (false);
			isSoloGame = true;
		}
		for (int i = 0; i < num_moons; i++) {
			GameObject new_moon = (GameObject)Instantiate (moon_prefab, new Vector2 (0, 0), Quaternion.identity);
			new_moon.GetComponent<DodgeMoonControl>().max_speed = max_speed;
			moons.Add (new_moon);
		}
		player1_score = 0;
		player2_score = 0;
	}

	// Update is called once per frame
	void Update () {
		if (timerGo)
			timer += Time.deltaTime;
	}

	public void Score (string goal_name) {
		if (goal_name == "Planet1" || goal_name == "Planet2") {
			explosionfx.Play ();
			if (isSoloGame) {
				EndGame ();
				return;
			}
		}

		if (goal_name == "Goal1" || goal_name == "Planet1") {
			player2_score++;
		} else if (goal_name == "Goal2" || goal_name == "Planet2") {
			player1_score++;
		}

		if (player1_score == 10 || player2_score == 10) {
			EndGame ();
			return;
		} else {
			Reset ();
		}
	}

	void EndGame() {
		timerGo = false;
		foreach (GameObject moon in moons) {
			GameObject.Destroy (moon);
		}
		Invoke ("LoadMenu", 3.0f);
	}

	void LoadMenu() {
		Application.LoadLevel ("menu");
	}

	void Reset () {
		GameObject new_moon = (GameObject)Instantiate (moon_prefab, new Vector2 (0, 0), Quaternion.identity);
		new_moon.GetComponent<DodgeMoonControl>().max_speed = max_speed;
		moons.Add (new_moon);
	}

	void OnGUI () {
		GUI.skin = layout;
		if (isSoloGame) {
			string minutes = Mathf.Floor(timer / 60).ToString("00");
			string seconds = Mathf.Floor(timer % 60).ToString("00");
			GUI.Label (new Rect (Screen.width / 2 - 40, 20, 150, 100), minutes + ":" + seconds);
			if (!timerGo) {
				GUI.Label (new Rect (Screen.width / 2 - 110, Screen.height / 2 - 20, 2000, 1000), "GAME OVER");
			}
		} else {
			GUI.Label (new Rect (Screen.width / 2 - 150 - 12, 20, 100, 100), "" + player1_score);
			GUI.Label (new Rect (Screen.width / 2 + 150 + 12, 20, 100, 100), "" + player2_score);

			if (player1_score >= 10) {
				GUI.Label (new Rect (Screen.width / 2 - 170, Screen.height / 2 - 20, 2000, 1000), "PLAYER ONE WINS");
			} else if (player2_score >= 10) {
				GUI.Label (new Rect (Screen.width / 2 - 170, Screen.height / 2 - 20, 2000, 1000), "PLAYER TWO WINS");
			}
		}
	}
}
