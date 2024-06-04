#pragma once

#include <memory>
#include "VLCMovie.h"

class ofxVLCVideoPlayer
{
    shared_ptr<VLCMovie> vlcMovieInstance;
    ofTexture dummyTexture;
    ofImage dummyImage;
public:
    ofxVLCVideoPlayer(void);
    ~ofxVLCVideoPlayer(void);
    bool loadMovie(std::string name);
    bool loadMovie(void* opaqueMedia, openCallback openCb, closeCallback closeCb, readCallback readCb, seekCallback seekCb);
    void finalizeInit(); // Needs to be called after loadMovie or when getNeedsPostInit and from open frameworks thread
    void closeMovie();
    void update();
    ofTexture& getTextureReference();
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
    bool getNeedsPostInit();
    bool isLoaded();
    bool isFrameReady();
    bool isRotated();
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