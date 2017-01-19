//
// Created by Administrator on 2016/11/17.
//

#include <ytxplayer/audio_engine.h>
#define LOG_NDEBUG 0
#define TAG "AudioEngine"
#include "ALog-priv.h"

// pre-recorded sound clips, both are 8 kHz mono 16-bit signed little endian
static const char hello[] =
#include "hello_clip.h"
;

static const char android[] =
#include "android_clip.h"
;


AudioEngine::AudioEngine() {
    ALOGI("AudioEngine()");
}

AudioEngine::~AudioEngine() {
    //释放资源
    // destroy buffer queue audio player object, and invalidate all associated interfaces
    ALOGI("~AudioEngine()");
    if (bqPlayerObject != NULL) {
        ALOGI("~AudioEngine bqPlayerObject");
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

Lock AudioEngine::mLock;
AudioEngine *AudioEngine::mAudioEngine = NULL;

AudioEngine* AudioEngine::getAudioEngine() {
    if(mAudioEngine == NULL){
        mLock.lock();
        if(mAudioEngine == NULL){
            mAudioEngine = new AudioEngine();
        }
        mLock.unlock();
    }

    return mAudioEngine;
}

void AudioEngine::releaseAudioEngine() {
    if(mAudioEngine != NULL){
        mLock.lock();
        if(mAudioEngine != NULL){
            delete mAudioEngine;
            mAudioEngine = NULL;
        }
        mLock.unlock();
    }
}

void AudioEngine::createEngine() {

    SLresult result;

    //创建engine
    result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    //实例化realize
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    //获取引擎接口,这个接口是为了获取其他需要的对象
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;


    //创建输出混合器,这个混合器是对于环境的说明.他是一个非必须的接口
    const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean req[1] = {SL_BOOLEAN_FALSE};
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, ids, req);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    //实例化输出混合器
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    //获取环境混响接口.如果环境混响效果不可用这可能会失败,要么是这个特性不被支持因为过多的CPU负荷,
    // 要么是请求MODIFY_AUDIO_SETTINGS许可不被允许或获取
    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
                                              &outputMixEnvironmentalReverb);
    if (SL_RESULT_SUCCESS == result) {
        result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
                outputMixEnvironmentalReverb, &reverbSettings);
        (void)result;
    }
    // 忽略环境混响失败的结果,对这个sample来说是可选的

}

void AudioEngine::createBufferQueueAudioPlayer(int sampleRate, int bufSize,int channel,
                                               slAndroidSimpleBufferQueueCallback callback) {

    SLresult result;
    if (sampleRate >= 0 && bufSize >= 0 ) {
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
    if(bqPlayerSampleRate) {
        format_pcm.samplesPerSec = bqPlayerSampleRate;       //sample rate in mili second
    }

    ALOGI("createBufferQueueAudioPlayer channel=%d\n",channel);
    if(channel == 2){
        format_pcm.numChannels = 2;
        format_pcm.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
    }else if(channel == 1){
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
                                                bqPlayerSampleRate? 2 : 3, ids, req);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // realize the player
    result = (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get the play interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerPlay);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get the buffer queue interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE,
                                             &bqPlayerBufferQueue);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // register callback on the buffer queue
    result = (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue, callback, NULL);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get the effect send interface
    bqPlayerEffectSend = NULL;
    if( 0 == bqPlayerSampleRate) {
        result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_EFFECTSEND,
                                                 &bqPlayerEffectSend);
        assert(SL_RESULT_SUCCESS == result);
        (void)result;
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
    (void)result;

    // set the player's state to playing
    result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;


}

void AudioEngine::releaseResampleBuf() {
    if( 0 == bqPlayerSampleRate) {

        // we are not using fast path, so we were not creating buffers, nothing to do

        return;
    }

    free(resampleBuf);
    resampleBuf = NULL;
}

