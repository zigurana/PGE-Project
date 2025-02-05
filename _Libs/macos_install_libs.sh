#/bin/bash

if [[ "$1" == "" ]]; then
 echo "Please define name of application!"
 exit 1
fi

TARGET_APP=$1

bak=~+

function abspath()
{
case "${1}" in
[./]*)
echo "$(cd ${1%/*}; pwd)/${1##*/}"
;;
*)
echo "${PWD}/${1}"
;;
esac
}

#=============Detect directory that contains script=====================
SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
SCRDIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
SOURCE="$(abspath "$SOURCE")"
[[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
SCRDIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
#=======================================================================

SCRIPT=$(abspath "$0")
SCRIPTPATH=$(dirname $SCRIPT)

SCRP=$(abspath $SCRIPTPATH"/../_Libs/_builds/macos/lib/")
bak2=$PWD

SCRP_Libs=$(abspath $SCRIPTPATH"/../_Libs/_builds/macos/lib/")
#cd $SCRIPTPATH/_sources
#SCRP_SrcD=$(abspath $PWD"/../_builds/macos/lib/")
#cd $bak2

curd="$SCRDIR/../"
CONFIGURATION_BUILD_DIR=$SCRDIR'/../bin'

if [[ "$curd" == "" ]]; then
   exit 1
fi

echo "copy libraries..."
if [ ! -d $curd"/bin/_Libs" ]; then
    mkdir -p $curd"/bin/_Libs"
    if [ -d $curd"/bin/_Libs/SDL2.framework" ]; then
        rm -Rf $curd"/bin/_Libs/SDL2.framework"
    fi
    #if [ -d $curd"/bin/_Libs/SDL2_image.framework" ]; then
    #    rm -Rf $curd"/bin/_Libs/SDL2_image.framework"
    #fi
    cp -Rfa "$SCRDIR/_builds/macos/frameworks/SDL2.framework" $curd"/bin/_Libs"
    #cp -Rfa "$SCRDIR/_builds/macos/frameworks/SDL2_image.framework" $curd"/bin/_Libs"
    cp -a $SCRDIR/_builds/macos/lib/*.dylib $curd"/bin/_Libs"
fi

EXECUTABLE_PATH="$TARGET_APP.app/Contents/MacOS/$TARGET_APP"

function relocateLibraryInCurrentApp() {
  install_name_tool -change $1$2 "@executable_path/../../../_Libs/$2" "$CONFIGURATION_BUILD_DIR/$EXECUTABLE_PATH"
}

function relocateLibraryInCurrentLib() {
  #echo $1$2
  install_name_tool -change $1$2 "@loader_path/$2" "$CONFIGURATION_BUILD_DIR/$EXECUTABLE_PATH"
}

fetchPathsForApp()
{
FILES=$SCRIPTPATH"/../_Libs/_builds/macos/lib/*.dylib"
for f in $FILES
do
filename="${f##*/}"
#echo "Processing $filename file..."
relocateLibraryInCurrentApp "" $filename #note the space
done
}

fetchPathsForLib()
{
FILES=$SCRIPTPATH"/../bin/_Libs/*.dylib"
SCRP=$(abspath $SCRIPTPATH"/../_Libs/_builds/macos/lib/")
for f in $FILES
do
filename="${f##*/}"
#echo "Processing $filename file..."
relocateLibraryInCurrentLib "/lib/" $filename #note the space
relocateLibraryInCurrentLib $SCRP_Libs $filename #note the space
#relocateLibraryInCurrentLib $SCRP_SrcD $filename #note the space
#echo $SCRP_Libs$filename
#echo $SCRP_SrcD$filename
done
}

echo "Installing libs for $TARGET_APP..."
fetchPathsForApp # Fetch for executable
relocateLibraryInCurrentApp "@rpath/" SDL2.framework/Versions/A/SDL2 #note the space
relocateLibraryInCurrentApp "@rpath/" SDL2_image.framework/Versions/A/SDL2_image #note the space

EXECUTABLE_PATH="_Libs/libSDL2_mixer_ext.dylib"
fetchPathsForLib #fetch for our dylib
#EXECUTABLE_PATH="_Libs/libSDL2_mixer_ext.1.dylib"
#fetchPathsForLib #fetch for our dylib
#EXECUTABLE_PATH="_Libs/libSDL2_mixer_ext.1.0.dylib"
#fetchPathsForLib #fetch for our dylib
#EXECUTABLE_PATH="_Libs/libSDL2_mixer_ext.1.0.0.dylib"
#fetchPathsForLib #fetch for our dylib
#EXECUTABLE_PATH="_Libs/libogg.dylib"
#fetchPathsForLib #fetch for our dylib
#EXECUTABLE_PATH="_Libs/libvorbis.dylib"
#fetchPathsForLib #fetch for our dylib
#EXECUTABLE_PATH="_Libs/libvorbisenc.dylib"
#fetchPathsForLib #fetch for our dylib
#EXECUTABLE_PATH="_Libs/libvorbisfile.dylib"
#fetchPathsForLib #fetch for our dylib
#EXECUTABLE_PATH="_Libs/libFLAC.dylib"
#fetchPathsForLib #fetch for our dylib
#EXECUTABLE_PATH="_Libs/libFLAC++.dylib"
#fetchPathsForLib #fetch for our dylib
#EXECUTABLE_PATH="_Libs/libmad.dylib"
#fetchPathsForLib #fetch for our dylib
#EXECUTABLE_PATH="_Libs/libmodplug.dylib"
#fetchPathsForLib #fetch for our dylib
#EXECUTABLE_PATH="_Libs/libz.dylib"
#fetchPathsForLib #fetch for our dylib
#EXECUTABLE_PATH="_Libs/libz.1.dylib"
#fetchPathsForLib #fetch for our dylib
#EXECUTABLE_PATH="_Libs/libz.1.2.8.dylib"
#fetchPathsForLib #fetch for our dylib
#EXECUTABLE_PATH="_Libs/libpng.dylib"
#fetchPathsForLib #fetch for our dylib
#EXECUTABLE_PATH="_Libs/libpng16.dylib"
#fetchPathsForLib #fetch for our dylib
#EXECUTABLE_PATH="_Libs/libpng16.16.dylib"
#fetchPathsForLib #fetch for our dylib
#EXECUTABLE_PATH="_Libs/libpng16.16.20.0.dylib"
#fetchPathsForLib #fetch for our dylib
#EXECUTABLE_PATH="_Libs/libFLAC.8.dylib"
#fetchPathsForLib #fetch for our dylib
#EXECUTABLE_PATH="_Libs/libvorbisfile.3.dylib"
#fetchPathsForLib #fetch for our dylib
#EXECUTABLE_PATH="_Libs/libvorbis.0.dylib"
#fetchPathsForLib #fetch for our dylib
#EXECUTABLE_PATH="_Libs/libvorbisenc.2.dylib"
#fetchPathsForLib #fetch for our dylib
#EXECUTABLE_PATH="_Libs/libFLAC++.6.dylib"
#fetchPathsForLib #fetch for our dylib

cd $bak
#if [[ $1 != "no-pause" ]]; then read -n 1; fi
exit 0

