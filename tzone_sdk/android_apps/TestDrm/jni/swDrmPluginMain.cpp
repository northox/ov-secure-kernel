#include "swDrmPluginMain.h"
#include <fcntl.h>
#include <string.h>
#include <drm/DrmRights.h>
#include <drm/DrmConstraints.h>
#include <drm/DrmMetadata.h>
#include <drm/DrmInfo.h>
#include <drm/DrmInfoEvent.h>
#include <drm/DrmInfoStatus.h>
#include <drm/DrmConvertedStatus.h>
#include <drm/DrmInfoRequest.h>
#include <drm/DrmSupportInfo.h>
#include <DrmManager.h>
#include <android/log.h>
#define BUF_SIZE 256

using namespace android;

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jstring JNICALL Java_com_example_testdrm_JniInterface_testDrm
							(JNIEnv *jEnv, jclass jClass, jstring input_param)
{
	char result[BUF_SIZE];
	const char *input;

	input = jEnv->GetStringUTFChars(input_param,NULL);
	if(input == NULL) {
		strcpy(result,"Hello from DRM Layer. No parameter passed ");
	} else {
		strcpy(result,"Hello from DRM Layer. You passed the string ");
		strcat(result,input);
	}
	jEnv->ReleaseStringUTFChars(input_param,input);
	return(jEnv->NewStringUTF((const char*)result));
}

JNIEXPORT jstring JNICALL Java_com_example_testdrm_JniInterface_initDrmEngine
                            (JNIEnv *jEnv, jclass jClass)
{
    char result[BUF_SIZE];
    result[0] = '\0';
    return(jEnv->NewStringUTF((const char*)result));
}

#ifdef __cplusplus
}
#endif

// This extern "C" is mandatory to be managed by TPlugInManager
extern "C" IDrmEngine* create() {
    return new SwDrmPlugin();
}

// This extern "C" is mandatory to be managed by TPlugInManager
extern "C" void destroy(IDrmEngine* pPlugIn) {
    delete pPlugIn;
    pPlugIn = NULL;
}

SwDrmPlugin::SwDrmPlugin()
    : DrmEngineBase() {

}

SwDrmPlugin::~SwDrmPlugin() {

}

DrmMetadata* SwDrmPlugin::onGetMetadata(int uniqueId, const String8* path)
{
    return NULL;
}

DrmConstraints* SwDrmPlugin::onGetConstraints(
        int uniqueId, const String8* path, int action)
{

    LOGD("SwDrmPlugin::onGetConstraints From Path: %d", uniqueId);
    DrmConstraints* drmConstraints = new DrmConstraints();
    String8 value("dummy_available_time");
    char* charValue = NULL;
    charValue = new char[value.length() + 1];
    strncpy(charValue, value.string(), value.length());

    //Just add dummy available time for verification
    drmConstraints->put(&(DrmConstraints::LICENSE_AVAILABLE_TIME), charValue);
    return drmConstraints;
}


DrmInfoStatus* SwDrmPlugin::onProcessDrmInfo(int uniqueId,
                                                const DrmInfo* drmInfo)
{
    LOGD("SwDrmPlugin::onProcessDrmInfo - Enter : %d", uniqueId);
    DrmInfoStatus* drmInfoStatus = NULL;
    if (NULL != drmInfo) {
        switch (drmInfo->getInfoType()) {
        case DrmInfoRequest::TYPE_REGISTRATION_INFO: {
            const DrmBuffer* emptyBuffer = new DrmBuffer();
            drmInfoStatus = new DrmInfoStatus(DrmInfoStatus::STATUS_OK,
                    DrmInfoRequest::TYPE_REGISTRATION_INFO, emptyBuffer, drmInfo->getMimeType());
            break;
        }
        case DrmInfoRequest::TYPE_UNREGISTRATION_INFO: {
            const DrmBuffer* emptyBuffer = new DrmBuffer();
            drmInfoStatus = new DrmInfoStatus(DrmInfoStatus::STATUS_OK,
                    DrmInfoRequest::TYPE_UNREGISTRATION_INFO, emptyBuffer, drmInfo->getMimeType());
            break;
        }
        case DrmInfoRequest::TYPE_RIGHTS_ACQUISITION_INFO: {
            String8 licenseString("dummy_license_string");
            const int bufferSize = licenseString.size();
            char* data = NULL;
            data = new char[bufferSize];
            memcpy(data, licenseString.string(), bufferSize);
            const DrmBuffer* buffer = new DrmBuffer(data, bufferSize);
            drmInfoStatus = new DrmInfoStatus(DrmInfoStatus::STATUS_OK,
                    DrmInfoRequest::TYPE_RIGHTS_ACQUISITION_INFO, buffer, drmInfo->getMimeType());
            break;
        }
        }
    }
    LOGD("SwDrmPlugin::onProcessDrmInfo - Exit");
    return drmInfoStatus;
}

