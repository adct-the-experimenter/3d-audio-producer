#!/usr/bin/env bash

#install pre-requisites

#Ubuntu install the required packages
#sudo apt-get -y install build-essential debianutils automake gcc g++ make -dev libpugixml-dev libsdl2-dev
#wget -c "https://github.com/raysan5/raylib/releases/download/3.5.0/raylib-3.5.0_linux_amd64.tar.gz" 
#tar -xzf raylib-3.5.0_linux_amd64.tar.gz && cd raylib-3.5.0_linux_amd64 && cp libraylib.* ../ && cp libraylib.* /usr/local/lib
 

#script:

VERSION=1.0

if [ -d "build" ]; then
	echo "build directory exists."
else
	mkdir build
fi

cd build

if [ -d "AppDir" ]; then
	echo "AppDir exists."
else
	mkdir AppDir
fi

ICON_FILE="thief-fighters.png" 

if [ -f ./AppDir/$ICON_FILE ]; then
	echo "$ICON_FILE already exists in AppDir."
else
	cp ../app_image_resources/thief-fighters.png ./AppDir
fi

DESKTOP_FILE="thief-fighters.desktop"

if [ -f ./AppDir/$DESKTOP_FILE ]; then
	echo "$DESKTOP_FILE already exists in AppDir."
else
	cp ../app_image_resources/thief-fighters.desktop ./AppDir
fi


cmake ../.. -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -DDATAPATH=${HOME}/thief-fighters-data && make -j$(nproc) && make install DESTDIR=AppDir

if [ -f "linuxdeploy-x86_64.AppImage" ]; then
	echo "linuxdeploy-x86_64.AppImage exists."
else
	wget -c "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage"
fi

chmod a+x linuxdeploy*.AppImage

./linuxdeploy*.AppImage --appdir AppDir --icon-file ./AppDir/thief-fighters.png --desktop-file ./AppDir/thief-fighters.desktop  --output appimage

if [ -d "thief-fighters-$VERSION" ]; then
	echo "thief fighters release directory container already exists."
else	
	mkdir thief-fighters-$VERSION
fi

cp Thief*.AppImage thief-fighters-$VERSION
cp -r ../../data ./thief-fighters-$VERSION/thief-fighters-data
cp ../app_image_resources/INSTALL ./thief-fighters-$VERSION
tar -czvf thief-fighters-$VERSION.tar.gz ./thief-fighters-$VERSION
