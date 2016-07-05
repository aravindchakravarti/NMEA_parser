#ifndef _NMEA_PARSER_H_
#define	_NMEA_PARSER_H_

typedef unsigned int 	UINT32;
typedef unsigned short 	UINT16;
typedef unsigned char	UINT8;
typedef int 			INT32;
typedef short 			INT16;
typedef char			INT8;

void				gpzdaMsgExtract		(INT8 *);
void				gpggaMsgExtract		(INT8 *);

typedef struct 
{
	INT8	msg[7];
	UINT8 	hour;
	UINT8 	min;
	UINT8 	sec;
	UINT8 	day;
	UINT8 	month;
	UINT8	lat;
	UINT8	lat_dir;
	UINT8	lon;
	UINT8	lon_dir;
	UINT16 	year;
	UINT16	alt;
	void	(*nmeaMsgDataExtract) (INT8*);
}NMEA_data_stuct;

NMEA_data_stuct NMEA_info[] = 
{
	{
		"$GPZDA", \
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
		&gpzdaMsgExtract,
	}
	,
	{
		"$GPGGA", \
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
		&gpggaMsgExtract,
	}
};

NMEA_data_stuct *NMEA_data_stuct_ptr;

enum NMEA_message_enum 
{
	GPZDA, GPZDG, GPGGA, GPRMC, GLZDA, GLZDG, GLGGA, GLRMC
};

#endif
