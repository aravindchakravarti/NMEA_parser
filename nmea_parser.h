#ifndef _NMEA_PARSER_H_
#define	_NMEA_PARSER_H_

/* Common types */
typedef unsigned int 	UINT32;
typedef unsigned short 	UINT16;
typedef unsigned char	UINT8;
typedef int 			INT32;
typedef short 			INT16;
typedef char			INT8;
typedef float			f_32;

#define					ERR		-1

/* This structure holds time and date from xxZDA*/
typedef struct
{
	UINT8		label[7];
	UINT16		hour;
	UINT16		min;
	UINT16		sec;
	UINT16		day;
	UINT16		month;
	UINT16		year;
}msg_ZDA_str;

/* This structure holds time and date from xxGGA*/
typedef struct
{
	UINT8		label[7];
	UINT8		lat_dir;
	UINT8		lon_dir;
	UINT16		hour;
	UINT16		min;
	UINT16		sec;
	UINT16		lat_deg;
	UINT16		lat_min;
	UINT16		lat_sec;
	UINT16		lon_deg;
	UINT16		lon_min;
	UINT16		lon_sec;
	UINT16		altitude;
	UINT16		fix;
	UINT16		number_of_sats;
}msg_GGA_str;

/* This structure holds call back functions and structure pointers 
for the input string decoding */
typedef	struct
{
	UINT8		header[7];
	void		*msg_struct_ptr;
	void		(*callBackFunction) (INT8* , void*);
}msg_callback_str;

#endif