#ifndef __NDEF_SIMPLE_WIFI_H__
#define __NDEF_SIMPLE_WIFI_H__

#include <stdint.h>

#define SYNC_TYPE_WIFI				1
#define SYNC_TYPE_FORMAT 			2
#define SYNC_TYPE_NONE 				0

#define NDEF_WIFI_ID_SSID_FIELD				0x1045
#define NDEF_WIFI_ID_NETWORK_KEY_FIELD		0x1027
#define NDEF_WIFI_ID_MAC_ADDRESS			0x1020
#define NDEF_WIFI_ID_AUTH_TYPE_FIELD		0x1003
#define NDEF_WIFI_ID_ENCRYPTION_TYPE		0x100F
#define NDEF_WIFI_ID_AUTH_TYPE_OPEN			0x01
#define NDEF_WIFI_ID_AUTH_TYPE_WPA_PSK		0x02
#define NDEF_WIFI_ID_AUTH_TYPE_WPA_EAP		0x08
#define NDEF_WIFI_ID_AUTH_TYPE_WPA2_EAP		0x10
#define NDEF_WIFI_ID_AUTH_TYPE_WPA2_PSK		0x20
#define NDEF_WIFI_ID_ENCRYPT_TYPE_NONE		0x01
#define NDEF_WIFI_ID_ENCRYPT_TYPE_WEP		0x02
#define NDEF_WIFI_ID_ENCRYPT_TYPE_TKIP		0x04
#define NDEF_WIFI_ID_ENCRYPT_TYPE_AES		0x08
#define NDEF_WIFI_ID_ENCRYPT_TYPE_MMODE		0x0c  //AES/TKIP Mixed Mode  

#define MAC_ADDR_LEN 6
#define MAX_WIFI_PWD_LEN 63
#define MAX_WIFI_SSID_LEN 32
#define MAX_WIFI_MAC_CHAR_LEN 17
#define MAX_NDEF_MSG_LEN_ALIGNED 176		// aligned to 16 (block size)
#ifndef NT3H_BYTES_PER_BLOCK
#define NT3H_BYTES_PER_BLOCK	16
#endif
#define MAX_FORMAT_BLOCKS ((MAX_NDEF_MSG_LEN_ALIGNED)/(NT3H_BYTES_PER_BLOCK))
#define NDEF_MSG_LEN_WITHOUT_SSIDPWD 67
#define NDEF_PAYLOAD_LEN_WITHOUT_SSIDPWD 39

extern __xdata uint8_t	glb_wifi_ssid[MAX_WIFI_SSID_LEN];
extern __xdata uint8_t	glb_ssid_len;
extern __xdata uint8_t	glb_pwd[MAX_WIFI_PWD_LEN];
extern __xdata uint8_t	glb_pwd_len;
extern __xdata uint8_t	glb_auth;
extern __xdata uint8_t	glb_enc;
extern __xdata uint8_t	glb_needs_sync;
extern __xdata uint8_t	glb_msg[MAX_NDEF_MSG_LEN_ALIGNED]; 
extern __xdata uint8_t	glb_msg_len;

void generate_simple_ndef_wifi_message();

/*
typedef struct wifi_config_s {
	uint8_t   ssid[MAX_WIFI_SSID_LEN]; 
	uint8_t   ssid_len;
	uint8_t   pwd[MAX_WIFI_PWD_LEN];
	uint8_t   pwd_len;
	uint8_t   auth;
	uint8_t   enc;
	uint8_t   mac[MAC_ADDR_LEN];
	uint8_t   needs_sync;
} wifi_config_t;

typedef struct ndef_wifi_msg_s {
	uint8_t   msg[MAX_NDEF_MSG_LEN_ALIGNED]; 
	uint8_t   msg_len;
} ndef_wifi_msg_t;
*/
//uint8_t generate_simple_ndef_wifi_message (wifi_config_t* wifi_conf, ndef_wifi_msg_t* ndef_msg);
//void generate_simple_ndef_wifi_message (wifi_config_t* wifi_conf, ndef_wifi_msg_t* ndef_msg);
//uint8_t convert_text_to_wifi_conf(uint8_t* txt, uint8_t txt_len, wifi_config_t* wifi_conf);

#endif
