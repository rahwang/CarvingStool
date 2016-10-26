using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class Menu : MonoBehaviour {

	static int NUM_BUTTONS = 3;
	// Text to display for num player options.
	static List<string> num_players = new List<string>(new string[] { "SOLO", "VERSUS" });
	// Text to display for play mode options.
	static List<string> modes = new List<string>( new string[] {"PONG", "DODGEBALL" });
	// Array of the option lists.
	List<string>[] button_options = new List<string>[] { num_players, modes };

	// Current button selected.
	private int curr_idx = 0;
	// The button frames to highlight when selected.
	public GameObject[] button_frames;
	// Array storing the current option selected for each button item.
	private int[] options = new int[NUM_BUTTONS];
	// Array storing the text to change.
	public GameObject[] button_text;

	public GUISkin layout;

	int Mod(int a, int b)
	{
		return (a % b + b) % b;
	}

	void SelectButton(int i) {
		button_frames [i].GetComponent<SpriteRenderer> ().color = Color.cyan; 
	}

	void UnSelectButton(int i) {
		button_frames [i].GetComponent<SpriteRenderer> ().color = Color.white;
	}

	void NextButton() {
		UnSelectButton (curr_idx);
		curr_idx = Mod(curr_idx + 1, NUM_BUTTONS);
		SelectButton (curr_idx);
	}

	void PrevButton() {
		UnSelectButton (curr_idx);
		curr_idx = Mod(curr_idx - 1, NUM_BUTTONS);
		SelectButton (curr_idx);
	}

	void NextOption() {
		if (curr_idx == NUM_BUTTONS - 1)
			return;
		options [curr_idx] = Mod (options [curr_idx] + 1, (button_options [curr_idx]).Count);
		button_text [curr_idx].GetComponent<TextMesh>().text = button_options [curr_idx] [options [curr_idx]];
	}

	void PrevOption() {
		if (curr_idx == NUM_BUTTONS - 1)
			return;
		options [curr_idx] = Mod (options [curr_idx] - 1, button_options [curr_idx].Count);
		button_text [curr_idx].GetComponent<TextMesh>().text = button_options [curr_idx] [options [curr_idx]];
	}


	// Use this for initialization
	void Start () {
		SelectButton (0);
		options [0] = 0;
		options [1] = 0;
		options [2] = 0;
		curr_idx = 0;


	}
	
	// Update is called once per frame
	void Update () {
		if (Input.GetKeyDown ("down")) {
			NextButton ();
		} else if (Input.GetKeyDown ("up")) {
			PrevButton ();
		} else if (Input.GetKeyDown ("right")) {
			NextOption ();
		} else if (Input.GetKeyDown ("left")) {
			PrevOption ();
		} else if (Input.GetKeyDown (KeyCode.Return)) {
			if (curr_idx == 2) {
				LoadGame ();
			}
		}
	}

	void LoadGame() {
		// Write num player options
		if (num_players [options [0]] == "SOLO") {
			PlayerPrefs.SetString ("moonshot_num_players", "solo");
		} else {
			PlayerPrefs.SetString ("moonshot_num_players", "versus");
		}

		// Load game
		if (modes [options [1]] == "PONG") {
			Application.LoadLevel ("pong");
		} else if (modes [options [1]] == "DODGEBALL") {
			Application.LoadLevel ("dodgeball");
		}
	}

	void OnGUI () {
		GUI.skin = layout		;
		GUI.Label(new Rect(Screen.width / 2 - 250, 150, 550, 200), "MOONSHOT");

		// Draw "number of players"
		//GUI.Button(new Rect(Screen.width / 2 - 110, 255, 220, 53), num_players[options[0]]);

		// Draw "game mode"
		//GUI.Button(new Rect(Screen.width / 2 - 110, 340, 220, 53), modes[options[1]]);

		// Draw "go"
		GUI.Box(new Rect(Screen.width / 2 - 105, 430, 220, 53), "");
	}
}
