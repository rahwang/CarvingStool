using UnityEngine;
using System.Collections;

public class MoonControl : MonoBehaviour {

	private GameObject planet1;
	private GameObject planet2;
	private Rigidbody2D rb;
	private Vector2 vel;

	public float max_speed = 5.0f;
	public float initial_speed = 5.0f;
	public float force_scale = 1.0f;
	private AudioSource ping_effect;

	// Use this for initialization
	void Start () {
		planet1 = GameObject.Find ("Planet1");
		planet2 = GameObject.Find ("Planet2");

		rb = GetComponent<Rigidbody2D>();
		ping_effect = GameObject.Find("ping").GetComponent<AudioSource> ();
		Spawn ();
	}

	// Update is called once per frame
	void Update () {

		float dist_planet2 = Vector2.Distance (planet2.transform.position, transform.position);
		Vector2 direction_planet2 = (planet2.transform.position - transform.position).normalized;
		Vector2 planet2_gravity = (rb.mass * direction_planet2 * force_scale) / dist_planet2;
		//rb.velocity += planet2_gravity;
		rb.AddForce (planet2_gravity);

		float dist_planet1 = Vector2.Distance(planet1.transform.position, transform.position);
		Vector2 direction_planet1 = (planet1.transform.position - transform.position).normalized;
		Vector2 planet1_gravity = (rb.mass * direction_planet1 * force_scale) / dist_planet1;
		rb.AddForce(planet1_gravity);

		// Clamp velocity
		if (Vector3.Magnitude(rb.velocity) > max_speed) {
			rb.velocity = rb.velocity.normalized * max_speed;
		}
	}

	public void Spawn () {
		float x = Random.Range(0.0f, 2.0f) - 1.0f;
		float y = Random.Range(0.0f, 2.0f) - 1.0f;
		Vector2 init_dir = new Vector2 (x, y);
		init_dir.Normalize ();

		rb.AddForce(init_dir * initial_speed);
	}

	public void Reset () {
		gameObject.transform.position = new Vector2 (0.0f, 0.0f);
		vel.x = 0.0f;
		vel.y = 0.0f;
		rb.velocity = vel;
		Invoke("Spawn", 0.8f);
	}

	void OnTriggerEnter2D () {
		ping_effect.Play ();
	}
}