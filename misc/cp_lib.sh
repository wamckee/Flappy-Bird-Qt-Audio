rm -rf Flappy-Bird-Qt-Audio.app/Contents/Framworks/$1.framework

mkdir Flappy-Bird-Qt-Audio.app/Contents/Frameworks/$1.framework
mkdir Flappy-Bird-Qt-Audio.app/Contents/Frameworks/$1.framework/Resources

cp -R $QT/lib/$1.framework/Versions/A/$1 Flappy-Bird-Qt-Audio.app/Contents/Frameworks/$1.framework
cp $QT/lib/$1.framework/Resources/Info.plist Flappy-Bird-Qt-Audio.app/Contents/Frameworks/$1.framework/Resources

install_name_tool -id @executable_path/../Frameworks/$1.framework/$1 Flappy-Bird-Qt-Audio.app/Contents/Frameworks/$1.framework/$1

misc/ren_lib.sh QtCore Frameworks/$1.framework/$1

