//
// Created by Administrator on 2016/11/17.
//

#define LOG_NDEBUG 0
#define TAG "AudioEngine"

#include <ytxplayer/audio_engine.h>
#include "ytxplayer/ALog-priv.h"


AudioEngine::AudioEngine() {
    engineObject = NULL;
    engineEngine = NULL;

    outputMixObject = NULL;
    outputMixEnvironmentalReverb = NULL;

    bqPlayerObject = NULL;
    bqPlayerPlay = NULL;
    bqPlayerBufferQueue = NULL;
    bqPlayerEffectSend = NULL;
    bqPlayerMuteSolo = NULL;
    bqPlayerVolume = NULL;
    bqPlayerSampleRate = 0;
    bqPlayerBufSize = 0;
    resampleBuf = NULL;

    audioEngineLock = PTHREAD_MUTEX_INITIALIZER;

    uriPlayerObject = NULL;
    uriPlayerPlay = NULL;
    uriPlayerSeek = NULL;
    uriPlayerMuteSolo = NULL;
    uriPlayerVolume = NULL;

    fdPlayerObject = NULL;
    fdPlayerPlay = NULL;
    fdPlayerSeek = NULL;
    fdPlayerMuteSolo = NULL;
    fdPlayerVolume = NULL;


    pcmPlayerObject = NULL;
    pcmPlayerPlay = NULL;
    pcmPlayerSeek = NULL;
    pcmPlayerMuteSolo = NULL;
    pcmPlayerVolume = NULL;


    recorderObject = NULL;
    recorderRecord = NULL;
    recorderBufferQueue = NULL;

    recorderSize = 0;

    nextBuffer = NULL;
    nextSize = 0;
    nextCount = 0;
}

AudioEngine::~AudioEngine() {
    //释放资源
    // destroy buffer queue audio player object, and invalidate all associated interfaces

    if (bqPlayerObject != NULL) {
        (*bqPlayerObject)->Destroy(bqPlayerObject);
        bqPlayerObject = NULL;
        bqPlayerPlay = NULL;
        bqPlayerBufferQueue = NULL;
        bqPlayerEffectSend = NULL;
        bqPlayerMuteSolo = NULL;
        bqPlayerVolume = NULL;
    }

    // destroy output mix object, and invalidate all associated interfaces
    if (outputMixObject != NULL) {
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject = NULL;
        outputMixEnvironmentalReverb = NULL;
    }

    // destroy engine object, and invalidate all associated interfaces
    if (engineObject != NULL) {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineEngine = NULL;
    }

}

void AudioEngine::createEngine() {

    SLresult result;

    //创建engine
    result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    //实例化realize
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    //获取引擎接口,这个接口是为了获取其他需要的对象
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;


    //创建输出混合器,这个混合器是对于环境的说明.他是一个非必须的接口
    const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean req[1] = {SL_BOOLEAN_FALSE};
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, ids, req);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    //实例化输出混合器
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    //获取环境混响接口.如果环境混响效果不可用这可能会失败,要么是这个特性不被支持因为过多的CPU负荷,
    // 要么是请求MODIFY_AUDIO_SETTINGS许可不被允许或获取
    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
                                              &outputMixEnvironmentalReverb);
    if (SL_RESULT_SUCCESS == result) {
        result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
                outputMixEnvironmentalReverb, &reverbSettings);
        (void) result;
    }
    // 忽略环境混响失败的结果,对这个sample来说是可选的

}


void AudioEngine::RegisterCallback(slAndroidSimpleBufferQueueCallback callback, void *pContext) {
    SLresult result;
    // register callback on the buffer queue
    result = (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue, callback, pContext);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    bqPlayerEffectSend = NULL;
    if (0 == bqPlayerSampleRate) {
        result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_EFFECTSEND,
                                                 &bqPlayerEffectSend);
        assert(SL_RESULT_SUCCESS == result);
        (void) result;
    }

#if 0   // mute/solo is not supported for sources that are known to be mono, as this is
    // get the mute/solo interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_MUTESOLO, &bqPlayerMuteSolo);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;
#endif

    // get the volume interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_VOLUME, &bqPlayerVolume);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    // set the player's state to playing
    result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;
}

void AudioEngine::createBufferQueueAudioPlayer(int sampleRate, int bufSize, int channel) {

    SLresult result;
    if (sampleRate >= 0 && bufSize >= 0) {
        bqPlayerSampleRate = sampleRate * 1000;
        //
        // 设备本地buffer size 是减少音频延时的一个因素,我们这里仅播放一个较大的buffer
        //
        bqPlayerBufSize = bufSize;
    }

    // 配置音频输入源
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, 1, SL_SAMPLINGRATE_8,
                                   SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
                                   SL_SPEAKER_FRONT_CENTER, SL_BYTEORDER_LITTLEENDIAN};
    //当可能的时候使能fast audio:一旦我们设置相同的rate到本地,快速音频路径将被触发
    if (bqPlayerSampleRate) {
        format_pcm.samplesPerSec = bqPlayerSampleRate;       //sample rate in mili second
    }

    ALOGI("createBufferQueueAudioPlayer channel=%d\n", channel);
    if (channel == 2) {
        format_pcm.numChannels = 2;
        format_pcm.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
    } else if (channel == 1) {
        format_pcm.numChannels = 1;
    }

    SLDataSource audioSrc = {&loc_bufq, &format_pcm};

    //配置音频接收器
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&loc_outmix, NULL};

    //创建audio player:
    //     fast audio 不被支持当 SL_IID_EFFECTSEND(效果输出) 被请求,对fast audio跳过他
    //
    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME, SL_IID_EFFECTSEND,
            /*SL_IID_MUTESOLO,*/};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE,
            /*SL_BOOLEAN_TRUE,*/ };

    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bqPlayerObject, &audioSrc, &audioSnk,
                                                bqPlayerSampleRate ? 2 : 3, ids, req);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    // realize the player
    result = (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    // get the play interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerPlay);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    // get the buffer queue interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE,
                                             &bqPlayerBufferQueue);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

}


void AudioEngine::setPlayingAudioPlayer(bool isPlaying) {
    SLresult result;

    if (NULL != bqPlayerPlay) {

        // set the player's state
        result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, isPlaying ?
                                                             SL_PLAYSTATE_PLAYING
                                                                       : SL_PLAYSTATE_PAUSED);
        assert(SL_RESULT_SUCCESS == result);
        (void) result;
    }
}


void AudioEngine::setMuteAudioPlayer(bool isMute) {
    SLresult result;

    if (NULL != bqPlayerVolume) {
        result = (*bqPlayerVolume)->SetMute(bqPlayerVolume, isMute);
        assert(SL_RESULT_SUCCESS == result);
        (void) result;
    }
}