#ifndef _OTZ_DRM_LIB_
#define _OTZ_DRM_LIB_

int smc_drm(void* buffer, int *ret, void* decrypt_buffer, int size);
void smc_drm_close(void);
int smc_drm_open(void);
#endif

