#include "VLCMovie.h"
#include <cassert>
#include <vlc/libvlc_media.h>
#include <vlc/libvlc_media_player.h>

VLCMovie::VLCMovie(string filename)
    : mediaType(FILE), filename(filename), frontImage(&image[1]), backImage(&image[0]),
    isFliped(false), isLooping(true), movieFinished(false), isInitialized(false),
    isVLCInitialized(false), isThumbnailOK(false), frontTexture(NULL), tryUpdate(false) {
    cout << "VLCMovie constructor" << endl;
}

VLCMovie::VLCMovie(void* opaqueMedia, openCallback openCb, closeCallback closeCb,
    readCallback readCb, seekCallback seekCb)
    : mediaType(CALLBACKS), opaqueMedia(opaqueMedia), openCb(openCb), closeCb(closeCb),
    readCb(readCb), seekCb(seekCb), frontImage(&image[1]), backImage(&image[0]),
    isFliped(false), isLooping(true), movieFinished(false), isInitialized(false),
    isVLCInitialized(false), isThumbnailOK(false), frontTexture(NULL), tryUpdate(false) {
    cout << "VLCMovie constructor (callbacks)" << endl;
}

VLCMovie::~VLCMovie(void)
{
    cout << "VLCMovie destructor" << endl;
    cleanupVLC();
}

void VLCMovie::init() {
    if (isInitialized) {
        return;
    }
    initializeVLC();
    if (!isVLCInitialized) {
        return;
    }
    needPostInit = true;
}

void VLCMovie::postInit() {
    for (int i = 0; i < 2; i++) {
        image[i].allocate(videoWidth, videoHeight, OF_IMAGE_COLOR_ALPHA);
    }

    frontTexture = &frontImage->getTextureReference();

    needPostInit = false;
    isInitialized = true;
}

void VLCMovie::loadMedia() {
    if (mediaType == FILE) {
        if ("http" == filename.substr(0, 4)) {
            m = libvlc_media_new_location(libvlc, filename.c_str());
        }
        else {
            m = libvlc_media_new_path(libvlc, filename.c_str());
        }
    }
    else if (mediaType == CALLBACKS) {
        m = libvlc_media_new_callbacks(libvlc, openCb, readCb, seekCb, closeCb, opaqueMedia);
    }
}

void VLCMovie::initializeVLC() {
    std::unique_lock<std::mutex> lock{ playerLock };

    cout << "init libvlc" << endl;
    char const* vlc_argv[] = {
        "--no-osd"
    };

    int vlc_argc = sizeof(vlc_argv) / sizeof(*vlc_argv);
    libvlc = libvlc_new(vlc_argc, vlc_argv);
    cout << "libvlc: " << libvlc << endl;
    if (!libvlc) {
        const char* error = libvlc_errmsg();
        cout << error << endl;
        return;
    }

    loadMedia();
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

    libvlc_audio_output_set(mp, "aout_directx");
    libvlc_video_set_callbacks(mp, lockStatic, unlockStatic, displayStatic, this);
    libvlc_video_set_format(mp, "RGBA", videoWidth, videoHeight, videoWidth * 4);

    eventManager = libvlc_media_player_event_manager(mp);
    libvlc_event_attach(eventManager, libvlc_MediaPlayerEndReached, vlcEventStatic, this);

    isVLCInitialized = true;
}

void VLCMovie::cleanupVLC() {
    std::unique_lock<std::mutex> lock{ playerLock };

    if (isInitialized)
    {
        libvlc_media_player_stop(mp);
        libvlc_media_player_release(mp);
    }
    libvlc_media_release(m);
    libvlc_release(libvlc);
}

void VLCMovie::play() {
    std::unique_lock<std::mutex> lock{ playerLock };

    if (isLooping) {
        libvlc_media_add_option(m, "input-repeat=65545");
    }
    else {
        libvlc_media_add_option(m, "input-repeat=0");
    }
    movieFinished = false;

    libvlc_media_player_play(mp);

}

void VLCMovie::pause()
{
    std::unique_lock<std::mutex> lock{ playerLock };
    libvlc_media_player_pause(mp);
}

void VLCMovie::rewind() {
    std::unique_lock<std::mutex> lock{ playerLock };
    libvlc_media_player_set_position(mp, 0);
}

void VLCMovie::stop() {
    std::unique_lock<std::mutex> lock{ playerLock };

    if (!isInitialized) {
        return;
    }

    libvlc_media_player_stop(mp);
    movieFinished = false;
    firstFrameReady = false;
    setTimeMillis(0);
    updateTexture();
}

void VLCMovie::seek(float position) {
    std::unique_lock<std::mutex> lock{ playerLock };

    libvlc_media_player_set_position(mp, position);
}

