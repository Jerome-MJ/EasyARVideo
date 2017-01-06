/**
* Copyright (c) 2015-2016 VisionStar Information Technology (Shanghai) Co., Ltd. All Rights Reserved.
* EasyAR is the registered trademark or trademark of VisionStar Information Technology (Shanghai) Co., Ltd in China
* and other countries for the augmented reality technology developed by VisionStar Information Technology (Shanghai) Co., Ltd.
*/

#include "ar.hpp"
#include "renderer.hpp"
#include <jni.h>
#include <GLES2/gl2.h>

#define JNIFUNCTION_NATIVE(sig) Java_cn_easyar_samples_helloarvideo_MainActivity_##sig

extern "C" {
    JNIEXPORT jboolean JNICALL JNIFUNCTION_NATIVE(nativeInit(JNIEnv* env, jobject object));
    JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeDestory(JNIEnv* env, jobject object));
    JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeInitGL(JNIEnv* env, jobject object));
    JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeResizeGL(JNIEnv* env, jobject object, jint w, jint h));
    JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeRender(JNIEnv* env, jobject obj));
    JNIEXPORT int JNICALL JNIFUNCTION_NATIVE(nativeGetDuration(JNIEnv* env, jobject obj));
    JNIEXPORT int JNICALL JNIFUNCTION_NATIVE(nativeGetCurrentPosition(JNIEnv* env, jobject obj));
    JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeVideoSeek(JNIEnv* env, jobject obj,jint pos));
    JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeVideoPlay(JNIEnv* env, jobject obj));
    JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeVideoPause(JNIEnv* env, jobject obj));
    JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeRotationChange(JNIEnv* env, jobject obj, jboolean portrait));
};

namespace EasyAR {
namespace samples {

class HelloARVideo : public AR
{
public:
    HelloARVideo();
    ~HelloARVideo();
    virtual void initGL();
    virtual void resizeGL(int width, int height);
    virtual void render();
    virtual bool clear();
    virtual int getDuration();
    virtual void seek(int pos);
    virtual int getCurrentPosition();
    virtual void onPlay();
    virtual void onPause();

private:
    Vec2I view_size;
    VideoRenderer* renderer[3];
    int tracked_target;
    int active_target;
    int texid[3];
    ARVideo* video;
    VideoRenderer* video_renderer;
};

HelloARVideo::HelloARVideo()
{
    view_size[0] = -1;
    tracked_target = 0;
    active_target = 0;
    for(int i = 0; i < 3; ++i) {
        texid[i] = 0;
        renderer[i] = new VideoRenderer;
    }
    video = NULL;
    video_renderer = NULL;
}

HelloARVideo::~HelloARVideo()
{
    for(int i = 0; i < 3; ++i) {
        delete renderer[i];
    }
}

void HelloARVideo::initGL()
{
    augmenter_ = Augmenter();
    augmenter_.attachCamera(camera_);
    for(int i = 0; i < 3; ++i) {
        renderer[i]->init();
        texid[i] = renderer[i]->texId();
    }
}

void HelloARVideo::resizeGL(int width, int height)
{
    view_size = Vec2I(width, height);
}

void HelloARVideo::render()
{
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Frame frame = augmenter_.newFrame();
    if(view_size[0] > 0){
        AR::resizeGL(view_size[0], view_size[1]);
        if(camera_ && camera_.isOpened())
            view_size[0] = -1;
    }
    augmenter_.setViewPort(viewport_);
    augmenter_.drawVideoBackground();
    glViewport(viewport_[0], viewport_[1], viewport_[2], viewport_[3]);

    AugmentedTarget::Status status = frame.targets()[0].status();
    if(status == AugmentedTarget::kTargetStatusTracked){
        int id = frame.targets()[0].target().id();
        if(active_target && active_target != id) {
            video->onLost();
            delete video;
            video = NULL;
            tracked_target = 0;
            active_target = 0;
        }
        if (!tracked_target) {
            if (video == NULL) {
                if(frame.targets()[0].target().name() == std::string("argame") && texid[0]) {
                    video = new ARVideo;
                    video->openVideoFile("video.mp4", texid[0]);
                    video_renderer = renderer[0];
                }
                else if(frame.targets()[0].target().name() == std::string("namecard") && texid[1]) {
                    video = new ARVideo;
                    video->openTransparentVideoFile("transparentvideo.mp4", texid[1]);
                    video_renderer = renderer[1];
                }
                else if(frame.targets()[0].target().name() == std::string("idback") && texid[2]) {
                    video = new ARVideo;
                    video->openStreamingVideo("http://7xl1ve.com5.z0.glb.clouddn.com/sdkvideo/EasyARSDKShow201520.mp4", texid[2]);
                    video_renderer = renderer[2];
                }
            }
            if (video) {
                video->onFound();
                tracked_target = id;
                active_target = id;
            }
        }
        Matrix44F projectionMatrix = getProjectionGL(camera_.cameraCalibration(), 0.2f, 500.f);
        Matrix44F cameraview = getPoseGL(frame.targets()[0].pose());
        ImageTarget target = frame.targets()[0].target().cast_dynamic<ImageTarget>();
        if(tracked_target) {
            video->update();
            video_renderer->render(projectionMatrix, cameraview, target.size());
        }
    } else {
        if (tracked_target) {
            video->onLost();
            tracked_target = 0;
        }
    }
}

bool HelloARVideo::clear()
{
    AR::clear();
    if(video){
        delete video;
        video = NULL;
        tracked_target = 0;
        active_target = 0;
    }
    return true;
}

int HelloARVideo::getDuration(){
//    AR::getDuration();
    if(video){
        return video->duration();
    }else{
        return 0;
    }
}
    int HelloARVideo::getCurrentPosition(){
//        AR::getDuration();
        if (video) {
            return video->currentPosition();
        }else{
            return 0;
        }
    }
    void HelloARVideo::onPause(){
//        AR::onPause();
    if (video)
    video->pause();
    }

