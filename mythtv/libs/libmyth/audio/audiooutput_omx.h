#ifndef AUDIOOUTPUT_OMX_H
#define AUDIOOUTPUT_OMX_H

#include <OMX_Types.h>
#include <OMX_Core.h>

#include <QMutex>
#include <QSemaphore>
#include <QList>
#include <QAtomicInt>

#include "audiooutputbase.h"
#include "omxcontext.h"

class AudioOutputOMX : public AudioOutputBase, private OMXComponentCtx
{
    // No copying
    AudioOutputOMX(const AudioOutputOMX&);
    AudioOutputOMX & operator =(const AudioOutputOMX&);

  public:
    explicit AudioOutputOMX(const AudioSettings &settings);
    virtual ~AudioOutputOMX();

    // VolumeBase implementation
    int GetVolumeChannel(int channel) const override; // VolumeBase
    void SetVolumeChannel(int channel, int volume) override; // VolumeBase

  protected:
    // AudioOutputBase implementation
    bool OpenDevice(void) override; // AudioOutputBase
    void CloseDevice(void) override; // AudioOutputBase
    void WriteAudio(unsigned char *aubuf, int size) override; // AudioOutputBase
    int  GetBufferedOnSoundcard(void) const override; // AudioOutputBase

    // AudioOutputBase overrides
    AudioOutputSettings* GetOutputSettings(bool passthrough) override; // AudioOutputBase

  private:
    // OMXComponentCtx implementation
    OMX_ERRORTYPE EmptyBufferDone (OMXComponent&, OMX_BUFFERHEADERTYPE*) override; // OMXComponentCtx
    void ReleaseBuffers(OMXComponent&) override; // OMXComponentCtx

  private:
    // implementation
    bool OpenMixer();

    // OMXComponentCB actions
    typedef OMX_ERRORTYPE ComponentCB();
    ComponentCB FreeBuffersCB, AllocBuffersCB;

    void reorderChannels(int *aubuf, int size);
    void reorderChannels(short *aubuf, int size);
    void reorderChannels(uchar *aubuf, int size);

  private:
    OMXComponent m_audiorender;

    QSemaphore m_ibufs_sema;    // EmptyBufferDone signal
    QMutex mutable m_lock;      // Protects data following
    QList<OMX_BUFFERHEADERTYPE*> m_ibufs;
    QAtomicInt m_pending;
};

#endif // ndef AUDIOOUTPUT_OMX_H
