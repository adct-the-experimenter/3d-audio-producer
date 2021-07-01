#!/usr/bin/env bash

#install pre-requisites

#Ubuntu install the required packages
#sudo apt-get -y install build-essential debianutils automake gcc g++ make -dev libpugixml-dev libsdl2-dev
#wget -c "https://github.com/raysan5/raylib/releases/download/3.5.0/raylib-3.5.0_linux_amd64.tar.gz" 
#tar -xzf raylib-3.5.0_linux_amd64.tar.gz && cd raylib-3.5.0_linux_amd64 && cp libraylib.* ../ && cp libraylib.* /usr/local/lib
 

#script:

VERSION=1.0.1

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

ICON_FILE="3d-audio-producer.png" 

if [ -f ./AppDir/$ICON_FILE ]; then
	echo "$ICON_FILE already exists in AppDir."
else
	cp ../app_image_resources/$ICON_FILE ./AppDir
fi

DESKTOP_FILE="3d-audio-producer.desktop"

if [ -f ./AppDir/$DESKTOP_FILE ]; then
	echo "$DESKTOP_FILE already exists in AppDir."
else
	cp ../app_image_resources/$DESKTOP_FILE ./AppDir
fi

cmake ../.. -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -DDATAPATH=${HOME}/3d-audio-producer-data/resources && make -j$(nproc) && make install DESTDIR=AppDir

if [ -f "linuxdeploy-x86_64.AppImage" ]; then
	echo "linuxdeploy-x86_64.AppImage exists."
else
	wget -c "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage"
fi

chmod a+x linuxdeploy*.AppImage

./linuxdeploy*.AppImage --appdir AppDir --icon-file ./AppDir/$ICON_FILE --desktop-file ./AppDir/$DESKTOP_FILE  --output appimage

if [ -d "3d-audio-producer-$VERSION" ]; then
	echo "3d audio producer release directory container already exists."
else	
	mkdir 3d-audio-producer-$VERSION
fi

cp 3D*.AppImage 3d-audio-producer-$VERSION
cp -r ../../data ./3d-audio-producer-$VERSION/3d-audio-producer-data
rm ./3d-audio-producer-$VERSION/3d-audio-producer-data/resources/*.wav
cp ../app_image_resources/INSTALL ./3d-audio-producer-$VERSION
tar -czvf 3d-audio-producer-$VERSION.tar.gz ./3d-audio-producer-$VERSION
