#include "stdafx.h"
#include "Camera.h"

#include "AudioSystem.h"


AudioSystem::AudioSystem()
{
	m_audio_engine = nullptr;
	m_sound = nullptr;

}


AudioSystem::~AudioSystem()
{
	Cleanup();
}

void AudioSystem::Initialize(Camera* cam)
{
	m_audio_engine = irrklang::createIrrKlangDevice();

	m_sound_list[0] = m_audio_engine->addSoundSourceFromFile("../data/audio/explosion.wav");
	m_sound_list[1] = m_audio_engine->addSoundSourceFromFile("../data/audio/explosion.wav");
	m_sound_list[2] = m_audio_engine->addSoundSourceFromFile("../data/audio/getout.ogg");
	m_sound_list[3] = m_audio_engine->addSoundSourceFromFile("../data/audio/explosion.wav");
	m_sound_list[4] = m_audio_engine->addSoundSourceFromFile("../data/audio/explosion.wav");
	m_sound_list[5] = m_audio_engine->addSoundSourceFromFile("../data/audio/explosion.wav");
	m_sound_list[6] = m_audio_engine->addSoundSourceFromFile("../data/audio/ophelia.mp3");
	m_sound_list[7] = m_audio_engine->addSoundSourceFromFile("../data/audio/explosion.wav");

	
	m_listener_pos = irrklang::vec3df(cam->GetPosition().x, cam->GetPosition().y, cam->GetPosition().z);
	m_audio_engine->setListenerPosition(m_listener_pos, m_listener_pos);

}

void AudioSystem::Update(Camera* cam)
{
	m_listener_pos = irrklang::vec3df(cam->GetPosition().x, cam->GetPosition().y, cam->GetPosition().z);
	m_audio_engine->setListenerPosition(m_listener_pos, m_listener_pos);
}

void AudioSystem::Cleanup()
{
	m_audio_engine->stopAllSounds();
	for (int i = 0; i < m_audio_engine->getSoundSourceCount(); i++)
	{
		m_sound_list[i]->drop();
		m_audio_list[i]->drop();
	}
	m_audio_engine->removeAllSoundSources();
	m_audio_engine->drop();
}



void AudioSystem::DestroySound(Sounds id)
{
	m_sound_list[id]->drop();
	m_audio_list[id]->drop();
}


void AudioSystem::DestroyMusic(Sounds id)
{
	m_sound_list[id]->drop();
	m_audio_list[id]->drop();
}


void AudioSystem::PlaySound(Sounds id, float volume)
{
	m_audio_list[id] = m_audio_engine->play2D(m_sound_list[id], false, false, true);
	m_audio_list[id]->setVolume(volume);
	m_audio_list[id]->setIsPaused(false);
}

void AudioSystem::PlaySound(Sounds id, float volume, Vec3 position)
{
	irrklang::vec3df pos(position.x, position.y, position.z);
	m_audio_list[id] = m_audio_engine->play3D(m_sound_list[id], pos,false, false, true);
	m_audio_list[id]->setVolume(volume);
	m_audio_list[id]->setIsPaused(false);
}

void AudioSystem::StopSound(Sounds id)
{
	m_audio_list[id]->stop();
}


void AudioSystem::PlayMusic(Sounds id, float volume)
{
	m_audio_list[id] = m_audio_engine->play2D(m_sound_list[id],true,false,true);
	m_audio_list[id]->setVolume(volume);
	m_audio_list[id]->setIsPaused(false);
}

void AudioSystem::StopMusic(Sounds id)
{
	m_audio_list[id]->stop();
}
