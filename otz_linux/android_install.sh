#/bin/sh

ANDROID_PATH=/home/pub/ov_android/android/
PATCH_DIR=`pwd` 

cp -R ../tzone_sdk $ANDROID_PATH/external

mv $ANDROID_PATH/boot-wrapper/boot.S       $ANDROID_PATH/boot-wrapper/boot.S.orig
mv $ANDROID_PATH/boot-wrapper/model.lds    $ANDROID_PATH/boot-wrapper/model.lds.orig
mv $ANDROID_PATH/boot-wrapper/model.lds.S  $ANDROID_PATH/boot-wrapper/model.lds.S.orig

cd $ANDROID_PATH
patch -p0 < $PATCH_DIR/ov_android.patch

cd $PATCH_DIR
