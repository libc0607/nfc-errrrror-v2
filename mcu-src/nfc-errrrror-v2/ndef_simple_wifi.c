/*
 *	ndef_simple_wifi
 *	apis for generate single ndef message from plain text
 *  @libc0607
*/

#include "ndef_simple_wifi.h"

__code const uint8_t ndef_wifi_record_type_char[] = {
	'a', 'p', 'p', 'l', 'i', 'c', 'a', 't', 
	'i', 'o', 'n', '/', 'v', 'n', 'd', '.', 
	'w', 'f', 'a', '.', 'w', 's', 'c'
};
__code const uint8_t ndef_wifi_record_type_char_size = sizeof(ndef_wifi_record_type_char); // 23

/*
	Typical ndef wifi message generated from App "NFC Tools Pro" has a 
	length of 67+sizeof(ssid)+sizeof(pwd). Max length of ssid is 32, 
	pwd is 63, so the buffer should have at least 67+32+63=162 bytes, 
	also the max ndef length; 
*/
void generate_simple_ndef_wifi_message()
{
	uint8_t i, p;
	
	// clean buffer
	for (i=0; i<MAX_NDEF_MSG_LEN_ALIGNED; i++) {
		glb_msg[i] = 0;
	}

	p = 0;
	
	// layer 1: ndef message header
	glb_msg[p++] = 0x03;
	// layer 1: ndef message full length
	glb_msg[p++] = NDEF_MSG_LEN_WITHOUT_SSIDPWD + (glb_ssid_len) + (glb_pwd_len);
	
	// layer 2: ndef record header
	// 8'b 1_1_0_1_1_010 // MB=1,ME=1,CF=0,SR=1,IL=1,TNF=0x2(MIME Media Record)
	glb_msg[p++] = 0xDA;	
	// layer 2: ndef record type length
	glb_msg[p++] = ndef_wifi_record_type_char_size;
	// layer 2: ndef record payload length
	glb_msg[p++] = NDEF_PAYLOAD_LEN_WITHOUT_SSIDPWD + (glb_ssid_len) + (glb_pwd_len);
	// layer 2: ndef record id length
	glb_msg[p++] = 0x01;
	// layer 2: ndef record type
	for (i=0; i<ndef_wifi_record_type_char_size; i++) {
		glb_msg[p++] = ndef_wifi_record_type_char[i];
	}
	// layer 2: ndef record id
	glb_msg[p++] = 0x01;
	
	// layer 3: payload
	// I don't know what this part means but it's generated by NFC tools Pro
	glb_msg[p++] = 0x10;
	glb_msg[p++] = 0x0E;
	glb_msg[p++] = 0x00;
	glb_msg[p++] = NDEF_PAYLOAD_LEN_WITHOUT_SSIDPWD + (glb_ssid_len) + (glb_pwd_len) - 4;
	// layer 3: payload: network index
	glb_msg[p++] = 0x10;
	glb_msg[p++] = 0x26;
	glb_msg[p++] = 0x00;
	glb_msg[p++] = 0x01;
	glb_msg[p++] = 0x01;
	// layer 3: payload: ssid
	glb_msg[p++] = 0x10;
	glb_msg[p++] = 0x45;
	glb_msg[p++] = 0x00;
	glb_msg[p++] = glb_ssid_len;
	for (i=0; i<glb_ssid_len; i++) {
		glb_msg[p++] = glb_wifi_ssid[i];
	}
	// layer 3: payload: auth type
	glb_msg[p++] = 0x10;
	glb_msg[p++] = 0x03;
	glb_msg[p++] = 0x00;
	glb_msg[p++] = 0x02;	
	glb_msg[p++] = 0x00;
	glb_msg[p++] = glb_auth;
	// layer 3: payload: encryption type
	glb_msg[p++] = 0x10;
	glb_msg[p++] = 0x0F;
	glb_msg[p++] = 0x00;
	glb_msg[p++] = 0x02;
	glb_msg[p++] = 0x00;
	glb_msg[p++] = glb_enc;
	// layer 3: payload: pwd
	glb_msg[p++] = 0x10;
	glb_msg[p++] = 0x27;
	glb_msg[p++] = 0x00;
	glb_msg[p++] = glb_pwd_len;
	for (i=0; i<glb_pwd_len; i++) {
		glb_msg[p++] = glb_pwd[i];
	}
	// layer 3: payload: mac
	glb_msg[p++] = 0x10;
	glb_msg[p++] = 0x20;
	glb_msg[p++] = 0x00;
	glb_msg[p++] = 0x06;
	glb_msg[p++] = 0xFF;
	glb_msg[p++] = 0xFF;
	glb_msg[p++] = 0xFF;
	glb_msg[p++] = 0xFF;
	glb_msg[p++] = 0xFF;
	glb_msg[p++] = 0xFF;
	
	// layer 1: ndef message end
	glb_msg[p++] = 0xFE;
	
	// finished, mark the length
	glb_msg_len = p;
}