void AudioEngine::selectClip(int which, int count) {
    switch (which) {
        case 0:     // CLIP_NONE
            nextBuffer = (short *) NULL;
            nextSize = 0;
            break;
        case 1:     // CLIP_HELLO
            nextBuffer = createResampledBuf(1, SL_SAMPLINGRATE_8, &nextSize);
            if(!nextBuffer) {
                nextBuffer = (short*)hello;
                nextSize  = sizeof(hello);
            }
            break;
        case 2:     // CLIP_ANDROID
            nextBuffer = createResampledBuf(2, SL_SAMPLINGRATE_8, &nextSize);
            if(!nextBuffer) {
                nextBuffer = (short*)android;
                nextSize  = sizeof(android);
            }
            break;
        case 3:     // CLIP_SAWTOOTH
            nextBuffer = createResampledBuf(3, SL_SAMPLINGRATE_8, &nextSize);
            if(!nextBuffer) {
                nextBuffer = (short*)sawtoothBuffer;
                nextSize  = sizeof(sawtoothBuffer);
            }
            break;
        case 4:     // CLIP_PLAYBACK
            nextBuffer = createResampledBuf(4, SL_SAMPLINGRATE_16, &nextSize);
            // we recorded at 16 kHz, but are playing buffers at 8 Khz, so do a primitive down-sample
            if(!nextBuffer) {
                unsigned i;
                for (i = 0; i < recorderSize; i += 2 * sizeof(short)) {
                    recorderBuffer[i >> 2] = recorderBuffer[i >> 1];
                }
                recorderSize >>= 1;
                nextBuffer = recorderBuffer;
                nextSize = recorderSize;
            }
            break;
        default:
            nextBuffer = NULL;
            nextSize = 0;
            break;
    }
    nextCount = count;
    if (nextSize > 0) {
        // here we only enqueue one buffer because it is a long clip,
        // but for streaming playback we would typically enqueue at least 2 buffers to start
        SLresult result;
        result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, nextBuffer, nextSize);
        if (SL_RESULT_SUCCESS != result) {


        }
    }

}

short* AudioEngine::createResampledBuf(uint32_t idx, uint32_t srcRate, unsigned *size) {
    short  *src = NULL;
    short  *workBuf;
    int    upSampleRate;
    int32_t srcSampleCount = 0;

    if(0 == bqPlayerSampleRate) {
        return NULL;
    }
    if(bqPlayerSampleRate % srcRate) {
        /*
         * simple up-sampling, must be divisible
         */
        return NULL;
    }
    upSampleRate = bqPlayerSampleRate / srcRate;

    switch (idx) {
        case 0:
            return NULL;
        case 1: // HELLO_CLIP
            srcSampleCount = sizeof(hello) >> 1;
            src = (short*)hello;
            break;
        case 2: // ANDROID_CLIP
            srcSampleCount = sizeof(android) >> 1;
            src = (short*) android;
            break;
        case 3: // SAWTOOTH_CLIP
            srcSampleCount = SAWTOOTH_FRAMES;
            src = sawtoothBuffer;
            break;
        case 4: // captured frames
            srcSampleCount = recorderSize / sizeof(short);
            src =  recorderBuffer;
            break;
        default:
            assert(0);
            return NULL;
    }

    resampleBuf = (short*) malloc((srcSampleCount * upSampleRate) << 1);
    if(resampleBuf == NULL) {
        return resampleBuf;
    }
    workBuf = resampleBuf;
    for(int sample=0; sample < srcSampleCount; sample++) {
        for(int dup = 0; dup  < upSampleRate; dup++) {
            *workBuf++ = src[sample];
        }
    }

    *size = (srcSampleCount * upSampleRate) << 1;     // sample format is 16 bit
    return resampleBuf;
}

void AudioEngine::bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
    assert(bq == bqPlayerBufferQueue);
    assert(NULL == context);
    // for streaming playback, replace this test by logic to find and fill the next buffer
    if (--nextCount > 0 && NULL != nextBuffer && 0 != nextSize) {
        SLresult result;
        // enqueue another buffer
        result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, nextBuffer, nextSize);
        // the most likely other result is SL_RESULT_BUFFER_INSUFFICIENT,
        // which for this code example would indicate a programming error
//        if (SL_RESULT_SUCCESS != result) {
//            pthread_mutex_unlock(&audioEngineLock);
//        }
        (void)result;
    }
}