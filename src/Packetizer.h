/*
*  Packetizer.h
*
*  Header file for Packetizer
*  written by: Ingo Randolf
*
*  This library is free software; you can redistribute it and/or
*  modify it under the terms of the GNU Lesser General Public
*  License as published by the Free Software Foundation; either
*  version 2.1 of the License, or (at your option) any later version.
*/

#ifndef Packetizer_h
#define Packetizer_h

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef ARDUINO

	// include arduino stuff
	#include "WString.h"

	#if ARDUINO >= 100
		#include "Arduino.h"
	#else
		#include "WProgram.h"
	#endif

#else
	// not arduino
	#include <string>
	typedef std::string String;
#endif


typedef enum _pktz_err {
	pz_noErr = 0,
	pz_noBuffer,
	pz_bufferSize
} pktz_err;



class Packetizer {
	
public:
	Packetizer(); // init with default internal buffersize of 32
	Packetizer(size_t); // init with custom buffer size
	~Packetizer();


	// buffer
	pktz_err init(size_t);
	pktz_err setBufferSize(size_t);
	size_t getBufferSize() { return m_bufferSize; };
	uint8_t* getBuffer() { return m_buffer; };

	// get packet length
	int getPacketEnd() { return m_packetLen; }; // deprecated
	int getPacketLen() { return m_packetLen; };

	// reset
	void reset();

	// append data
	pktz_err appendData(int);
	pktz_err appendData(long);
	pktz_err appendData(String);
	pktz_err appendData(uint8_t);
	pktz_err appendData(uint8_t*, size_t);

	// start condition
	pktz_err setStartCondition(uint8_t);
	pktz_err setStartCondition(int);
	pktz_err setStartCondition(long);
	pktz_err setStartCondition(String);
	pktz_err setStartCondition(uint8_t*, size_t);
	
	uint8_t* getStartCondition() { return m_startCondition; };
	size_t getStartConditionSize() { return m_startConditionSize; };  
	bool isStartCondition() { return m_startConditionSize > 0; };

	// end condition
	pktz_err setEndCondition(uint8_t);
	pktz_err setEndCondition(int);
	pktz_err setEndCondition(long);
	pktz_err setEndCondition(String);
	pktz_err setEndCondition(uint8_t*, size_t);
	
	uint8_t* getEndCondition() { return m_endCondition; };
	size_t getEndConditionSize() { return m_endConditionSize; };  
	bool isEndCondition() { return m_endConditionSize > 0; };

	#ifdef ARDUINO
		void sendStartCondition(Print& _print);
		void sendEndCondition(Print& _print);
	#endif

	// user callbacks
	void onPacketStart( void (*)(void) );
	void onPacket( void (*)(uint8_t*, size_t) );
	void onOverflow( void (*)(uint8_t*, size_t) );


private:
	void initVars();
	void freeBuffer(uint8_t**, size_t*);
	pktz_err allocateBuffer(uint8_t**, size_t*, size_t);
	void resetBuffer();

	// buffer
	uint8_t*	m_buffer;
	size_t		m_bufferSize;
	size_t		m_index; // current index for writing
	
	bool		m_startFound;
	int			m_packetLen;

	// start condition
	uint8_t*	m_startCondition;
	size_t		m_startConditionSize;
	size_t		m_startIndex;

	// end condition
	uint8_t*	m_endCondition;
	size_t		m_endConditionSize;
	size_t		m_endIndex;

	// callbacks
	void (*user_onStart)(void);
	void (*user_onPacket)(uint8_t*, size_t);
	void (*user_onOverflow)(uint8_t*, size_t);
};

#endif

