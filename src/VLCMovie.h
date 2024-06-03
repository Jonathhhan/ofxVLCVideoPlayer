#pragma once

#include "ofMain.h"
#ifdef WIN32
#define LIBVLC_USE_PTHREAD_CANCEL
typedef SSIZE_T ssize_t;
#endif
#include <vlc/vlc.h>
#include <memory>

typedef int     (*openCallback) (void *opaque, void **datap, uint64_t *sizep);
typedef void    (*closeCallback)(void *opaque);
typedef ssize_t (*readCallback) (void *opaque, unsigned char *buf, size_t len);
typedef int     (*seekCallback) (void *opaque, uint64_t offset);

//class VLCMovie : public AbstractMovie, public ofBaseSoundOutput
class VLCMovie
{
    enum { FILE, CALLBACKS } mediaType;
    string filename;
    void * opaqueMedia;
    openCallback openCb;
    closeCallback closeCb;
    readCallback readCb;
    seekCallback seekCb;

    ofImage image[2];
    ofImage *frontImage;
    ofImage *backImage;
    ofTexture *frontTexture;
	ofImage thumbnailImage;

    libvlc_instance_t *libvlc;
    libvlc_media_t *m;
    libvlc_media_player_t *mp;
    libvlc_event_manager_t *eventManager;

    void initializeVLC();
    void loadMedia();
    void cleanupVLC();

    bool isInitialized;
    bool isVLCInitialized;

    int videoWidth, videoHeight;
    float fps = -1;
	libvlc_time_t video_length_ms;
	bool isThumbnailOK;
    bool isFliped;
    bool isLooping;
    bool tryUpdate;
    bool needPostInit = false;
    bool firstFrameReady = false;

    bool movieFinished;

    std::mutex playerLock;
    std::mutex backImageMutex;
    std::mutex imageFlipMutex;

    // VLC Video callbaks
    static void *lockStatic(void *data, void **p_pixels);
    static void unlockStatic(void *data, void *id, void *const *p_pixels);
    static void displayStatic(void *data, void *id);

	static void *lockForThumbnailStatic(void *data, void **p_pixels);
    static void unlockForThumbnailStatic(void *data, void *id, void *const *p_pixels);
    static void displayForThumbnailStatic(void *data, void *id);

	void *lock(void **p_pixels);
    void unlock(void *id, void *const *p_pixels);
    void display(void *id);

	void *lockForThumbnail(void **p_pixels);
    void unlockForThumbnail(void *id, void *const *p_pixels);
    void displayForThumbnail(void *id);

    // VLC Event callbacks
    static void vlcEventStatic(const libvlc_event_t *event, void *data);
    void vlcEvent(const libvlc_event_t *event);

public:
    VLCMovie(string filename);
    VLCMovie(void * opaqueMedia, openCallback openCb, closeCallback closeCb, readCallback readCb, seekCallback seekCb);
    ~VLCMovie(void);
    void init();
    void postInit(); // For initialization that needs to be done in the same thread as open frameworks
    void play();
    void pause();
    void rewind();
    void stop();
    void seek(float position);
    void setLoop(bool isLooping);
    unsigned int getImageWidth();
    unsigned int getImageHeight();
    float getFPS();
    void setFPS(float fps);
    float getDuration();
    int getTotalNumFrames();
    void updateTexture();
    ofTexture &getTexture();
	ofImage &getThumbnailImage();
    bool isMovieFinished();
    bool isPlaying();
    bool isFirstFrameReady();
    bool isRotated();
    bool getIsInitialized();
    bool getNeedsPostInit();
	float getPosition();
	libvlc_time_t getTimeMillis();
    void setTimeMillis(libvlc_time_t ms);
    void setFrame(int frame);
    int getCurrentFrame();
    void setVolume(int volume);
    void toggleMute();
};