status_t SwDrmPlugin::onSetOnInfoListener(
            int uniqueId, const IDrmEngine::OnInfoListener* infoListener)
{
    LOGD("SwDrmPlugin::onSetOnInfoListener : %d", uniqueId);
    return DRM_NO_ERROR;
}

status_t SwDrmPlugin::onInitialize(int uniqueId)
{
    LOGD("SwDrmPlugin::onInitialize : %d", uniqueId);

    return DRM_NO_ERROR;
}

status_t SwDrmPlugin::onTerminate(int uniqueId)
{
    LOGD("SwDrmPlugin::onTerminate : %d", uniqueId);
    return DRM_NO_ERROR;
}

DrmSupportInfo* SwDrmPlugin::onGetSupportInfo(int uniqueId)
{
    LOGD("SwDrmPlugin::onGetSupportInfo : %d", uniqueId);
    DrmSupportInfo* drmSupportInfo = new DrmSupportInfo();
    // Add mimetype's
    drmSupportInfo->addMimeType(String8("application/video.*.*"));
    // Add File Suffixes
    drmSupportInfo->addFileSuffix(String8(".*"));
    // Add plug-in description
    drmSupportInfo->setDescription(String8("Sierraware custom DRM plug-in"));
    return drmSupportInfo;
}

status_t SwDrmPlugin::onSaveRights(int uniqueId, const DrmRights& drmRights,
            const String8& rightsPath, const String8& contentPath)
{
    LOGD("SwDrmPlugin::onSaveRights : %d", uniqueId);
    return DRM_NO_ERROR;
}

DrmInfo* SwDrmPlugin::onAcquireDrmInfo(int uniqueId,
                                        const DrmInfoRequest* drmInfoRequest)
{
    LOGD("SwDrmPlugin::onAcquireDrmInfo : %d", uniqueId);
    DrmInfo* drmInfo = NULL;
    if (NULL != drmInfoRequest) {
        String8 dataString("dummy_acquistion_string");
        int length = dataString.length();
        char* data = NULL;
        data = new char[length];
        memcpy(data, dataString.string(), length);
        drmInfo = new DrmInfo(drmInfoRequest->getInfoType(),
            DrmBuffer(data, length), drmInfoRequest->getMimeType());
    }
    return drmInfo;
}

bool SwDrmPlugin::onCanHandle(int uniqueId, const String8& path)
{
    LOGD("SwDrmPlugin::canHandle: %s ", path.string());
    String8 extension = path.getPathExtension();
    extension.toLower();
    return (String8(".*") == extension);
    return(0);
}

String8 SwDrmPlugin::onGetOriginalMimeType(int uniqueId, const String8& path)
{
    LOGD("SwDrmPlugin::onGetOriginalMimeType() : %d", uniqueId);
    return String8("video/*.*");
}

int SwDrmPlugin::onGetDrmObjectType(int uniqueId,
                                const String8& path, const String8& mimeType)
{
    LOGD("SwDrmPlugin::onGetDrmObjectType() : %d", uniqueId);
    return DrmObjectType::UNKNOWN;
}

