using UnityEngine;
using System.Collections;

public class PlanetAI : MonoBehaviour {

	private GameManager game_manger;
	private Vector2 goal_pos;
	private float speed;
	public float force_scale = 5.0f;
	private Rigidbody2D rb;

	public float sample_rate = 0.1f;

	// Use this for initialization
	void Start () {
		rb = GetComponent<Rigidbody2D>();
		game_manger = GameObject.Find ("HUD").GetComponent<GameManager> ();
		speed = GameObject.Find ("HUD").GetComponent<PlayerControllers> ().speed;
		goal_pos = GameObject.Find ("Goal2").transform.position;
		//InvokeRepeating ("Sample", 0.0f, 0.5f); 
	}

	void Sample() {
		Vector2 moon_pos = game_manger.GetMoonPos ();
		Vector2 moon_vel = game_manger.GetMoonVel ();
		Vector2 pos = gameObject.transform.position;
		Vector2 vel_dir, tmp;

		// Move towards moon.
		/*
		if (Vector2.Distance (moon_pos, gameObject.transform.position) > 1f) {
			Vector2 direction_to_goal = (goal_pos - moon_pos).normalized;
			if (Vector2.Dot (moon_vel.normalized, direction_to_goal) > 0.8) {
				rb.velocity = (goal_pos - pos).normalized * speed;
			} else {
				rb.velocity = (moon_pos - pos).normalized * speed;
			}
		} else {
			// If the moon is too close, get out of the way.
			Vector2 direction_to_us = (pos - moon_pos).normalized;
			rb.velocity = (new Vector2 (moon_vel.y, moon_vel.x)).normalized * speed * 0.5f;
			rb.velocity += (pos - moon_pos).normalized * speed * 0.5f;
		}*/
			
		/*
		if (Vector2.Distance (moon_pos, gameObject.transform.position) > 1f) {
			Vector2 direction_to_goal = (goal_pos - moon_pos).normalized;
			if (Vector2.Dot (moon_vel.normalized, direction_to_goal) > 0.8) {
				vel_dir = (goal_pos - pos).normalized;
			} else {
				vel_dir = (moon_pos - pos).normalized;
			}
		} else {
			// If the moon is too close, get out of the way.
			Vector2 direction_to_us = (pos - moon_pos).normalized;
			vel_dir = (new Vector2 (moon_vel.y, moon_vel.x)).normalized * 0.5f;
			vel_dir += (pos - moon_pos).normalized * 0.5f;
		}

		Vector2 target = pos + vel_dir * speed;
		StartCoroutine(moveObject(pos, target, sample_rate)); */
	}


	IEnumerator moveObject(Vector2 source, Vector2 target, float duration)
	{
		Debug.Log ("trying things");
		float elapsedTime = 0.0f;
		while(elapsedTime < duration)
		{
			gameObject.transform.position = Vector2.Lerp(source, target, elapsedTime / duration);
			elapsedTime += Time.deltaTime;
			yield return null;
		}
		gameObject.transform.position = target;
	}
	
	// Update is called once per frame
	void FixedUpdate () {
		Vector2 moon_pos = game_manger.GetMoonPos ();
		Vector2 moon_vel = game_manger.GetMoonVel ();
		Vector2 pos = gameObject.transform.position;
		Vector2 vel_dir, tmp;

		float dist = Vector2.Distance (moon_pos, gameObject.transform.position);
		Vector2 direction_to_goal = (goal_pos - moon_pos).normalized;
		if (dist > 1.0f) {
			if (Vector2.Dot (moon_vel.normalized, direction_to_goal) > 0.8) {
				tmp = (goal_pos - pos).normalized;
			} else {
				tmp = (moon_pos - pos).normalized * 0.75f;
				tmp += (goal_pos - pos).normalized * 0.25f;
			}
		} else if (dist < 0.95) {
			// If the moon is too close, get out of the way.
			Vector2 direction_to_us = (pos - moon_pos).normalized;
			tmp = (new Vector2 (moon_vel.y, moon_vel.x)).normalized * 0.6f;
			tmp += (pos - moon_pos).normalized * 0.4f;
		} else {
			tmp = Vector2.zero;
		}

		rb.AddForce (tmp * force_scale);

		// Clamp velocity
		if (Vector3.Magnitude(rb.velocity) > speed) {
			rb.velocity = rb.velocity.normalized * speed;
		}
	}
}
