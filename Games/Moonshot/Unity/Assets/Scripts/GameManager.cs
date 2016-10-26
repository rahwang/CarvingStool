using UnityEngine;
using System.Collections;

public class GameManager : MonoBehaviour {

	public static int player1_score;
	public static int player2_score;
	public static GameObject moon;

	public GUISkin layout;
	public GameObject moon_prefab; 
	private static GameObject planet1;
	private static GameObject planet2;
	private AudioSource explosionfx;
	private bool isSoloGame = false;

	// Use this for initialization
	void Start () {
		explosionfx = gameObject.GetComponent<AudioSource> ();
		planet1 = GameObject.Find ("Planet1");
		planet2 = GameObject.Find ("Planet2");

		// Check if it's a solo game. If so, disable player 1.
		if (PlayerPrefs.GetString ("moonshot_num_players") == "solo") {
			planet1.GetComponent<PlanetAI> ().enabled = true;
			isSoloGame = true;
		}

		moon = (GameObject) Instantiate(moon_prefab, new Vector2(0, 0), Quaternion.identity);
		player1_score = 0;
		player2_score = 0;
	}
	
	// Update is called once per frame
	void Update () {
	
	}

	public Vector2 GetMoonPos() {
		if (moon) {
			return moon.transform.position;
		} else {
			return Vector2.zero;
		}
	}

	public Vector2 GetMoonVel() {
		if (moon) {
			return moon.GetComponent<Rigidbody2D> ().velocity;
		} else {
			return Vector2.zero;
		}
	}

	public void Score (string goal_name) {
		if (goal_name == "Planet1" || goal_name == "Planet2") {
			explosionfx.Play ();
		}

		if (goal_name == "Goal1" || goal_name == "Planet1") {
			player2_score++;
		} else if (goal_name == "Goal2" || goal_name == "Planet2") {
			player1_score++;
		}

		if (player1_score >= 10 || player2_score >= 10) {
			GameObject.Destroy (moon);
			Invoke ("LoadMenu", 3.0f);
		} else {
			GameObject.Destroy (moon);
			Invoke ("Reset", 2.0f);
		}
	}

	void LoadMenu() {
		Application.LoadLevel ("menu");
	}

	void Reset () {
		moon = (GameObject) Instantiate(moon_prefab, new Vector2(0, 0), Quaternion.identity);
		planet1.transform.position = new Vector2 (-4.75f, 0.0f);
		planet2.transform.position = new Vector2 (4.75f, 0.0f);
	}

	void OnGUI () {
		GUI.skin = layout;
		GUI.Label(new Rect(Screen.width / 2 - 150 - 12, 20, 100, 100), "" + player1_score);
		GUI.Label(new Rect(Screen.width / 2 + 150 + 12, 20, 100, 100), "" + player2_score);

		if (player1_score >= 10) {
			if (isSoloGame) {
				GUI.Label (new Rect (Screen.width / 2 - 170, Screen.height / 2 - 20, 2000, 1000), "THE COMPUTER WINS");
			} else {
				GUI.Label (new Rect (Screen.width / 2 - 170, Screen.height / 2 - 20, 2000, 1000), "PLAYER ONE WINS");
			}
		} else if (player2_score >= 10) {
			if (isSoloGame) {
				GUI.Label (new Rect (Screen.width / 2 - 170, Screen.height / 2 - 20, 2000, 1000), "THE HUMAN WINS");
			} else {
				GUI.Label (new Rect (Screen.width / 2 - 170, Screen.height / 2 - 20, 2000, 1000), "PLAYER TWO WINS");
			}
		}
	}
}