int SwDrmPlugin::onCheckRightsStatus(int uniqueId,
                                            const String8& path, int action)
{
    LOGD("SwDrmPlugin::onCheckRightsStatus() : %d", uniqueId);
    int rightsStatus = RightsStatus::RIGHTS_VALID;
    return rightsStatus;
}

status_t SwDrmPlugin::onConsumeRights(int uniqueId,
                                        DecryptHandle* decryptHandle,
                                                    int action, bool reserve)
{
    LOGD("SwDrmPlugin::onConsumeRights() : %d", uniqueId);
    return DRM_NO_ERROR;
}

status_t SwDrmPlugin::onSetPlaybackStatus(int uniqueId,
                            DecryptHandle* decryptHandle,
                                        int playbackStatus, int64_t position)
{
    LOGD("SwDrmPlugin::onSetPlaybackStatus() : %d", uniqueId);
    return DRM_NO_ERROR;
}

bool SwDrmPlugin::onValidateAction(int uniqueId, const String8& path,
            int action, const ActionDescription& description)
{
    LOGD("SwDrmPlugin::onValidateAction() : %d", uniqueId);
    return true;
}

status_t SwDrmPlugin::onRemoveRights(int uniqueId, const String8& path)
{
    LOGD("SwDrmPlugin::onRemoveRights() : %d", uniqueId);
    return DRM_NO_ERROR;
}

status_t SwDrmPlugin::onRemoveAllRights(int uniqueId)
{
    LOGD("SwDrmPlugin::onRemoveAllRights() : %d", uniqueId);
    return DRM_NO_ERROR;
}

status_t SwDrmPlugin::onOpenConvertSession(int uniqueId, int convertId)
{
    LOGD("SwDrmPlugin::onOpenConvertSession() : %d", uniqueId);
    return DRM_NO_ERROR;
}

DrmConvertedStatus* SwDrmPlugin::onConvertData(
            int uniqueId, int convertId, const DrmBuffer* inputData)
{
	LOGD("Waiting for infinite time inside onDecrypt\n");
	while(1);
    LOGD("SwDrmPlugin::onConvertData() : %d", uniqueId);
    DrmBuffer* convertedData = NULL;
    if (NULL != inputData && 0 < inputData->length) {
        int length = inputData->length;
        char* data = NULL;
        data = new char[length];
        convertedData = new DrmBuffer(data, length);
        memcpy(convertedData->data, inputData->data, length);
    }
    return new DrmConvertedStatus(DrmConvertedStatus::STATUS_OK, convertedData, 0 /*offset*/);
}

DrmConvertedStatus* SwDrmPlugin::onCloseConvertSession(int uniqueId,
                                                                int convertId)
{
    LOGD("SwDrmPlugin::onCloseConvertSession() : %d", uniqueId);
    return new DrmConvertedStatus(DrmConvertedStatus::STATUS_OK, NULL, 0 /*offset*/);
}

status_t SwDrmPlugin::onOpenDecryptSession(int uniqueId,
        DecryptHandle* decryptHandle, int fd, off64_t offset, off64_t length)
{
    LOGD("SwDrmPlugin::onOpenDecryptSession() : %d", uniqueId);
#if 1
    decryptHandle->mimeType = String8("video/*.*");
    decryptHandle->decryptApiType = DecryptApiType::CONTAINER_BASED;
    decryptHandle->status = DRM_NO_ERROR;
    decryptHandle->decryptInfo = NULL;
    filedes = dup(fd);
	if(!smc_drm_open())
		return DRM_ERROR_CANNOT_HANDLE;

    return DRM_NO_ERROR;
#endif
    return(DRM_ERROR_CANNOT_HANDLE);
}

status_t SwDrmPlugin::onOpenDecryptSession(int uniqueId,
                            DecryptHandle* decryptHandle, const char* uri)
{
    return(DRM_ERROR_CANNOT_HANDLE);
    /*return DRM_NO_ERROR; */
}


