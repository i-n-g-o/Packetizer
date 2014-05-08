/*//-------------------------------------------------------------------------------
*	Packetizer.cpp
*
*	Packetizer to analyze data for start and end condition
*	
*	written by: Ingo Randolf - 2014
*
*
*	This library is free software; you can redistribute it and/or
*	modify it under the terms of the GNU Lesser General Public
*	License as published by the Free Software Foundation; either
*	version 2.1 of the License, or (at your option) any later version.
//-------------------------------------------------------------------------------*/


#include "Packetizer.h"


//-------------------------------------------------------------------------------
// Constructor //////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------
Packetizer::Packetizer()
{
	initVars();
}

Packetizer::Packetizer(size_t _len)
{
	initVars();
	init((size_t)_len);
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

char Packetizer::init(size_t _size)
{
	return setBufferSize(_size);
}

char Packetizer::setBufferSize(size_t _size)
{
	// free buffer
	freeBuffer(&m_buffer, &m_bufferSize);	
	resetBuffer();
	
	// safety
	if (_size <= 0) return pz_bufferSize;
		
	
	// allocate buffer
	m_buffer = (char*)malloc(m_bufferSize);
  
  	// test
	if (m_buffer == 0) {
		return pz_noBuffer;
	}
	
	// set size
	m_bufferSize = _size;
	
	// erase buffer
	resetBuffer();
	
	return pz_noErr;
}


/*
*	append data
*/
char Packetizer::appendData(int _data)
{
	return appendData((char*)&_data, sizeof(int));
}

char Packetizer::appendData(long _data)
{
	return appendData((char*)&_data, sizeof(long));
}

char Packetizer::appendData(String _data)
{
	return appendData((char*)_data.c_str(), (size_t)_data.length());
}

// append data
char Packetizer::appendData(char* _buffer, size_t _bufferSize)
{
	if (m_buffer == 0) return pz_noBuffer;
	if (_buffer == 0) return pz_noBuffer;
	if (_bufferSize == 0) return pz_bufferSize;
	
	
	unsigned int i;
	for (i=0; i<_bufferSize; i++)
	{
		appendData(_buffer[i]);
	}
	
	return pz_noErr;
}


// append one char and test conditions
char Packetizer::appendData(char _c)
{
	// safety
	if (m_buffer == 0) return pz_noBuffer;
	
	
	if (m_startConditionSize > 0)
	{
		// search for start...
		if (_c != m_startCondition[m_startIndex])
			m_startIndex = 0;
			
		if (_c == m_startCondition[m_startIndex])
		{
			if(++m_startIndex >= m_startConditionSize)
			{
				// startcondition found
				// we always start at 0
				m_index = 0;
				
				m_startFound = true;
				
				if (user_onStart)
				{
					user_onStart();
				}
				
				// dont add caracter to buffer
				return pz_noErr;
			}
		}
		
		
		if (!m_startFound) return pz_noErr;
	}
	
	
	// add data to our buffer
	m_buffer[m_index] = _c;
	
	
	//------------------------------------------------
	// start was found or no startcondition was set
	
	if (m_endConditionSize > 0)
	{
		// look for endcondition...		
		if (_c != m_endCondition[m_endIndex])
			m_endIndex = 0;
		
		
		if (_c == m_endCondition[m_endIndex])
		{
			if(++m_endIndex >= m_endConditionSize)
			{
				// we found an end... call user-method
				if (user_onPacket)
				{
					size_t len = 0;
					
					// calculate len only if it will be >0
					if ( m_index >= m_endConditionSize)
					{
						len = m_index + 1 - m_endConditionSize;
					}
					
					//call user method
					user_onPacket(m_buffer, len);
				}
			
				// reset buffer
				resetBuffer();
			
				return pz_noErr;
			}
		}
	}
	

	// increment writing index
	if (++m_index >= m_bufferSize)
	{
		// reset index	
		m_index = 0;
		
		//call overflow...
		if (user_onOverflow)
		{
			user_onOverflow(m_buffer, m_bufferSize);
		}
	}
	
	return pz_noErr;	
}


/*
*	set startcondition
*/
char Packetizer::setStartCondition(int _data)
{
	return setStartCondition((char*)&_data, sizeof(int));
}

char Packetizer::setStartCondition(long _data)
{
	return setStartCondition((char*)&_data, sizeof(long));
}

char Packetizer::setStartCondition(String _data)
{
	return setStartCondition((char*)_data.c_str(), (size_t)_data.length());
}

char Packetizer::setStartCondition(char* _buffer, size_t _bufferSize)
{
	// free start condition
	freeBuffer(&m_startCondition, &m_startConditionSize);
	m_startIndex = 0;
	m_startFound = false;
	
	// safety
	if (_buffer == 0) return pz_noErr;
	if (_bufferSize == 0) return pz_noErr;
	
	// allocate buffer
	m_startCondition = (char*)malloc(_bufferSize);

	if (m_startCondition == 0) {
		return pz_noBuffer;
	}

	m_startConditionSize = _bufferSize;

	// copy bytes
	memcpy(m_startCondition, _buffer, _bufferSize);
	
	return pz_noErr;
}


/*
*	set endcondition
*/
char Packetizer::setEndCondition(int _data)
{
	return setEndCondition((char*)&_data, sizeof(int));
}

char Packetizer::setEndCondition(long _data)
{
	return setEndCondition((char*)&_data, sizeof(long));
}

char Packetizer::setEndCondition(String _data)
{
	return setEndCondition((char*)_data.c_str(), (size_t)_data.length());
}

char Packetizer::setEndCondition(char* _buffer, size_t _bufferSize)
{
	// free end condition
	freeBuffer(&m_endCondition, &m_endConditionSize);
	m_endIndex = 0;
	
	// safety
	if (_buffer == 0) return pz_noErr;
	if (_bufferSize == 0) return pz_noErr;
	
	
	// allocate buffer
	m_endCondition = (char*)malloc(_bufferSize);

	if (m_endCondition == 0) {
		return pz_noBuffer;
	}

	m_endConditionSize = _bufferSize;

	// copy bytes
	memcpy(m_endCondition, _buffer, _bufferSize);
	
	return pz_noErr;
}


//-------------------------------------------------------------------------------
// Callback functions
//-------------------------------------------------------------------------------
void Packetizer::onPacketStart( void (*function)(void) )
{
	user_onStart = function;
}

void Packetizer::onPacket( void (*function)(char*, size_t) )
{
	user_onPacket = function;
}

void Packetizer::onOverflow( void (*function)(char*, size_t) )
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
	
	user_onStart = 0;
	user_onPacket = 0;
	user_onOverflow = 0;
}


void Packetizer::freeBuffer(char** _buffer, size_t* _bufferSize)
{
	// free buffer if allocated
	if (*_buffer != 0) {
		free(*_buffer);		
	}
	
	*_buffer = 0;
	*_bufferSize = 0;
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
