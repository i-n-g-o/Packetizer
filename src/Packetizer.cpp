/*
*	Packetizer.cpp
*
*	Packetizer to analyze data for start and end condition
*	https://github.com/i-n-g-o/Packetizer
*	
*	written by: Ingo Randolf
*
*	This library is free software; you can redistribute it and/or
*	modify it under the terms of the GNU Lesser General Public
*	License as published by the Free Software Foundation; either
*	version 2.1 of the License, or (at your option) any later version.
*/


#include "Packetizer.h"

//-------------------------------------------------------------------------------
// Constructor //////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------
Packetizer::Packetizer()
{
	initVars();
	init(32);
}

Packetizer::Packetizer(size_t _size)
{
	initVars();
	init(_size);
}

Packetizer::~Packetizer()
{
	freeBuffer(&m_buffer, &m_bufferSize);
	freeBuffer(&m_startCondition, &m_startConditionSize);
	freeBuffer(&m_endCondition, &m_endConditionSize);
}


//-------------------------------------------------------------------------------
// Public Methods ///////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------
pktz_err Packetizer::init(size_t _size)
{
	return setBufferSize(_size);
}

pktz_err Packetizer::setBufferSize(size_t _size)
{
	freeBuffer(&m_buffer, &m_bufferSize);
	
	pktz_err result = allocateBuffer(&m_buffer, &m_bufferSize, _size);

	resetBuffer();
	
	return result;
}


void Packetizer::reset()
{
	m_index = 0;
	m_startFound = false;
	m_startIndex = 0;
	m_endIndex = 0;
}


//-------------------------------------------------------------------------------
// append data
//-------------------------------------------------------------------------------
pktz_err Packetizer::appendData(int data)
{
	return appendData((uint8_t*)&data, sizeof(int));
}

pktz_err Packetizer::appendData(long data)
{
	return appendData((uint8_t*)&data, sizeof(long));
}

pktz_err Packetizer::appendData(String data)
{
#ifdef __RFduino__
	// Stupidly, RFDuino's copy of String uses cstr() instead of c_str()
	return appendData((uint8_t*)data.cstr(), (size_t)data.length());
#else
	return appendData((uint8_t*)data.c_str(), (size_t)data.length());
#endif
}

// append data
pktz_err Packetizer::appendData(uint8_t* _buffer, size_t _bufferSize)
{
	if (m_buffer == 0) return pz_noBuffer;
	if (_buffer == 0) return pz_noBuffer;
	if (_bufferSize == 0) return pz_bufferSize;
	
	
	unsigned int i;
	for (i=0; i<_bufferSize; i++) {
		appendData(_buffer[i]);
	}
	
	return pz_noErr;
}


//-------------------------------------------------------------------------------
// append one byte and test conditions
pktz_err Packetizer::appendData(uint8_t data)
{
	if (m_buffer == 0) return pz_noBuffer;
	
	m_packetLen = -1;
	
	// look for startcondition
	if (m_startConditionSize > 0) {
		
		// if incoming byte does not match current startCondition byte
		// reset startIndex and continue
		if (data != m_startCondition[m_startIndex]) {
			m_startIndex = 0;
		}
		
		// check if incoming byte matches startCondition byte
		if (data == m_startCondition[m_startIndex]) {
			
			if (++m_startIndex >= m_startConditionSize) {
				
				// startcondition found
				// we always start at index 0
				m_index = 0;
				m_endIndex = 0;
				m_startIndex = 0;				
				m_startFound = true;
				
				if (user_onStart) {
					user_onStart();
				}
				
				// don't add character to buffer
				return pz_noErr;
			}
		}
		
		if (!m_startFound) return pz_noErr;
	}
	
	// add data to our buffer
	m_buffer[m_index] = data;
	
	// continue to look for end condition
	if (m_endConditionSize > 0) {
		
		// if incoming byte does not match current endCondition byte
		// reset startIndex and continue
		if (data != m_endCondition[m_endIndex]) {
			m_endIndex = 0;
		}
		
		// check if incoming byte matches endCondition byte
		if (data == m_endCondition[m_endIndex]) {
			
			if (++m_endIndex >= m_endConditionSize) {
				
				// we found the end, calculate the length
				m_packetLen = m_index + 1 - m_endConditionSize;
		
				// call user-method if any
				if (user_onPacket) {
					user_onPacket(m_buffer, m_packetLen);
				}
				
				// reset index
				m_index = 0;
				m_endIndex = 0;
				m_startIndex = 0;
				m_startFound = false;
			
				return pz_noErr;
			}
		}
	}
	
	// increment writing index and check for overflow
	if (++m_index >= m_bufferSize) {
		
		// reset index	
		m_index = 0;
		
		// call overflow...
		if (user_onOverflow) {
			user_onOverflow(m_buffer, m_bufferSize);
		}
	}
	
	return pz_noErr;
}


//-------------------------------------------------------------------------------
// start condition
//-------------------------------------------------------------------------------
pktz_err Packetizer::setStartCondition(uint8_t data)
{
	return setStartCondition((uint8_t*)&data, sizeof(uint8_t));
}

