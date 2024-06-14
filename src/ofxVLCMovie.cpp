#include "ofxVLCMovie.h"
#include <cassert>
#include <vlc/libvlc_media.h>
#include <vlc/libvlc_media_player.h>

ofxVLCMovie::ofxVLCMovie(std::string filename)
    : filename(filename), frontImage(&image[1]), backImage(&image[0]), isLooping(true),
    movieFinished(false), isInitialized(false), frontTexture(NULL), libvlc(NULL), eventManager(NULL),
    m(NULL), mp(NULL), videoHeight(0), videoWidth(0), video_length_ms(0) {
    cout << "VLCMovie constructor" << endl;
}

ofxVLCMovie::~ofxVLCMovie(void) {
    cout << "VLCMovie destructor" << endl;
    cleanupVLC();
}

void ofxVLCMovie::init(int vlc_argc, char const* vlc_argv[]) {
    libvlc = libvlc_new(vlc_argc, vlc_argv);
    if (!libvlc) {
        const char* error = libvlc_errmsg();
        cout << error << endl;
        return;
    }

    if (ofStringTimesInString(filename, "://") == 1) {
        m = libvlc_media_new_location(libvlc, filename.c_str());
    }
    else {
        m = libvlc_media_new_path(libvlc, filename.c_str());
    }

    //libvlc_media_add_option(m, ":sout=#duplicate{dst=display,dst=std{access=file,mux=mp4,dst=xyz.mp4}");
    mp = libvlc_media_player_new_from_media(m);
    libvlc_media_parse(m);
    unsigned int x, y;
    if (libvlc_video_get_size(mp, 0, &x, &y) != -1) {
        videoWidth = x;
        videoHeight = y;
    }
    else {
        videoWidth = 1280;
        videoHeight = 720;
    }
    video_length_ms = libvlc_media_get_duration(m);
    cout << "Video size: (" << videoWidth << ", " << videoHeight << ")" << endl;
    cout << "Video length: " << video_length_ms << "(ms)" << endl;

    libvlc_video_set_callbacks(mp, lockStatic, unlockStatic, displayStatic, this);
    libvlc_video_set_format(mp, "RGBA", videoWidth, videoHeight, videoWidth * 4);

    eventManager = libvlc_media_player_event_manager(mp);
    libvlc_event_attach(eventManager, libvlc_MediaPlayerEndReached, vlcEventStatic, this);

    for (int i = 0; i < 2; i++) {
        image[i].allocate(videoWidth, videoHeight, OF_IMAGE_COLOR_ALPHA);
    }
    frontTexture = &frontImage->getTexture();

    isInitialized = true;
}

void ofxVLCMovie::cleanupVLC() {
    if (isInitialized)
    {
        libvlc_media_player_stop(mp);
        libvlc_media_player_release(mp);
    }
    libvlc_media_release(m);
    libvlc_release(libvlc);
}

void ofxVLCMovie::play() {
    if (isLooping) {
        libvlc_media_add_option(m, "input-repeat=-1");
    }
    else {
        libvlc_media_add_option(m, "input-repeat=0");
    }
    movieFinished = false;
    libvlc_media_player_play(mp);
}

void ofxVLCMovie::pause() {
    libvlc_media_player_pause(mp);
}

void ofxVLCMovie::stop() {
    if (!isInitialized) {
        return;
    }
    libvlc_media_player_stop(mp);
    movieFinished = false;
    firstFrameReady = false;
    setTimeMillis(0);
    updateTexture();
}

void ofxVLCMovie::setPosition(float position) {
    libvlc_media_player_set_position(mp, position);
}

void* ofxVLCMovie::lockStatic(void* data, void** p_pixels) {
    return ((ofxVLCMovie*)data)->lock(p_pixels);
}

void ofxVLCMovie::unlockStatic(void* data, void* id, void* const* p_pixels) {
    ((ofxVLCMovie*)data)->unlock(id, p_pixels);
}

void ofxVLCMovie::displayStatic(void* data, void* id) {
    ((ofxVLCMovie*)data)->display(id);
}

void ofxVLCMovie::vlcEventStatic(const libvlc_event_t* event, void* data) {
    ((ofxVLCMovie*)data)->vlcEvent(event);
}

void ofxVLCMovie::vlcEvent(const libvlc_event_t* event) {
    if (event->type == libvlc_MediaPlayerEndReached) {
        movieFinished = true;
    }
}

void* ofxVLCMovie::lock(void** p_pixels) {
    *p_pixels = backImage->getPixels().getData();
    return NULL;
}

void ofxVLCMovie::unlock(void* id, void* const* p_pixels) {
}

void ofxVLCMovie::display(void* id) {
    ofImage* tmp = backImage;
    backImage = frontImage;
    frontImage = tmp;
}

unsigned int ofxVLCMovie::getImageWidth() {
    return videoWidth;
}

unsigned int ofxVLCMovie::getImageHeight() {
    return videoHeight;
}

void ofxVLCMovie::updateTexture() {
    frontImage->update();
    frontTexture = &frontImage->getTexture();
    firstFrameReady = true;
}

ofTexture& ofxVLCMovie::getTexture() {
    return *frontTexture;
}

void ofxVLCMovie::setLoop(bool isLooping) {
    this->isLooping = isLooping;
}

bool ofxVLCMovie::isMovieFinished() {
    return movieFinished;
}

bool ofxVLCMovie::isPlaying() {
    return libvlc_media_player_is_playing(mp);
}

bool ofxVLCMovie::isFirstFrameReady() {
    return firstFrameReady;
}

bool ofxVLCMovie::getIsInitialized() {
    return isInitialized;
}

float ofxVLCMovie::getPosition() {
    return libvlc_media_player_get_position(mp);
}

libvlc_time_t ofxVLCMovie::getTimeMillis() {
    return libvlc_media_player_get_time(mp);
}

void ofxVLCMovie::setTimeMillis(libvlc_time_t ms) {
    libvlc_media_player_set_time(mp, ms);
}

float ofxVLCMovie::getFPS() {
    return fps;
}

void ofxVLCMovie::setFPS(float fps) {
    ofxVLCMovie::fps = fps;
}

float ofxVLCMovie::getDuration() {
    return video_length_ms;
}

void ofxVLCMovie::setFrame(int frame) {
    libvlc_time_t ms = 1000 * frame / fps;
    setTimeMillis(ms);
}

int ofxVLCMovie::getCurrentFrame() {
    libvlc_time_t ms = getTimeMillis();
    int frame = fps * ms / 1000;
    return frame;
}

int ofxVLCMovie::getTotalNumFrames() {
    return fps * video_length_ms / 1000;
}

void ofxVLCMovie::setVolume(int volume) {
    libvlc_audio_set_volume(mp, volume);
}

void ofxVLCMovie::toggleMute() {
    libvlc_audio_toggle_mute(mp);
}
