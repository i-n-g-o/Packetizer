/*-------------------------------------------------------------------------------
*   Packetizer.java
*
*   java class for Packetizer
*   https://github.com/i-n-g-o/Packetizer
*
*   written by: Ingo Randolf - 2016
*
*
*   This class is free software; you can redistribute it and/or
*   modify it under the terms of the GNU Lesser General Public
*   License as published by the Free Software Foundation; either
*   version 2.1 of the License, or (at your option) any later version.
*-------------------------------------------------------------------------------*/

package plugins.units.data.bytes;

/**
 * Created by inx on 13/01/16.
 */
public class Packetizer {


    private PacketizerListener listener = null;

    private byte[]  m_buffer;

    private int     m_packetEnd;

    private int     m_index; // current index for writing

    private boolean m_startFound;

    private byte[]  m_startCondition;

    private int     m_startIndex;

    private byte[]  m_endCondition;

    private int     m_endIndex;

    /** */
    public Packetizer() {
        m_buffer = new byte[128];
    }

    public Packetizer(int _size) {
        m_buffer = new byte[_size];
    }

    public void setBufferSize(int _size) {
        m_buffer = new byte[_size];
        resetBuffer();
    }

    private void resetBuffer() {
        m_index = 0;
        m_startIndex = 0;
        m_endIndex = 0;
        m_startFound = false;
    }

    /** */
    public void setListener(PacketizerListener _listener) {
        listener = _listener;
    }

    /** */
    public void setStartCondition(byte[] _start) {
        m_startCondition = _start;
    }

    public void setEndCondition(byte[] _end) {
        m_endCondition = _end;
    }

    /** */
    public int getPacketEnd() {
        return m_packetEnd;
    }


    /** */
    public void appendData(byte[] _buffer) {
        for (int i=0; i<_buffer.length; i++) {
            appendData(_buffer[i]);
        }
    }

    public void appendData(byte _c) {

        m_packetEnd = -1;

        if (m_startCondition != null)
        {
            // search for start...
            if (_c != m_startCondition[m_startIndex])
                m_startIndex = 0;

            if (_c == m_startCondition[m_startIndex])
            {
                if(++m_startIndex >= m_startCondition.length)
                {
                    // startcondition found
                    // we always start at index 0
                    m_index = 0;
                    m_endIndex = 0;
                    m_startIndex = 0;
                    m_startFound = true;

                    if (listener != null) {
                        listener.onPacketStart();
                    }

                    // dont add caracter to buffer
                    return;
                }
            }


            if (!m_startFound) return;
        }


        // add data to our buffer
        m_buffer[m_index] = _c;

        //------------------------------------------------
        // start was found or no startcondition was set

        if (m_endCondition != null)
        {
            // look for endcondition...
            if (_c != m_endCondition[m_endIndex])
                m_endIndex = 0;


            if (_c == m_endCondition[m_endIndex])
            {
                if(++m_endIndex >= m_endCondition.length)
                {
                    // we found an end
                    m_packetEnd = 0;

                    // calculate len only if it will be >0
                    if ( m_index >= m_endCondition.length) {
                        m_packetEnd = m_index + 1 - m_endCondition.length;
                    }


                    // call user-method if any
                    if (listener != null) {
                        byte[] packet = new byte[m_packetEnd];

                        for (int i=0; i<m_packetEnd; i++) {
                            packet[i] = m_buffer[i];
                        }
                        listener.onPacket(packet);
                    }

                    // reset index
                    m_index = 0;
                    m_endIndex = 0;
                    m_startIndex = 0;
                    m_startFound = false;

                    return;
                }
            }
        }


        // increment writing index
        if (++m_index >= m_buffer.length)
        {
            // reset index
            m_index = 0;

            //call overflow...
            if (listener != null) {
                listener.onPacketOverflow(m_buffer);
            }
        }
    }

}
