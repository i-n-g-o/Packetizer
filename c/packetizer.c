/* -----------------------------------------------------------------------------
 *	Packetizer.c
 *
 *	Packetizer to analyze data for start and end condition
 *	https://github.com/i-n-g-o/Packetizer
 *
 *	written by: Ingo Randolf - 2014
 *
 *
 *	This library is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU Lesser General Public
 *	License as published by the Free Software Foundation; either
 *	version 2.1 of the License, or (at your option) any later version.
 * ---------------------------------------------------------------------------*/

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "packetizer.h"

struct pktz {
	uint8_t*	m_buffer;
	size_t		m_bufferSize;
	int         m_packetLen;

	size_t		m_index; // current index for writing
	bool		m_startFound;

	// start condition
	uint8_t*	m_startCondition;
	size_t		m_startConditionSize;
	size_t		m_startIndex;

	// end condition
	uint8_t*	m_endCondition;
	size_t		m_endConditionSize;
	size_t		m_endIndex;

	// callbacks
	void		(*user_onStart)(void);
	void		(*user_onPacket)(uint8_t*, size_t, void*);
	void		(*user_onOverflow)(uint8_t*, size_t);
	void*		m_userData;
};

struct pktz* pktz_init_string(size_t bufSize, const char* startCond, const char* endCond)
{
	struct pktz* pktz = calloc(1, sizeof(struct pktz));
	if (pktz == NULL)
		return NULL;

	pktz->m_buffer = malloc(bufSize);
	if (pktz->m_buffer == NULL) {
		free(pktz);
		return NULL;
	}

	pktz->m_bufferSize = bufSize;

	if (startCond) {
		pktz->m_startCondition = (uint8_t*)strdup(startCond);
		pktz->m_startConditionSize = strlen(startCond);
	}

	pktz->m_endCondition = (uint8_t*)strdup(endCond);
	pktz->m_endConditionSize = strlen(endCond);

	return pktz;
}

void pktz_fini(struct pktz* pktz)
{
	if (pktz == NULL)
		return;
	if (pktz->m_buffer)
		free(pktz->m_buffer);
	if (pktz->m_startCondition)
		free(pktz->m_startCondition);
	if (pktz->m_endCondition)
		free(pktz->m_endCondition);
	free(pktz);
}

// append one byte and test conditions
pktz_err pktz_appendData(struct pktz* pktz, uint8_t data)
{
	if (pktz == NULL || pktz->m_buffer == NULL) return pz_noBuffer;

	pktz->m_packetLen = -1;

	// look for startcondition
	if (pktz->m_startConditionSize > 0) {

		// if incoming byte does not match current startCondition byte
		// reset startIndex and continue
		if (data != pktz->m_startCondition[pktz->m_startIndex]) {
			pktz->m_startIndex = 0;
		}

		// check if incoming byte matches startCondition byte
		if (data == pktz->m_startCondition[pktz->m_startIndex]) {

			if (++pktz->m_startIndex >= pktz->m_startConditionSize) {

				// startcondition found
				// we always start at index 0
				pktz->m_index = 0;
				pktz->m_endIndex = 0;
				pktz->m_startIndex = 0;
				pktz->m_startFound = true;

				if (pktz->user_onStart) {
					pktz->user_onStart();
				}

				// don't add character to buffer
				return pz_noErr;
			}
		}

		if (!pktz->m_startFound) return pz_noErr;
	}

	// add data to our buffer
	pktz->m_buffer[pktz->m_index] = data;

	// continue to look for end condition
	if (pktz->m_endConditionSize > 0) {

		// if incoming byte does not match current endCondition byte
		// reset startIndex and continue
		if (data != pktz->m_endCondition[pktz->m_endIndex]) {
			pktz->m_endIndex = 0;
		}

		// check if incoming byte matches endCondition byte
		if (data == pktz->m_endCondition[pktz->m_endIndex]) {

			if (++pktz->m_endIndex >= pktz->m_endConditionSize) {

				// we found the end, calculate the length
				pktz->m_packetLen = pktz->m_index + 1 - pktz->m_endConditionSize;

				// call user-method if any
				if (pktz->user_onPacket) {
					pktz->user_onPacket(pktz->m_buffer,
							    pktz->m_packetLen,
							    pktz->m_userData);
				}

				// reset index
				pktz->m_index = 0;
				pktz->m_endIndex = 0;
				pktz->m_startIndex = 0;
				pktz->m_startFound = false;

				return pz_noErr;
			}
		}
	}

	// increment writing index and check for overflow
	if (++pktz->m_index >= pktz->m_bufferSize) {

		// reset index
		pktz->m_index = 0;

		// call overflow...
		if (pktz->user_onOverflow) {
			pktz->user_onOverflow(pktz->m_buffer, pktz->m_bufferSize);
		}
	}

	return pz_noErr;
}

pktz_err pktz_appendBuf(struct pktz* pktz, const uint8_t* buf, size_t len)
{
	pktz_err err = pz_noErr;
	for (size_t i = 0; i < len; i++) {
		err = pktz_appendData(pktz, buf[i]);
		if (err != pz_noErr)
			return err;
	}
	return pz_noErr;
}


// set user callbacks
void pktz_setOnStart(struct pktz* pktz, void (*start_handler)(void*))
{
	pktz->user_onStart = start_handler;
}

void pktz_setOnPacket(struct pktz* pktz, void (*packet_handler)(uint8_t*, size_t, void*))
{
	pktz->user_onPacket = packet_handler;
}

void pktz_setOnOverflow(struct pktz* pktz, void (*overflow_handler)(uint8_t*, size_t, void*))
{
	pktz->user_onOverflow = overflow_handler;
}

// set user data
void pktz_setUserData(struct pktz* pktz, void* ud)
{
	pktz->m_userData = ud;
}