    void HelloARVideo::onPlay(){
//        AR::onPlay();
    if(video){
        video->play();
    }
//if (prepared_)
//player_.play();
    }

    void HelloARVideo::seek(int pos){
        if(video){
            video->seek(pos);
        }
    }

}
}
EasyAR::samples::HelloARVideo ar;

JNIEXPORT jboolean JNICALL JNIFUNCTION_NATIVE(nativeInit(JNIEnv*, jobject))
{
    bool status = ar.initCamera();
    ar.loadAllFromJsonFile("targets.json");
    ar.loadFromImage("namecard.jpg");
    status &= ar.start();
    return status;
}

JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeDestory(JNIEnv*, jobject))
{
    ar.clear();
}
JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeVideoPlay(JNIEnv*, jobject))
{
    ar.onPlay();
}
JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeVideoPause(JNIEnv*, jobject))
{
    ar.onPause();
}
JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeVideoSeek(JNIEnv*, jobject,jint pos))
{
    ar.seek(pos);
}

JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeInitGL(JNIEnv*, jobject))
{
    ar.initGL();
}

JNIEXPORT int JNICALL JNIFUNCTION_NATIVE(nativeGetDuration(JNIEnv*, jobject))
{
    return ar.getDuration();
}

JNIEXPORT int JNICALL JNIFUNCTION_NATIVE(nativeGetCurrentPosition(JNIEnv*, jobject))
{
    return ar.getCurrentPosition();
}

JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeResizeGL(JNIEnv*, jobject, jint w, jint h))
{
    ar.resizeGL(w, h);
}

JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeRender(JNIEnv*, jobject))
{
    ar.render();
}

JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeRotationChange(JNIEnv*, jobject, jboolean portrait))
{
    ar.setPortrait(portrait);
}
