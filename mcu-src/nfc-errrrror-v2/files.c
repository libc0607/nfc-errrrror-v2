/*
	Files on USB mass storage 
	Since I dont want to write or merge sth. like "jsonparser" or "iniparser" into ch55x
	so I decided to storage different configs to different files to keep it simple.
	
	Github: libc0607/nfc-errrrror-v2

	Usage when writing a new Wi-Fi config: 
	1. write <ssid>, <pwd>, <auth>, <enc> to files
		ssid: 32 Bytes max, read until first '\r' or '\n'
		pwd: 8~63 Bytes, read until first '\r' or '\n'
		auth: [open, wpapsk, wpaeap, wpa2eap, wpa2psk] or UPPERCASE
		enc: [none, wep, tkip, aes, aestkip] or UPPERCASE
	2. sync
	3. write '1' to sync.txt
	4. sync
	
	Or, formatting the ntag:
	1. write 'F' to sync.txt
	2. sync
	
	It's an one-way transfer, configs cannot be read out by opening the files (for now).
*/

#include "USBMassStorage.h"
#include "ndef_simple_wifi.h"

// Loooooong static file will cause reset on boot, who tm knows why
__code const uint8_t filecontent_readme[] = "github.com/libc0607/nfc-errrrror-v2 \n"; 
__code const uint8_t filecontent_wifissid[] = "";
__code const uint8_t filecontent_wifipwd[] = "";
__code const uint8_t filecontent_wifiauth[] = "";
__code const uint8_t filecontent_wifienc[] = "";
__code const uint8_t filecontent_sync[] = "";

// Note: 
// 1. all cb_write_wifi_* save config to glb_wifi_config
// 2. Because all configs are shorter than BULK_MAX_PACKET_SIZE (64 bytes by default)
//    so we only need to skip the package when offset!=0. this saves a lot of code
void cb_write_wifi_ssid (uint16_t offset) 
{
	// only get ssid from the start of the file 
	if (offset != 0)
		return;
	
	if (glb_needs_sync != SYNC_TYPE_NONE)
		return;

	for (uint8_t i=0; i<MAX_WIFI_SSID_LEN; i++) {
		if ((BOT_Rx_Buf[i] != '\r') && (BOT_Rx_Buf[i] != '\n')) {
			glb_wifi_ssid[i] = BOT_Rx_Buf[i];
		} else {
			glb_ssid_len = i;
			return;
		}
	}
}

void cb_write_wifi_pwd (uint16_t offset) 
{
	// only get contents from the start of the file 
	if (offset != 0)
		return;
	
	if (glb_needs_sync != SYNC_TYPE_NONE)
		return;
	
	for (uint8_t i=0; i<MAX_WIFI_PWD_LEN; i++) {
		if ((BOT_Rx_Buf[i] != '\r') && (BOT_Rx_Buf[i] != '\n')) {
			glb_pwd[i] = BOT_Rx_Buf[i];
		} else {
			glb_pwd_len = i;
			return;
		}
	}
}

void cb_write_wifi_auth (uint16_t offset) 
{
	// only get ssid from the start of the file 
	if (offset != 0)
		return;

	if (glb_needs_sync != SYNC_TYPE_NONE)
		return;

	// set default to open
	glb_auth = NDEF_WIFI_ID_AUTH_TYPE_OPEN;

	// type 2: wpapsk
	if ( (BOT_Rx_Buf[3]=='p') || (BOT_Rx_Buf[3]=='P') ) {
		glb_auth = NDEF_WIFI_ID_AUTH_TYPE_WPA_PSK;
	} 
	// type 3: wpaeap
	if ( (BOT_Rx_Buf[3]=='e') || (BOT_Rx_Buf[3]=='E')) {
		glb_auth = NDEF_WIFI_ID_AUTH_TYPE_WPA_EAP;
	} 
	// type 4: wpa2eap
	if ( (BOT_Rx_Buf[4]=='e') || (BOT_Rx_Buf[4]=='E')) {
		glb_auth = NDEF_WIFI_ID_AUTH_TYPE_WPA2_EAP;
	}
	// type 5: wpa2psk
	if ( (BOT_Rx_Buf[4]=='p') || (BOT_Rx_Buf[4]=='P')) {
		glb_auth = NDEF_WIFI_ID_AUTH_TYPE_WPA2_PSK;
	}
}

