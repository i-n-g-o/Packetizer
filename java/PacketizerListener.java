/*-------------------------------------------------------------------------------
*   PacketizerListener.java
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
package org.packetizer;

public interface PacketizerListener {

    public void onPacketStart();

    public void onPacket(byte[] _packet);

    public void onPacketOverflow(byte[] _packet);
}
