#pragma once

#include <memory>
#include "ofxVLCMovie.h"

class ofxVLCVideoPlayer {
    shared_ptr<ofxVLCMovie> vlcMovieInstance;
    ofTexture dummyTexture;
    ofImage dummyImage;
public:
    ofxVLCVideoPlayer(void);
    ~ofxVLCVideoPlayer(void);
    void loadMovie(std::string name, int vlc_argc, char const* vlc_argv[]);
    void closeMovie();
    void update();
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