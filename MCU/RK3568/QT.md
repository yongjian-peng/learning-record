#### 执行命令

```
/home/yxw/myproject/rk356x_linux5.1/buildroot/output/rockchip_rk3568/host/bin/qmake test.pro 
make

在屏幕上创建应用
	图标显示的方式：配置文件加入图标以及执行程序
	
cat /etc/xdg/weston/weston.ini.d/06-mpv-player.ini

```





#### 编译配置

```
BR2_PACKAGE_QT5IMAGEFORMATS=y

BR2_PACKAGE_QT5BASE_SQL=y
#BR2_PACKAGE_QT5BASE_SQLITE_NONE is not set
BR2_PACKAGE_QT5BASE_SQLITE_SYSTEM=y

BR2_PACKAGE_QT5BASE_PNG=y
BR2_PACKAGE_QT5BASE_EGLFS=y

BR2_PACKAGE_OPENJPEG=y
BR2_PACKAGE_QT5BASE_JPEG=y
BR2_PACKAGE_LIBJPEG=y
BR2_PACKAGE_QT5BASE_MYSQL=y
BR2_PACKAGE_QT5BASE_SQLITE_QT=y
BR2_PACKAGE_QT5BASE_GIF=y
BR2_PACKAGE_QT5BASE_SYSLOG=y

```



#### 编译报错

```
2025-11-10T19:46:07 ERROR: Feature 'system-pcre2' was enabled, but the pre-condition 'libs.pcre2' failed.
2025-11-10T19:46:07 make[1]: *** [package/pkg-generic.mk:306: /home/yxw/myproject/rk356x_linux5.1/buildroot/output/rockchip_rk3568/build/qt5base-e44097b63d17ba3178a637df7fac51ddc51cb48b/.stamp_configured] Error 3
2025-11-10T19:46:07 make: *** [Makefile:114: _all] Error 2
2025-11-10T19:46:07 make: Leaving directory '/home/yxw/myproject/rk356x_linux5.1/buildroot'
Please check details in /home/yxw/myproject/rk356x_linux5.1/output/sessions/2025-11-10_19-38-49/br-rockchip_rk3568_2025-11-10_19-38-50.log
ERROR: Running /home/yxw/myproject/rk356x_linux5.1/device/rockchip/common/build-hooks/30-rootfs.sh - build_buildroot failed!
ERROR: exit code 1 from line 16:
    "$RK_SCRIPTS_DIR/mk-buildroot.sh" $RK_BUILDROOT_CFG "$IMAGE_DIR"
ERROR: call stack:
    30-rootfs.sh: build_buildroot(16)
    30-rootfs.sh: build_hook(262)
    build-helper: try_func(63)
    build-helper: try_hook(95)
    build-helper: source(175)
    30-rootfs.sh: main(286)
ERROR: Running /home/yxw/myproject/rk356x_linux5.1/device/rockchip/common/build-hooks/30-rootfs.sh - try_func build_hook buildroot failed!
ERROR: exit code 1 from line 67:
    build_hook
ERROR: call stack:
    build-helper: try_func(67)
    build-helper: try_hook(95)
    build-helper: source(175)
    30-rootfs.sh: main(286)
ERROR: Running ./build.sh - run_hooks build buildroot failed!
ERROR: exit code 1 from line 256:
    /home/yxw/myproject/rk356x_linux5.1/device/rockchip/common/build-hooks/30-rootfs.sh build buildroot
ERROR: call stack:
    build.sh: run_hooks(256)
    build.sh: run_build_hooks(275)
    build.sh: main(644)
    build.sh: main(655)
ERROR: Running ./build.sh - run_build_hooks build buildroot failed!
ERROR: exit code 1 from line 278:
    build
ERROR: call stack:
    build.sh: run_build_hooks(278)
    build.sh: main(644)
    build.sh: main(655)
```

