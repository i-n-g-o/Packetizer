#ifndef IO_PACKETIZER_H_
#define IO_PACKETIZER_H_

#include <stdint.h>
#include <stddef.h>

typedef enum _pktz_err {
	pz_noErr = 0,
	pz_noBuffer,
	pz_bufferSize
} pktz_err;

struct pktz;

struct pktz* pktz_init_string(size_t bufSize, const char* startCond, const char* endCond);
void pktz_fini(struct pktz* pktz);
pktz_err pktz_appendData(struct pktz* pktz, uint8_t data);
void pktz_setOnPacket(struct pktz* pktz, void (*)(uint8_t*, size_t, void*));
void pktz_setUserData(struct pktz* pktz, void* ud);

#endif
