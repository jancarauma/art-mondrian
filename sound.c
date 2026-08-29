/*************************************************************************
 * 
 * sound.c
 * 
 * (c) 2021 J. G. Silva (django) <dparicarana@gmail.com>
 * 
 * "Snake game written in C - animations, retro menu, smart snake 
 * enemies, sound effects."
 * 
 * last modification: 2021-01-07
 * 
 ************************************************************************/
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bass.h"

/* Canal principal do dispositivo de áudio (áudios principais do jogo) */
unsigned long int audio_mainchannel;

/* Canal auxiliar para efeitos curtos sobre o áudio principal */
unsigned long int audio_effectchannel;

/* Nome do último arquivo tocado no canal principal, usado por audio_is_track_playing */
static char last_track[256] = "";

/* Reproduz o áudio correspondente ao arquivo dado, atualizando *audiochannel
 * com o handle do novo stream (senão o handle antigo é perdido e o canal
 * não pode mais ser parado/reutilizado pelo chamador) */
int audio_play(unsigned long int *audiochannel, char *filename,
	short repeat)
{
	unsigned long int new_channel;

	BASS_ChannelStop(*audiochannel);

	if(!(new_channel = BASS_StreamCreateFile(
		FALSE, filename, 0, 0, repeat ? BASS_SAMPLE_LOOP : 0)))
//	&&if(!(new_channel = BASS_MusicLoad(
//		FALSE, filename, 0, 0, BASS_MUSIC_RAMP | BASS_SAMPLE_FLOAT | (BASS_SAMPLE_LOOP && repeat), 1)))
	{
		// Erro ao carregar arquivo...
		return(EXIT_FAILURE);
	} else {
		*audiochannel = new_channel;

		strncpy(last_track, filename, sizeof(last_track) - 1);
		last_track[sizeof(last_track) - 1] = '\0';

		// Inicia reprodução do áudio...
		BASS_ChannelPlay(*audiochannel, FALSE);
	}
	return(EXIT_SUCCESS);
}

/* Desabilita reprodução de áudio */
void audio_stop(void)
{
	BASS_Stop();
}

/* Indica se o canal ainda está tocando (útil para esperar um áudio terminar) */
int audio_is_playing(unsigned long int audiochannel)
{
	return(BASS_ChannelIsActive(audiochannel) == BASS_ACTIVE_PLAYING);
}

/* Indica se o arquivo dado é o que está tocando agora no canal principal */
int audio_is_track_playing(const char *filename)
{
	return(audio_is_playing(audio_mainchannel) && strcmp(last_track, filename) == 0);
}

/* Habilita reprodução de áudio */
void audio_resume(void)
{
	BASS_Start();
}

/* Inicializa o módulo de áudio com as configurações padrão */
int audio_initialize(void)
{
	// Verifica se foi carregada uma versão correta da biblioteca BASS
	if (HIWORD(BASS_GetVersion()) != BASSVERSION)
	{
		return(BASS_ERROR_VERSION);
	}
	// Inicializa a biblioteca BASS
	if (!BASS_Init(-1, 44100, 0, NULL, NULL)) 
	{
		return(BASS_ERROR_INIT);
	}
	return(BASS_OK);
}

/* Finaliza o módulo de áudio corretamente */
void audio_terminate(void)
{
	BASS_Free();
}
