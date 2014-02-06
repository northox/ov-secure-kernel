#include <jni.h>
#include <otz_virtual_keyboard.h>

JNIEXPORT jstring
	JNICALL Java_com_example_virtualkeyboard_VirtualKeyboardActivity_VirtualKeyboard
							(JNIEnv *jEnv, jclass jClass)

{
	char *ret = virtual_keyboard();
	return((*jEnv)->NewStringUTF(jEnv, ret));
}
