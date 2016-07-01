#include "stdio.h"

typedef unsigned int 	UINT32;
typedef unsigned short 	UINT16;
typedef unsigned char	UINT8;
typedef char			INT8;

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
}NMEA_data_stuct;

NMEA_data_stuct NMEA_info[] = 
{
	{
		.msg = "GPZDA", 
		.0,
		.0,
		.0,
		.0,
		.0,
		.0,
		.0,
		.0,
		.0,
		.0,
		.0,
	},
	{
		.msg = "GPGGA", 
		.0,
		.0,
		.0,
		.0,
		.0,
		.0,
		.0,
		.0,
		.0,
		.0,
		.0,	
	}
	
};

enum NMEA_message_enum 
{
	GPZDA,
	GPZDG,
	GPGGA,
	GPRMC,
	GLZDA,
	GLZDG,
	GLGGA,
	GLRMC
};

enum NMEA_message_enum NMEA_message;

void NMEAInit 			(void);
void NMEAMsgExtractor 	(INT8 *);

main ()
{
	FILE 	*fp;
	INT8 	ch		= 0;
	UINT8 	count	= 0;
	INT8	NMEA_string[80];
	
	INT8	*hi;
	
	hi = "Hello";
	
	printf ("%s", hi);
	
	
	printf ("\nWelcome to NMEA parser\n");
	
	fp = fopen ("nmea.txt","r");
	
	if (fp == NULL)
	{
		printf ("File open ERROR!\n");
	}
	else
	{
		fscanf(fp, "%s", NMEA_string);
	}

	NMEAInit();
	
	NMEAMsgExtractor(&NMEA_string[0]);
}

void NMEAInit (void)
{
	
}

void NMEAMsgExtractor (INT8 *NMEA_string)
{
	INT8 	header[6];
	INT8	index	= 0;
	NMEA_data_stuct *NMEA_info_ptr;
	
	NMEA_info_ptr = &NMEA_info[0];
	
	for (index = 0; index <6; index++)
	{
		header[index] = *NMEA_string++;
	}
	
	printf ("****\n");
	printf ("%s", NMEA_info_ptr->msg);/**NMEA_info_ptr->msg); */
	
}
