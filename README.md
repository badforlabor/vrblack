# vrblack
### 简介

- UE4 plugin. make vr-scene black when open level. 
- 在OpenLevel的过程中，让VR的世界是黑掉的，而不是用SteamVr默认的场景。
- 工程中，test3为测试工程。vrblack为插件。
- SimulateLongLoad组件，是用来模拟阻塞的，其实内部就是调用了Sleep函数而已。
- 用法很简单，在调用OpenLevel之前，调用BP工具函数BeforeOpenLevel即可。



### 思路

​	UE4的渲染是多线程的，在OpenLevel的过程中，主线程会阻塞住，但是渲染线程不会阻塞。所以，可以在渲染线程每帧tick的时候，直接操作SteamVr的Compositor，让其绘制左右双眼的内容。

​	为了简单，我只绘制了一张黑色的贴图。如果更深层次，可以绘制电影之类的。这个，就可以参考UE4提供的Splash功能了。（入口函数：UStereoLayerFunctionLibrary::SetSplashScreen）



### 注意

- UE4.20中，测试了一下UE4自带的SetSplashScreen，有bug，会崩溃。需要手动改下UE4的源码才能解决。
