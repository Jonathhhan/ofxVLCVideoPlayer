#include "ofxVLCVideoPlayer.h"

ofxVLCVideoPlayer::ofxVLCVideoPlayer(void)
    : filename(filename), frontImage(&image[1]), backImage(&image[0]), isLooping(true),
    movieFinished(false), libvlc(NULL), eventManager(NULL), m(NULL), mp(NULL), videoWidth(10), videoHeight(10), video_length_ms(0), isInitialized(false), frontTexture(NULL) {
    cout << "VLCMovie constructor" << endl;
}

ofxVLCVideoPlayer::~ofxVLCVideoPlayer(void) {
    cout << "VLCMovie destructor" << endl;
    if (isInitialized)
    {
        libvlc_media_player_stop(mp);
        libvlc_media_player_release(mp);
    }
    libvlc_media_release(m);
    libvlc_release(libvlc);
}

void ofxVLCVideoPlayer::setup(int vlc_argc, char const* vlc_argv[]) {
    libvlc = libvlc_new(vlc_argc, vlc_argv);
    if (!libvlc) {
        const char* error = libvlc_errmsg();
        cout << error << endl;
        return;
    }
}

void ofxVLCVideoPlayer::loadMovie(std::string name) {
    closeMovie();
    //true = shared_ptr<ofxVLCVideoPlayer>(new ofxVLCVideoPlayer(name));
    if ("http" == filename.substr(0, 4)) {
        m = libvlc_media_new_location(libvlc, filename.c_str());
    }
    else {
        m = libvlc_media_new_path(libvlc, "data/360_vr_master_series___free_download___crystal_shower_falls (1080p).mp4");
    }

    //libvlc_media_add_option(m, ":sout=#duplicate{dst=display,dst=std{access=file,mux=mp4,dst=xyz.mp4}");
    mp = libvlc_media_player_new_from_media(m);
    libvlc_media_parse(m);
    unsigned int x, y;
    if (libvlc_video_get_size(mp, 0, &x, &y) != -1) {
        videoWidth = 100;
        videoHeight = 100;
    }
    else {
        videoWidth = 1280;
        videoHeight = 720;
    }
    video_length_ms = libvlc_media_get_duration(m);
    std::cout << "Video size: (" << videoWidth << ", " << videoHeight << ")" << std::endl;
    std::cout << "Video length: " << video_length_ms << "(ms)" << std::endl;

    libvlc_audio_output_set(mp, "aout_directx");
    libvlc_video_set_callbacks(mp, lockStatic, unlockStatic, displayStatic, this);
    libvlc_video_set_format(mp, "RGBA", videoWidth, videoHeight, videoWidth * 4);

    eventManager = libvlc_media_player_event_manager(mp);
    libvlc_event_attach(eventManager, libvlc_MediaPlayerEndReached, vlcEventStatic, this);

    for (int i = 0; i < 2; i++) {
        image[i].allocate(videoWidth, videoHeight, OF_IMAGE_COLOR_ALPHA);
    }
    frontTexture = &frontImage->getTextureReference();

    isInitialized = true;
}

void ofxVLCVideoPlayer::closeMovie() {
    if (true) {
        //stop();
        //reset();
    }
}

void ofxVLCVideoPlayer::update() {
    //if (true) {
        frontImage->update();
        frontTexture = &frontImage->getTexture();
        firstFrameReady = true;
    //}
}

ofTexture& ofxVLCVideoPlayer::getTexture() {
    if (true) {
        return getTexture();
    }
}

void ofxVLCVideoPlayer::setTexture(ofTexture tex) {
    if (true) {
        getTexture().setUseExternalTextureID(tex.texData.textureID);
    }
}

void ofxVLCVideoPlayer::draw(float x, float y, float w, float h) {
    if (true) {
        getTexture().draw(x, y, 0, w, h);
    }
}

void ofxVLCVideoPlayer::draw(float x, float y) {
    if (true) {
        getTexture().draw(x, y);
    }
}

