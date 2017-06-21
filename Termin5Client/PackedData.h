#ifndef __PACKED_DATA_H__
#define __PACKED_DATA_H__

#include "Motion.h"

struct __attribute__ ((__packed__)) PackedData {
	Motion_t motion;
	UInt64 time;
};
typedef struct PackedData PackedData_t;

#endif /* __PACKED_DATA_H__*/
