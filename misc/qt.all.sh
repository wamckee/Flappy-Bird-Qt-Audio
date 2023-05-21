export QT=/usr/local/Qt-6.4.2

echo Flappy-Bird-Qt-Audio executable
mkdir Flappy-Bird-Qt-Audio.app/Contents/Frameworks
misc/ren_lib.sh QtWidgets MacOS/Flappy-Bird-Qt-Audio
misc/ren_lib.sh QtGui MacOS/Flappy-Bird-Qt-Audio
misc/ren_lib.sh QtCore MacOS/Flappy-Bird-Qt-Audio
misc/ren_lib.sh QtNetwork MacOS/Flappy-Bird-Qt-Audio
misc/ren_lib.sh QtOpenGL MacOS/Flappy-Bird-Qt-Audio
misc/ren_lib.sh QtOpenGLWidgets MacOS/Flappy-Bird-Qt-Audio
misc/ren_lib.sh QtMultimedia MacOS/Flappy-Bird-Qt-Audio

echo QtCore
misc/cp_lib.sh QtCore

echo QtWidgets
misc/cp_lib.sh QtWidgets
misc/ren_lib.sh QtGui Frameworks/QtWidgets.framework/QtWidgets

echo QtGui
misc/cp_lib.sh QtGui
misc/cp_lib.sh QtDBus
misc/ren_lib.sh QtDBus Frameworks/QtGui.framework/QtGui

echo QtNetwork
misc/cp_lib.sh QtNetwork

echo QtOpenGL
misc/cp_lib.sh QtOpenGL
misc/ren_lib.sh QtGui Frameworks/QtOpenGL.framework/QtOpenGL

echo QtOpenGLWidgets
misc/cp_lib.sh QtOpenGLWidgets
misc/ren_lib.sh QtOpenGL Frameworks/QtOpenGLWidgets.framework/QtOpenGLWidgets
misc/ren_lib.sh QtGui Frameworks/QtOpenGLWidgets.framework/QtOpenGLWidgets
misc/ren_lib.sh QtWidgets Frameworks/QtOpenGLWidgets.framework/QtOpenGLWidgets

echo QtMultimedia
misc/cp_lib.sh QtMultimedia
misc/ren_lib.sh QtGui Frameworks/QtMultimedia.framework/QtMultimedia
misc/ren_lib.sh QtNetwork Frameworks/QtMultimedia.framework/QtMultimedia

echo plugins
mkdir Flappy-Bird-Qt-Audio.app/Contents/plugins
cp -R $QT/plugins/styles Flappy-Bird-Qt-Audio.app/Contents/plugins/styles
misc/ren_lib.sh QtCore plugins/styles/libqmacstyle.dylib
misc/ren_lib.sh QtGui plugins/styles/libqmacstyle.dylib
misc/ren_lib.sh QtWidgets plugins/styles/libqmacstyle.dylib
mkdir Flappy-Bird-Qt-Audio.app/Contents/plugins/platforms
cp $QT/plugins/platforms/libqcocoa.dylib Flappy-Bird-Qt-Audio.app/Contents/plugins/platforms/libqcocoa.dylib
misc/ren_lib.sh QtCore plugins/platforms/libqcocoa.dylib
misc/ren_lib.sh QtGui plugins/platforms/libqcocoa.dylib