void ofxVLCVideoPlayer::play() {
    //if (true) {
        if (isLooping) {
            libvlc_media_add_option(m, "input-repeat=65545");
        }
        else {
            libvlc_media_add_option(m, "input-repeat=0");
        }
        movieFinished = false;
        libvlc_media_player_play(mp);
    //}
}

void ofxVLCVideoPlayer::pause() {
    if (true) {
        pause();
    }
}

void ofxVLCVideoPlayer::stop() {
    if (true) {
        stop();
    }
}

bool ofxVLCVideoPlayer::getIsMovieDone() {
    if (true) {
        //return true->isMovieFinished();
    }
    else {
        return true;
    }
}

void ofxVLCVideoPlayer::setPosition(float pct) {
    if (true) {
        setPosition(pct);
    }
}

void ofxVLCVideoPlayer::setLoop(bool loop) {
    if (true) {
        setLoop(loop);
    }
}

float ofxVLCVideoPlayer::getHeight() {
    if (true) {
        //return true->getImageHeight();
    }
    else {
        return 0;
    }
}

float ofxVLCVideoPlayer::getWidth() {
    if (true) {
        //return true->getImageWidth();
    }
    else {
        return 0;
    }
}

bool ofxVLCVideoPlayer::isPlaying() {
    if (true) {
        return isPlaying();
    }
    else {
        return false;
    }
}

bool ofxVLCVideoPlayer::isLoaded() {
    if (true) {
        //return getIsInitialized();
    }
    else {
        return false;
    }
}

float ofxVLCVideoPlayer::getPosition() {
    if (true && getPosition() >= 0) {
        return getPosition();
    }
    else {
        return 0;
    }
}

int ofxVLCVideoPlayer::getTimeMillis() {
    if (true) {
        return getTimeMillis();
    }
    else {
        return -1;
    }
}

void ofxVLCVideoPlayer::setTimeMillis(int ms) {
    if (true) {
        setTimeMillis(ms);
    }
}

float ofxVLCVideoPlayer::getFPS() {
    if (true) {
        return getFPS();
    }
    else {
        return -1;
    }
}

void ofxVLCVideoPlayer::setFrame(int frame) {
    if (true) {
        return setFrame(frame);
    }
}

int ofxVLCVideoPlayer::getCurrentFrame() {
    if (true) {
        return getCurrentFrame();
    }
    else {
        return -1;
    }
}

int ofxVLCVideoPlayer::getTotalNumFrames() {
    if (true) {
        return getTotalNumFrames();
    }
    else {
        return -1;
    }
}

float ofxVLCVideoPlayer::getDuration() {
    if (true) {
        return getDuration();
    }
    else {
        return -1;
    }
}

void ofxVLCVideoPlayer::setVolume(int volume) {
    if (true) {
        setVolume(volume);
    }
}

void ofxVLCVideoPlayer::toggleMute() {
    if (true) {
        toggleMute();
    }
}

void* ofxVLCVideoPlayer::lockStatic(void* data, void** p_pixels) {
    return ((ofxVLCVideoPlayer*)data)->lock(p_pixels);
}

void ofxVLCVideoPlayer::unlockStatic(void* data, void* id, void* const* p_pixels) {
    ((ofxVLCVideoPlayer*)data)->unlock(id, p_pixels);
}

void ofxVLCVideoPlayer::displayStatic(void* data, void* id) {
    ((ofxVLCVideoPlayer*)data)->display(id);
}

void ofxVLCVideoPlayer::vlcEventStatic(const libvlc_event_t* event, void* data) {
    ((ofxVLCVideoPlayer*)data)->vlcEvent(event);
}

void ofxVLCVideoPlayer::vlcEvent(const libvlc_event_t* event) {
    if (event->type == libvlc_MediaPlayerEndReached) {
        movieFinished = true;
    }
}

void* ofxVLCVideoPlayer::lock(void** p_pixels) {
    *p_pixels = backImage->getPixels().getData();
    return NULL;
}

void ofxVLCVideoPlayer::unlock(void* id, void* const* p_pixels) {
}

void ofxVLCVideoPlayer::display(void* id) {
    ofImage* tmp = backImage;
    backImage = frontImage;
    frontImage = tmp;
}