status_t SwDrmPlugin::onCloseDecryptSession(int uniqueId,
                                                DecryptHandle* decryptHandle)
{
    LOGD("SwDrmPlugin::onCloseDecryptSession() : %d", uniqueId);
    if (NULL != decryptHandle) {
		smc_drm_close();
        if (NULL != decryptHandle->decryptInfo) {
            delete decryptHandle->decryptInfo;
            decryptHandle->decryptInfo = NULL;
        }
        delete decryptHandle;
        decryptHandle = NULL;
    }
    return DRM_NO_ERROR;
}


status_t SwDrmPlugin::onInitializeDecryptUnit(int uniqueId,
                    DecryptHandle* decryptHandle,
                            int decryptUnitId, const DrmBuffer* headerInfo)
{
    LOGD("SwDrmPlugin::onInitializeDecryptUnit() : %d", uniqueId);
    return DRM_NO_ERROR;
}

status_t SwDrmPlugin::onDecrypt(int uniqueId, DecryptHandle* decryptHandle,
        int decryptUnitId, const DrmBuffer* encBuffer,
                                            DrmBuffer** decBuffer, DrmBuffer* IV)
{
	LOGD("Waiting for infinite time inside onDecrypt\n");
	while(1);
    LOGD("SwDrmPlugin::onDecrypt() : %d", uniqueId);
    if (NULL != (*decBuffer) && 0 < (*decBuffer)->length) {
        memcpy((*decBuffer)->data, encBuffer->data, encBuffer->length);
        (*decBuffer)->length = encBuffer->length;
    }
    return DRM_NO_ERROR;
}

status_t SwDrmPlugin::onFinalizeDecryptUnit(
            int uniqueId, DecryptHandle* decryptHandle, int decryptUnitId)
{
    LOGD("SwDrmPlugin::onFinalizeDecryptUnit() : %d", uniqueId);
    return DRM_NO_ERROR;
}


ssize_t SwDrmPlugin::onPread(int uniqueId, DecryptHandle* decryptHandle,
            void* buffer, ssize_t numBytes, off64_t offset)
{
    char decrypt_buffer[1024], local_buffer[1024];
    int numBytesRet;
    ssize_t ret = 0, ret_tmp = -1;
    int cnt = numBytes;
    LOGD("SwDrmPlugin::onPread() : %d\n", uniqueId);
    off64_t result = lseek64(filedes, offset, SEEK_SET);
        if (result == -1) {
            LOGE("seek to %lld failed", offset);
            return UNKNOWN_ERROR;
        }

    while(cnt > 1024){
	ret_tmp = ::read(filedes, decrypt_buffer, 1024);
	if(ret_tmp > 0){
#if 1
		if(smc_drm(local_buffer, &numBytesRet, decrypt_buffer, ret_tmp)) {
#endif
//		LOGD("Memcmp : %d\n", memcmp(decrypt_buffer, local_buffer, ret_tmp));
			memcpy((void*)((uint32_t)buffer + numBytes - cnt), local_buffer, ret_tmp);
//			memcpy((void*)((uint32_t)buffer + numBytes - cnt), decrypt_buffer, ret_tmp);
			ret += numBytesRet;
//			ret += ret_tmp;
#if 1
		}
		else {
			return ret_tmp;
		}
#endif
	}else{
		return ret_tmp;
	}
	cnt -=1024;	
    }
    if(cnt > 0){
	ret_tmp = ::read(filedes, decrypt_buffer, cnt);
	if(ret_tmp > 0){
#if 1
		if(smc_drm(local_buffer, &numBytesRet, decrypt_buffer, ret_tmp)) {
#endif
//		LOGD("Memcmp : %d\n", memcmp(decrypt_buffer, local_buffer, ret_tmp));
		memcpy((void*)((uint32_t)buffer + numBytes - cnt), local_buffer, ret_tmp);
//		memcpy((void*)((uint32_t)buffer + numBytes - cnt), decrypt_buffer, ret_tmp);
//		return ret + ret_tmp;
		return ret + numBytesRet;
#if 1
		}
		else {
		return ret_tmp;
		}
#endif
	}else{
		return ret_tmp;
	}
    }else
	return ret;
}
