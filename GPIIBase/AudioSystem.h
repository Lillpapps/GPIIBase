#pragma once



class Camera;

class AudioSystem
{
public:
	AudioSystem();
	~AudioSystem();

	enum Sounds
	{
		Tower1,
		Tower2,
		Attack1,
		Attack2,
		Enemy1,
		Enemy2,
		Fill1,
		Fill2
	};


	void Initialize(Camera* cam);
	void Update(Camera* cam);
	void Cleanup();

	void DestroySound(Sounds id);
	void DestroyMusic(Sounds id);

	void PlaySound(Sounds id, float volume);
	void PlaySound(Sounds id, float volume, Vec3 position);
	void StopSound(Sounds id);

	void PlayMusic(Sounds id, float volume);
	void StopMusic(Sounds id);

private:

	irrklang::ISoundEngine* m_audio_engine;
	irrklang::ISound* m_sound;
	irrklang::vec3df m_listener_pos;
	irrklang::ISoundSource* m_sound_list[8];

	irrklang::ISound* m_audio_list[8];


};

