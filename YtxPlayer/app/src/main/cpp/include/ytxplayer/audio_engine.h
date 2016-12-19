//
// Created by Administrator on 2016/11/17.
//

#ifndef YTXPLAYER_AUDIO_ENGINE_H
#define YTXPLAYER_AUDIO_ENGINE_H
#include <assert.h>
#include <jni.h>
#include <string.h>
#include <pthread.h>
#include "lock.h"

#ifdef __cplusplus
extern "C" {
#endif
// for native audio
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#ifdef __cplusplus
}
#endif

// synthesized sawtooth clip
#define SAWTOOTH_FRAMES 8000

// 5 seconds of recorded audio at 16 kHz mono, 16-bit signed little endian
#define RECORDER_FRAMES (16000 * 5)

class AudioEngine{
public:

    // engine interfaces
     SLObjectItf engineObject = NULL;
     SLEngineItf engineEngine;

// output mix interfaces
     SLObjectItf outputMixObject = NULL;
     SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;

// buffer queue player interfaces
     SLObjectItf bqPlayerObject = NULL;
     SLPlayItf bqPlayerPlay;
     SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
     SLEffectSendItf bqPlayerEffectSend;
     SLMuteSoloItf bqPlayerMuteSolo;
     SLVolumeItf bqPlayerVolume;
     SLmilliHertz bqPlayerSampleRate = 0;
     jint   bqPlayerBufSize = 0;
     short *resampleBuf = NULL;
// a mutext to guard against re-entrance to record & playback
// as well as make recording and playing back to be mutually exclusive
// this is to avoid crash at situations like:
//    recording is in session [not finished]
//    user presses record button and another recording coming in
// The action: when recording/playing back is not finished, ignore the new request
     pthread_mutex_t  audioEngineLock = PTHREAD_MUTEX_INITIALIZER;

// aux effect on the output mix, used by the buffer queue player
     const SLEnvironmentalReverbSettings reverbSettings =
            SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

// URI player interfaces
     SLObjectItf uriPlayerObject = NULL;
     SLPlayItf uriPlayerPlay;
     SLSeekItf uriPlayerSeek;
     SLMuteSoloItf uriPlayerMuteSolo;
     SLVolumeItf uriPlayerVolume;

// file descriptor player interfaces
     SLObjectItf fdPlayerObject = NULL;
     SLPlayItf fdPlayerPlay;
     SLSeekItf fdPlayerSeek;
     SLMuteSoloItf fdPlayerMuteSolo;
     SLVolumeItf fdPlayerVolume;

//pcm player interfaces
     SLObjectItf pcmPlayerObject = NULL;
     SLPlayItf pcmPlayerPlay;
     SLSeekItf pcmPlayerSeek;
     SLMuteSoloItf pcmPlayerMuteSolo;
     SLVolumeItf pcmPlayerVolume;

// recorder interfaces
     SLObjectItf recorderObject = NULL;
     SLRecordItf recorderRecord;
     SLAndroidSimpleBufferQueueItf recorderBufferQueue;
     short sawtoothBuffer[SAWTOOTH_FRAMES];
     short recorderBuffer[RECORDER_FRAMES];
     unsigned recorderSize = 0;

    // pointer and size of the next player buffer to enqueue, and number of remaining buffers
     short *nextBuffer;
     unsigned nextSize;
     int nextCount;

public:
    void createEngine();
    void createBufferQueueAudioPlayer(int sampleRate, int bufSize,int channel,slAndroidSimpleBufferQueueCallback callback);
    void releaseResampleBuf(void);
    void selectClip(int which, int count);
    short* createResampledBuf(uint32_t idx, uint32_t srcRate, unsigned *size);
    // this callback handler is called every time a buffer finishes playing
    void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context);

protected:
    AudioEngine();
    ~AudioEngine();

public:
    static Lock mLock;
    static AudioEngine *getAudioEngine();
    static void releaseAudioEngine();

private:
    static AudioEngine *mAudioEngine;

};

#endif //YTXPLAYER_AUDIO_ENGINE_H
