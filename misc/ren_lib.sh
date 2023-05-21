install_name_tool -change @rpath/$1.framework/Versions/A/$1 @executable_path/../Frameworks/$1.framework/$1 Flappy-Bird-Qt-Audio.app/Contents/$2
