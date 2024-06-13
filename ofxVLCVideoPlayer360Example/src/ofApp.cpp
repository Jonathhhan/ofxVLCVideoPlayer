#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    ofDisableArbTex();
    ofSetWindowTitle("ofxVLCVideoPlayer360Example");
    ofSetFrameRate(60);
    cam.setPosition(0,0,0);
    cam.setAutoDistance(false);
    char const* vlc_argv[] = { "--vout=opengl", "--video-filter=transform", "--transform-type=hflip" };
    int vlc_argc = sizeof(vlc_argv) / sizeof(*vlc_argv);
    player.loadMovie("data/360_vr_master_series___free_download___crystal_shower_falls (1080p).mp4", vlc_argc, vlc_argv);
    player.setLoop(true);
    player.play();
}

//--------------------------------------------------------------
void ofApp::update() {
    player.update();
}

//--------------------------------------------------------------
void ofApp::draw() {
    cam.begin();
    player.getTexture().bind();
    sphere.draw();
    player.getTexture().unbind();
    cam.end();
    ofDrawBitmapString("FPS: " + ofToString(ofGetFrameRate()), 20, 20);
    ofDrawBitmapString("Second: " + ofToString(player.getPosition() * player.getDuration() / 1000), 20, 40);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    player.pause();
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {
}
