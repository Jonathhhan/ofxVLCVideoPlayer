#pragma once

#include "ofMain.h"
#ifdef WIN32
#define LIBVLC_USE_PTHREAD_CANCEL
typedef SSIZE_T ssize_t;
#endif
#include "vlc/vlc.h"
#include <memory>

class VLCMovie {
    string filename;

    ofImage image[2];
    ofImage* frontImage;
    ofImage* backImage;
    ofTexture* frontTexture;

    libvlc_instance_t* libvlc;
    libvlc_media_t* m;
    libvlc_media_player_t* mp;
    libvlc_event_manager_t* eventManager;

    void loadMedia();
    void cleanupVLC();

    bool isInitialized;
    int videoWidth, videoHeight;
    float fps = -1;
    libvlc_time_t video_length_ms;
    bool isLooping;
    bool firstFrameReady = false;
    bool movieFinished;

    // VLC Video callbaks
    static void* lockStatic(void* data, void** p_pixels);
    static void unlockStatic(void* data, void* id, void* const* p_pixels);
    static void displayStatic(void* data, void* id);

    void* lock(void** p_pixels);
    void unlock(void* id, void* const* p_pixels);
    void display(void* id);

    // VLC Event callbacks
    static void vlcEventStatic(const libvlc_event_t* event, void* data);
    void vlcEvent(const libvlc_event_t* event);

public:
    VLCMovie(std::string filename);
    ~VLCMovie(void);
    void init(int vlc_argc, char const* vlc_argv[]);
    void play();
    void pause();
    void stop();
    void setPosition(float position);
    void setLoop(bool isLooping);
    unsigned int getImageWidth();
    unsigned int getImageHeight();
    float getFPS();
    void setFPS(float fps);
    float getDuration();
    int getTotalNumFrames();
    void updateTexture();
    ofTexture& getTexture();
    bool isMovieFinished();
    bool isPlaying();
    bool isFirstFrameReady();
    bool getIsInitialized();
    float getPosition();
    libvlc_time_t getTimeMillis();
    void setTimeMillis(libvlc_time_t ms);
    void setFrame(int frame);
    int getCurrentFrame();
    void setVolume(int volume);
    void toggleMute();
};