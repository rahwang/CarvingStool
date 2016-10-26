using UnityEngine;

/// <summary>
/// Creating instance of particles from code with no effort
/// </summary>
public class ExplosionSpawn : MonoBehaviour
{
	/// <summary>
	/// Singleton
	/// </summary>
	public static ExplosionSpawn Instance;

	public ParticleSystem effect;

	void Awake()
	{
		// Register the singleton
		if (Instance != null)
		{
			Debug.LogError("Multiple instances!");
		}

		Instance = this;
	}

	/// Create an explosion at the given location
	public void Explosion(Vector3 position, Transform trans)
	{
		instantiate(effect, position, trans);
	}

	/// Instantiate a Particle system from prefab
	private ParticleSystem instantiate(ParticleSystem prefab, Vector3 position, Transform trans)
	{
		ParticleSystem newParticleSystem = Instantiate(
			prefab,
			position,
			Quaternion.identity
		) as ParticleSystem;

		newParticleSystem.transform.parent = trans;

		// Make sure it will be destroyed
		Destroy(
			newParticleSystem.gameObject,
			newParticleSystem.startLifetime
		);

		return newParticleSystem;
	}
}