void* VLCMovie::lockStatic(void* data, void** p_pixels) {
    return ((VLCMovie*)data)->lock(p_pixels);
}

void VLCMovie::unlockStatic(void* data, void* id, void* const* p_pixels) {
    ((VLCMovie*)data)->unlock(id, p_pixels);
}

void VLCMovie::displayStatic(void* data, void* id) {
    ((VLCMovie*)data)->display(id);

}

void VLCMovie::vlcEventStatic(const libvlc_event_t* event, void* data) {
    ((VLCMovie*)data)->vlcEvent(event);
}

void VLCMovie::vlcEvent(const libvlc_event_t* event) {
    if (event->type == libvlc_MediaPlayerEndReached) {
        movieFinished = true;
    }
}

void* VLCMovie::lock(void** p_pixels) {
    while (tryUpdate) {
#ifdef WIN32
        Sleep(10);
#else
        usleep(10 * 1000);
#endif
    }

    while (!isInitialized)
    {
#ifdef WIN32
        Sleep(10);
#else
        usleep(10 * 1000);
#endif
    }

    *p_pixels = backImage->getPixels().getData();
    return NULL;
}

void VLCMovie::unlock(void* id, void* const* p_pixels) {
}

void VLCMovie::display(void* id) {
    while (tryUpdate) {
#ifdef WIN32
        Sleep(10);
#else
        usleep(10 * 1000);
#endif
    }

    if (imageFlipMutex.try_lock()) {
        ofImage* tmp = backImage;
        backImage = frontImage;
        frontImage = tmp;
        isFliped = true;
        imageFlipMutex.unlock();
    }
}

unsigned int VLCMovie::getImageWidth() {
    return videoWidth;
}

unsigned int VLCMovie::getImageHeight() {
    return videoHeight;
}

void VLCMovie::updateTexture() {
    if (!isFliped) return;
    tryUpdate = true;
    imageFlipMutex.lock();
    frontImage->update();
    frontTexture = &frontImage->getTextureReference();
    isFliped = false;
    imageFlipMutex.unlock();
    tryUpdate = false;

    firstFrameReady = true;
}

ofTexture& VLCMovie::getTexture() {
    return *frontTexture;
}

void VLCMovie::setLoop(bool isLooping) {
    this->isLooping = isLooping;
}

bool VLCMovie::isMovieFinished() {
    return movieFinished;
}

bool VLCMovie::isPlaying() {
    return libvlc_media_player_is_playing(mp);
}

bool VLCMovie::isFirstFrameReady()
{
    return firstFrameReady;
}

bool VLCMovie::isRotated()
{
    libvlc_media_track_t** tracks;
    unsigned count = libvlc_media_tracks_get(m, &tracks);
    if (!count) {
        return false;
    }
    for (size_t trackIdx = 0; trackIdx < count; ++trackIdx)
    {
        if (tracks[trackIdx]->i_type == libvlc_track_video)
        {
            libvlc_video_orient_t orientation = tracks[trackIdx]->video->i_orientation;
            libvlc_media_tracks_release(tracks, count);
            return orientation == libvlc_video_orient_left_bottom
                || orientation == libvlc_video_orient_right_top;
        }
    }
    libvlc_media_tracks_release(tracks, count);
    return false;
}

bool VLCMovie::getIsInitialized() {
    return isInitialized;
}

bool VLCMovie::getNeedsPostInit()
{
    return needPostInit;
}

float VLCMovie::getPosition() {
    return libvlc_media_player_get_position(mp);
}

libvlc_time_t VLCMovie::getTimeMillis() {
    return libvlc_media_player_get_time(mp);
}

void VLCMovie::setTimeMillis(libvlc_time_t ms) {
    libvlc_media_player_set_time(mp, ms);
}

float VLCMovie::getFPS() {
    return fps;
}

void VLCMovie::setFPS(float fps) {
    VLCMovie::fps = fps;
}

float VLCMovie::getDuration() {
    return video_length_ms;
}

void VLCMovie::setFrame(int frame) {
    libvlc_time_t ms = 1000 * frame / fps;
    setTimeMillis(ms);
}

int VLCMovie::getCurrentFrame() {
    libvlc_time_t ms = getTimeMillis();
    int frame = fps * ms / 1000;
    return frame;
}

int VLCMovie::getTotalNumFrames() {
    return fps * video_length_ms / 1000;
}

void VLCMovie::setVolume(int volume) {
    std::unique_lock<std::mutex> lock{ playerLock };
    libvlc_audio_set_volume(mp, volume);
}

void VLCMovie::toggleMute() {
    std::unique_lock<std::mutex> lock{ playerLock };
    libvlc_audio_toggle_mute(mp);
}