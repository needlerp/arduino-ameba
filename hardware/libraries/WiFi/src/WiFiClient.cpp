/*
  WiFiClient.cpp - Library for Arduino Wifi shield.
  Copyright (c) 2011-2014 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "WiFi.h"
#include "WiFiClient.h"



WiFiClient::WiFiClient()  : _sock(MAX_SOCK_NUM){
	_pTcpSocket = new TCPSocketConnection();
}

WiFiClient::WiFiClient(TCPSocketConnection* s)  
{	
	_pTcpSocket = s;
	_sock = _pTcpSocket->get_socket_fd();
	WiFiClass::_state[_sock] = _sock;
}



int WiFiClient::connect(const char* host, uint16_t port) {
	IPAddress remote_addr;
	if (WiFi.hostByName(host, remote_addr))
	{
		return connect(remote_addr, port);
	}
	return 0;
}

int WiFiClient::connect(IPAddress ip, uint16_t port) 
{
	int ret;

    _sock = getFirstSocket();
    if (_sock != SOCK_NOT_AVAIL)
    {
		ret = _pTcpSocket->connect(ip.get_address(), port);
		if ( ret != 0 ) {
			Serial.print("WiFiClient : connect failed ret=");
			Serial.println(ret);
			return 0; // socket connect failed. 
		}

		
    	if (!connected())
       	{
			Serial.println("WiFiClient : connect failed : connected is not true");
    		return 0;
    	}

		_sock = _pTcpSocket->get_socket_fd();
		WiFiClass::_state[_sock] = _sock;
	
		
    }
	else{
    
    	Serial.println("No Socket available");
    	return 0;
    }
    return 1;
}

int WiFiClient::available() {
	int ret;

	if (_sock == 255) return 0;
	if (_pTcpSocket->is_connected() == false ) return 0;

	return (int)_pTcpSocket->available();
}

size_t WiFiClient::write(uint8_t b) {
	  return write(&b, 1);
}

size_t WiFiClient::write(const uint8_t *buf, size_t size) {
  int i;
  
  return _pTcpSocket->send_all((char*)buf, (int)size);
}


int WiFiClient::read() {
	uint8_t ch;
	int ret;
	
	ret = read(&ch, 1);
	if ( ret <= 0 ) return -1;
	else return (int)ch;
	
}


int WiFiClient::read(uint8_t* buf, size_t size) {
  return _pTcpSocket->receive((char*)buf, (int)size);
}

int WiFiClient::peek() {
	return _pTcpSocket->peek();
}

void WiFiClient::flush() {
  while (available())
    read();
}

void WiFiClient::stop() {

  if (_sock == 255)
    return;

  _pTcpSocket->close();
  WiFiClass::_state[_sock] = NA_STATE;

  _sock = 255;
}

uint8_t WiFiClient::connected() {

  if (_sock == 255) {
    return 0;
  } else {
  	return ( _pTcpSocket->is_connected() == true )? 1 : 0;
  }
}

WiFiClient::operator bool() {
  if ( _sock == 255 ) return false;
  return _pTcpSocket->is_connected();
}

// Private Methods
uint8_t WiFiClient::getFirstSocket()
{
    for (int i = 0; i < MAX_SOCK_NUM; i++) {
      if (WiFiClass::_state[i] == NA_STATE)
      {
          return i;
      }
    }
    return SOCK_NOT_AVAIL;
}


char* WiFiClient::get_address(void)
{
	return _pTcpSocket->get_address();
}