/*
void generate_simple_ndef_wifi_message (wifi_config_t* wifi_conf, ndef_wifi_msg_t* ndef_msg)
{
	uint8_t i, p;
	
	// clean buffer
	for (i=0; i<MAX_NDEF_MSG_LEN_ALIGNED; i++) {
		ndef_msg->msg[i] = 0;
	}

	p = 0;
	
	// layer 1: ndef message header
	ndef_msg->msg[p++] = 0x03;
	// layer 1: ndef message full length
	ndef_msg->msg[p++] = NDEF_MSG_LEN_WITHOUT_SSIDPWD + (wifi_conf->ssid_len) + (wifi_conf->pwd_len);
	
	// layer 2: ndef record header
	// 8'b 1_1_0_1_1_010 // MB=1,ME=1,CF=0,SR=1,IL=1,TNF=0x2(MIME Media Record)
	ndef_msg->msg[p++] = 0xDA;	
	// layer 2: ndef record type length
	ndef_msg->msg[p++] = ndef_wifi_record_type_char_size;
	// layer 2: ndef record payload length
	ndef_msg->msg[p++] = NDEF_PAYLOAD_LEN_WITHOUT_SSIDPWD + (wifi_conf->ssid_len) + (wifi_conf->pwd_len);
	// layer 2: ndef record id length
	ndef_msg->msg[p++] = 0x01;
	// layer 2: ndef record type
	for (i=0; i<ndef_wifi_record_type_char_size; i++) {
		ndef_msg->msg[p++] = ndef_wifi_record_type_char[i];
	}
	// layer 2: ndef record id
	ndef_msg->msg[p++] = 0x01;
	
	// layer 3: payload
	// I don't know what this part means but it's generated by NFC tools Pro
	ndef_msg->msg[p++] = 0x10;
	ndef_msg->msg[p++] = 0x0E;
	ndef_msg->msg[p++] = 0x00;
	ndef_msg->msg[p++] = NDEF_PAYLOAD_LEN_WITHOUT_SSIDPWD + (wifi_conf->ssid_len) + (wifi_conf->pwd_len) - 4;
	// layer 3: payload: network index
	ndef_msg->msg[p++] = 0x10;
	ndef_msg->msg[p++] = 0x26;
	ndef_msg->msg[p++] = 0x00;
	ndef_msg->msg[p++] = 0x01;
	ndef_msg->msg[p++] = 0x01;
	// layer 3: payload: ssid
	ndef_msg->msg[p++] = 0x10;
	ndef_msg->msg[p++] = 0x45;
	ndef_msg->msg[p++] = 0x00;
	ndef_msg->msg[p++] = wifi_conf->ssid_len;
	for (i=0; i<wifi_conf->ssid_len; i++) {
		ndef_msg->msg[p++] = wifi_conf->ssid[i];
	}
	// layer 3: payload: auth type
	ndef_msg->msg[p++] = 0x10;
	ndef_msg->msg[p++] = 0x03;
	ndef_msg->msg[p++] = 0x00;
	ndef_msg->msg[p++] = 0x02;	
	ndef_msg->msg[p++] = 0x00;
	ndef_msg->msg[p++] = wifi_conf->auth;
	// layer 3: payload: encryption type
	ndef_msg->msg[p++] = 0x10;
	ndef_msg->msg[p++] = 0x0F;
	ndef_msg->msg[p++] = 0x00;
	ndef_msg->msg[p++] = 0x02;
	ndef_msg->msg[p++] = 0x00;
	ndef_msg->msg[p++] = wifi_conf->enc;
	// layer 3: payload: pwd
	ndef_msg->msg[p++] = 0x10;
	ndef_msg->msg[p++] = 0x27;
	ndef_msg->msg[p++] = 0x00;
	ndef_msg->msg[p++] = wifi_conf->pwd_len;
	for (i=0; i<wifi_conf->pwd_len; i++) {
		ndef_msg->msg[p++] = wifi_conf->pwd[i];
	}
	// layer 3: payload: mac
	ndef_msg->msg[p++] = 0x10;
	ndef_msg->msg[p++] = 0x20;
	ndef_msg->msg[p++] = 0x00;
	ndef_msg->msg[p++] = 0x06;
	ndef_msg->msg[p++] = 0xFF;
	ndef_msg->msg[p++] = 0xFF;
	ndef_msg->msg[p++] = 0xFF;
	ndef_msg->msg[p++] = 0xFF;
	ndef_msg->msg[p++] = 0xFF;
	ndef_msg->msg[p++] = 0xFF;
	
	// layer 1: ndef message end
	ndef_msg->msg[p++] = 0xFE;
	
	ndef_msg->msg_len = p;
}
*/