pktz_err Packetizer::setStartCondition(int data)
{
	return setStartCondition((uint8_t*)&data, sizeof(int));
}

pktz_err Packetizer::setStartCondition(long data)
{
	return setStartCondition((uint8_t*)&data, sizeof(long));
}

pktz_err Packetizer::setStartCondition(String data)
{
#ifdef __RFduino__
	// Stupidly, RFDuino's copy of String uses cstr() instead of c_str()
	return setStartCondition((uint8_t*)data.cstr(), (size_t)data.length());
#else
	return setStartCondition((uint8_t*)data.c_str(), (size_t)data.length());
#endif
}

pktz_err Packetizer::setStartCondition(uint8_t* _buffer, size_t _bufferSize)
{
	freeBuffer(&m_startCondition, &m_startConditionSize);
	m_startIndex = 0;
	m_startFound = false;
	
	// safety
	if (_buffer == 0) return pz_noErr;
	if (_bufferSize == 0) return pz_noErr;

	// allocate buffer
	pktz_err result = allocateBuffer(&m_startCondition, &m_startConditionSize, _bufferSize);
	if (result != pz_noErr) return result;
	
	// copy bytes
	memcpy(m_startCondition, _buffer, _bufferSize);
	
	return pz_noErr;
}


//-------------------------------------------------------------------------------
// end condition
//-------------------------------------------------------------------------------
pktz_err Packetizer::setEndCondition(uint8_t data)
{
	return setEndCondition((uint8_t*)&data, sizeof(uint8_t));
}

pktz_err Packetizer::setEndCondition(int data)
{
	return setEndCondition((uint8_t*)&data, sizeof(int));
}

pktz_err Packetizer::setEndCondition(long data)
{
	return setEndCondition((uint8_t*)&data, sizeof(long));
}

pktz_err Packetizer::setEndCondition(String data)
{
#ifdef __RFduino__
	// Stupidly, RFDuino's copy of String uses cstr() instead of c_str()
	return setEndCondition((uint8_t*)data.cstr(), (size_t)data.length());
#else
	return setEndCondition((uint8_t*)data.c_str(), (size_t)data.length());
#endif
}

pktz_err Packetizer::setEndCondition(uint8_t* _buffer, size_t _bufferSize)
{
	freeBuffer(&m_endCondition, &m_endConditionSize);
	m_endIndex = 0;
	
	// safety
	if (_buffer == 0) return pz_noErr;
	if (_bufferSize == 0) return pz_noErr;
	
	// allocate buffer
	pktz_err result = allocateBuffer(&m_endCondition, &m_endConditionSize, _bufferSize);	
	if (result != pz_noErr) return result;
	
	// copy bytes
	memcpy(m_endCondition, _buffer, _bufferSize);

	return pz_noErr;
}


#ifdef ARDUINO
//-------------------------------------------------------------------------------
// send start and end condition
//-------------------------------------------------------------------------------
void Packetizer::sendStartCondition(Print& _print)
{
	if (m_startConditionSize > 0) {
		_print.write(m_startCondition, m_startConditionSize);
	}
}

void Packetizer::sendEndCondition(Print& _print)
{
	if (m_endConditionSize > 0) {
		_print.write(m_endCondition, m_endConditionSize);
	}
}
#endif


//-------------------------------------------------------------------------------
// Callback functions
//-------------------------------------------------------------------------------
void Packetizer::onPacketStart( void (*function)(void) )
{
	user_onStart = function;
}

void Packetizer::onPacket( void (*function)(uint8_t*, size_t) )
{
	user_onPacket = function;
}

void Packetizer::onOverflow( void (*function)(uint8_t*, size_t) )
{
	user_onOverflow = function;
}


//-------------------------------------------------------------------------------
// Private Methods //////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------
void Packetizer::initVars()
{
	m_buffer = 0;
	m_bufferSize = 0;
	m_index = 0;
	m_startFound = false;
	m_startCondition = 0;
	m_startConditionSize = 0;
	m_startIndex = 0;
	m_endCondition = 0;
	m_endConditionSize = 0;
	m_endIndex = 0;
	m_packetLen = -1;
	
	user_onStart = 0;
	user_onPacket = 0;
	user_onOverflow = 0;
}

void Packetizer::freeBuffer(uint8_t** _buffer, size_t* _bufferSize)
{
	// free buffer if allocated
	if (*_buffer != 0) {
		free(*_buffer);		
	}
	
	*_buffer = 0;
	*_bufferSize = 0;
}

pktz_err Packetizer::allocateBuffer(uint8_t** _buffer, size_t* _bufferSize, size_t _size)
{
	if (_size == 0) return pz_bufferSize;

	*_buffer = (uint8_t*)malloc(_size);
	
	if (*_buffer == 0) return pz_noBuffer;
	
	*_bufferSize = _size;
	
	return pz_noErr;
}

void Packetizer::resetBuffer()
{
	m_index = 0;	
	m_startIndex = 0;
	m_endIndex = 0;	
	m_startFound = false;
	
	if (m_buffer == 0) return;
	if (m_bufferSize == 0) return;
	
	// clear buffer
	memset(m_buffer, 0, m_bufferSize);	
}
