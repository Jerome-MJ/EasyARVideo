# EasyARVideo
关于EasyAR本地Android项目，增加了seekbar的操作显示，以及暂停开始，等操作。大家有什么更多地建议也可以提
# 效果如下
--
![image](https://github.com/Jerome-MJ/EasyARVideo/raw/master/screenshots/s2.jpg)
        

# 主要修改：
### 1.修改EasyARSDKSamples/package/include下的player.hpp文件,确定你的这块区域一致

<pre>
<code>

    VideoPlayer();
    virtual ~VideoPlayer();

    //! should be called before open
    void setRenderTexture(int texture);
    //! only have effect when called before open
    void setVideoType(VideoType videoType);

    void open(const char* path, StorageType storageType, VideoPlayerCallBack* callback = 0);
    void close();

    bool play();
    bool stop();
    bool pause();
    void updateFrame();
    int duration();
    int currentPosition();
    bool seek(int position);
    Vec2I size();
    float volume();
    bool setVolume(float volume);
</code>
</pre>
### 2.修改jni中得ar.cc，ar.hpp，helloarvideo.cc文件，并在MainActivity中添加对应的方法
### 3.在main目录下执行ndk-build，生成.so文件