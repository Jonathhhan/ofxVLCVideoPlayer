#pragma once

#include "ofMain.h"
#ifdef WIN32
#define LIBVLC_USE_PTHREAD_CANCEL
typedef SSIZE_T ssize_t;
#endif
#include "vlc/vlc.h"
#include <memory>
#include <cassert>
#include <vlc/libvlc_media.h>
#include <vlc/libvlc_media_player.h>

class ofxVLCVideoPlayer {
    ofTexture dummyTexture;
    ofImage dummyImage;
    //shared_ptr<ofxVLCVideoPlayer> vlcMovieInstance;
    std::string filename;

    ofImage image[2];
    ofImage* frontImage;
    ofImage* backImage;
    ofTexture* frontTexture;

    libvlc_instance_t* libvlc;
    libvlc_media_t* m;
    libvlc_media_player_t* mp;
    libvlc_event_manager_t* eventManager;

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
    ofxVLCVideoPlayer(void);
    ~ofxVLCVideoPlayer(void);
    void setup(int vlc_argc, char const* vlc_argv[]);
    void loadMovie(std::string name);
    void closeMovie();
    void update();
    void setTexture(ofTexture tex);
    ofTexture& getTexture();
    void draw(float x, float y, float w, float h);
    void draw(float x, float y);
    void play();
    void pause();
    void stop();
    bool getIsMovieDone();
    void setPosition(float pct);
    void setLoop(bool loop);
    float getHeight();
    float getWidth();
    bool isPlaying();
    bool isLoaded();
    float getPosition();
    int getTimeMillis();
    void setTimeMillis(int ms);
    float getFPS();
    float getDuration();
    void setFrame(int frame);
    int getCurrentFrame();
    int getTotalNumFrames();
    void setVolume(int volume);
    void toggleMute();
};