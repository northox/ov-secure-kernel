#ifndef _DISK_IO_H_
#define _DISK_IO_H_

#include <sw_types.h>

#define SW_FS_STARTBLK	87073
#define SW_FS_BLKCNT	8192
#define SW_FS_SIZE		(4 * 1024 * 1024)

u32 read_from_disk();

#endif
