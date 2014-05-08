/*//-------------------------------------------------------------------------------
*	Packetizer.h
*
*	Header file for Packetizer
*	
*	written by: Ingo Randolf - 2014
*
*
*	This library is free software; you can redistribute it and/or
*	modify it under the terms of the GNU Lesser General Public
*	License as published by the Free Software Foundation; either
*	version 2.1 of the License, or (at your option) any later version.
//-------------------------------------------------------------------------------*/

// ensure this library description is only included once
#ifndef Packetizer_h
#define Packetizer_h


#include <inttypes.h>
#include <stdio.h> // for size_t

#include "WString.h"



#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif


enum packetizer_errors
{
	pz_noErr = 0,
	pz_noBuffer,
	pz_bufferSize
};



// library interface description
class Packetizer
{
  public:
	Packetizer();
    Packetizer(size_t);    
    ~Packetizer();
    
        
    // buffer
    char init(size_t);
    char setBufferSize(size_t);    
    size_t getBufferSize() { return m_bufferSize; };
    char* getBuffer() { return m_buffer; };
	
	// append data
	char appendData(int);
	char appendData(long);
	char appendData(String);
	char appendData(char);
	char appendData(char*, size_t);
	
	
	// start condition
	char setStartCondition(int);
	char setStartCondition(long);
	char setStartCondition(String);
	char setStartCondition(char*, size_t);
	char* getStartCondition() { return m_startCondition; };
	size_t getStartConditionSize() { return m_startConditionSize; };	
	bool isStartCondition() { return m_startConditionSize > 0; };
	
	// end condition
	char setEndCondition(int);
	char setEndCondition(long);
	char setEndCondition(String);
	char setEndCondition(char*, size_t);
	char* getEndCondition() { return m_endCondition; };
	size_t getEndConditionSize() { return m_endConditionSize; };	
	bool isEndCondition() { return m_endConditionSize > 0; };
	
	// user callbacks
	void onPacketStart( void (*)(void) );
	void onPacket( void (*)(char*, size_t) );
	void onOverflow( void (*)(char*, size_t) );


  private:
  	void initVars();
  	void freeBuffer(char** _buffer, size_t* _bufferSize);
  	void resetBuffer();
  
  	// buffer
    char* 	m_buffer;
    size_t 	m_bufferSize;
    
    size_t 	m_index; // current index for writing
    bool 	m_startFound;


	// start condition
	char*	m_startCondition;
	size_t	m_startConditionSize;
	size_t	m_startIndex;
	
	// end condition
	char*	m_endCondition;
	size_t	m_endConditionSize;
	size_t	m_endIndex;
	
	// callbacks
	void (*user_onStart)(void);
	void (*user_onPacket)(char*, size_t);
	void (*user_onOverflow)(char*, size_t);

};

#endif