void cb_write_wifi_enc (uint16_t offset) 
{
	// only get ssid from the start of the file 
	if (offset != 0)
		return;
	
	if (glb_needs_sync != SYNC_TYPE_NONE)
		return;

	// set default to open
	glb_enc = NDEF_WIFI_ID_ENCRYPT_TYPE_NONE;

	// type 2: wep
	if ( (BOT_Rx_Buf[0]=='w') || (BOT_Rx_Buf[0]=='W') ) {
		glb_enc = NDEF_WIFI_ID_ENCRYPT_TYPE_WEP;
	} 
	// type 3: tkip
	if ( (BOT_Rx_Buf[0]=='t') || (BOT_Rx_Buf[0]=='T')) {
		glb_enc = NDEF_WIFI_ID_ENCRYPT_TYPE_TKIP;
	} 
	// type 4: aes
	if ( (BOT_Rx_Buf[0]=='a') || (BOT_Rx_Buf[0]=='A')) {
		glb_enc = NDEF_WIFI_ID_ENCRYPT_TYPE_AES;
	}
	// type 5: aestkip
	if ( (BOT_Rx_Buf[3]=='t') || (BOT_Rx_Buf[3]=='T')) {
		glb_enc = NDEF_WIFI_ID_ENCRYPT_TYPE_MMODE;
	}
}

void cb_write_sync (uint16_t offset) 
{
	if ( (offset == 0) && (BOT_Rx_Buf[0] == '1') ) {
	  glb_needs_sync = SYNC_TYPE_WIFI;
	} 
	else if ( (offset == 0) && (BOT_Rx_Buf[0] == 'F') ) {
	  glb_needs_sync = SYNC_TYPE_FORMAT;
	} 
	else { 
	  glb_needs_sync = SYNC_TYPE_NONE;
	}
}

__code File_Entry filesOnDrive[] = {  //keep filename UPPERCASE
  {
    .filename = {'R', 'E', 'A', 'D', 'M', 'E', ' ', ' ', 'T', 'X', 'T'},
    .filetime = {TIME_LB(12, 34, 56), TIME_HB(12, 34, 56)},
    .filedate = {DATE_LB(2021, 1, 2), DATE_HB(2021, 1, 2)},
    .filesize = sizeof(filecontent_readme),
    .fileCallBackType = CONST_DATA_FILE,
    .fileReadHandler = {.constPtr = filecontent_readme},
    .fileWriteHandler = 0,  //NULL
  },
  {
    .filename = {'W', 'I', 'F', 'I', 'S', 'S', 'I', 'D', 'T', 'X', 'T'},
    .filetime = {TIME_LB(12, 34, 56), TIME_HB(12, 34, 56)},
    .filedate = {DATE_LB(2021, 1, 2), DATE_HB(2021, 1, 2)},
    .filesize = sizeof(filecontent_wifissid),
    .fileCallBackType = CONST_DATA_FILE,
    .fileReadHandler = {.constPtr = filecontent_wifissid},
    .fileWriteHandler = cb_write_wifi_ssid, 
  },
  {
    .filename = {'W', 'I', 'F', 'I', 'P', 'W', 'D', ' ', 'T', 'X', 'T'},
    .filetime = {TIME_LB(12, 34, 56), TIME_HB(12, 34, 56)},
    .filedate = {DATE_LB(2021, 1, 2), DATE_HB(2021, 1, 2)},
    .filesize = sizeof(filecontent_wifipwd),
    .fileCallBackType = CONST_DATA_FILE,
    .fileReadHandler = {.constPtr = filecontent_wifipwd},
    .fileWriteHandler = cb_write_wifi_pwd,  
  },  
  {
    .filename = {'W', 'I', 'F', 'I', 'A', 'U', 'T', 'H', 'T', 'X', 'T'},
    .filetime = {TIME_LB(12, 34, 56), TIME_HB(12, 34, 56)},
    .filedate = {DATE_LB(2021, 1, 2), DATE_HB(2021, 1, 2)},
    .filesize = sizeof(filecontent_wifiauth),
    .fileCallBackType = CONST_DATA_FILE,
    .fileReadHandler = {.constPtr = filecontent_wifiauth},
    .fileWriteHandler = cb_write_wifi_auth,  
  },  
  {
    .filename = {'W', 'I', 'F', 'I', 'E', 'N', 'C', ' ', 'T', 'X', 'T'},
    .filetime = {TIME_LB(12, 34, 56), TIME_HB(12, 34, 56)},
    .filedate = {DATE_LB(2021, 1, 2), DATE_HB(2021, 1, 2)},
    .filesize = sizeof(filecontent_wifienc),
    .fileCallBackType = CONST_DATA_FILE,
    .fileReadHandler = {.constPtr = filecontent_wifienc},
    .fileWriteHandler = cb_write_wifi_enc,  
  },
  {
    .filename = {'S', 'Y', 'N', 'C', ' ', ' ', ' ', ' ', 'T', 'X', 'T'},
    .filetime = {TIME_LB(12, 34, 56), TIME_HB(12, 34, 56)},
    .filedate = {DATE_LB(2021, 1, 2), DATE_HB(2021, 1, 2)},
    .filesize = sizeof(filecontent_sync),
    .fileCallBackType = CONST_DATA_FILE,
    .fileReadHandler = {.constPtr = filecontent_sync},
    .fileWriteHandler = cb_write_sync,  
  }
};

__code uint8_t filesOnDriveCount = sizeof(filesOnDrive) / sizeof(File_Entry);
