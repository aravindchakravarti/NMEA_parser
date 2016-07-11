#include "stdio.h"
#include "nmea_parser.h"


void 				zdaMsgExtract 	(INT8 *, void *);
void				ggaMsgExtract	(INT8 *, void *);
void 				hexToDecimal 	(INT8 *, INT16 , INT8 *);
msg_callback_str* 	nmeaHeaderFinder(INT8 *);
INT32 				nmeaTimeExtract (msg_ZDA_str *, INT8 *, INT8 *);

msg_ZDA_str msg_ZDA[] = {	{"$GPZDA",0,0,0,0,0,0},
							{"$GLZDA",0,0,0,0,0,0},
							/* Add any other message you need here. Ex:
							{"$IRZDA, 0,0,0,0,0,0} -> for IRNSS */
						};
						
msg_GGA_str msg_GGA[] = {	{"$GPGGA",0,0,0,0,0,0,0,0,0,0,0,0,0},
							{"$GLGGA",0,0,0,0,0,0,0,0,0,0,0,0,0},
							/* Add any other message you need here. Ex:
							{"$IRGGA, 0,0,0,0,0,0} -> for IRNSS */
						};
			
							
msg_callback_str msg_callback[]= {	{"$GPZDA", &msg_ZDA[0], &zdaMsgExtract},
									{"$GLZDA", &msg_ZDA[1], &zdaMsgExtract},
									/* Add any other message you need here. Ex:
									{"$IRZDA, *msg_ZDA[2], &zdaMsgExtract} -> for IRNSS */
									{"$GPGGA", &msg_GGA[0], &ggaMsgExtract},
									{"$GLGGA", &msg_GGA[1], &ggaMsgExtract}
								};

msg_callback_str *msg_callback_ptr;		
						
UINT16	number_of_msgs = sizeof(msg_callback)/sizeof(msg_callback_str);
									
main ()
{
	FILE 	*fp;
	INT8 	ch		= 0;
	UINT8 	count	= 0;
	INT8	NMEA_string[80];
		
	printf ("\nHi\nWelcome to NMEA parser\n");
		
	fp = fopen ("nmea.txt","r");
	
	if (fp == NULL)
	{
		printf ("File open ERROR!\n");
	}
	else
	{
		fscanf(fp, "%s", NMEA_string);
	}
	
	msg_callback_ptr = nmeaHeaderFinder (&NMEA_string[0]);
	
	printf ("\n****\t%s\t****\n", msg_callback_ptr->header);
	
	msg_callback_ptr->callBackFunction(NMEA_string, msg_callback_ptr->msg_struct_ptr);
	
}


msg_callback_str* nmeaHeaderFinder (INT8 *NMEA_string)
{
	INT8 	header[7];
	INT8	index	= 0;
	INT32 	return_value = 0;
	
	msg_callback_str *NMEA_info_ptr;
	
	NMEA_info_ptr = &msg_callback[0];
	
	for (index = 0; index <6; index++)
	{
		header[index] = *NMEA_string++;
	}
	
	header[index] = '\0';
	

	printf ("\nMy Current number of messages are : %d\n", number_of_msgs);
	
	
	for (index = 0; index < number_of_msgs; index++)
	{
		//LocalStringCompare (&header[0], NMEA_string);
		return_value = strcmp (header, NMEA_info_ptr->header);
		if (return_value == 0)
		{
			/* We found the message */
			break;
		}
		else
		{
			/* Increment and point to next structure element */
			NMEA_info_ptr++; 		
		}
	}
	
	return (NMEA_info_ptr);
}

void zdaMsgExtract (INT8 *NMEA_string, void *msg_struct_ptr)
{
	INT8 time_holder[10] 	= {0};			/*Optimize these values if required */
	INT8 date_holder[10] 	= {0};
	INT16 index 			= 0;
	msg_ZDA_str *msg_ZDA_str_ptr;
		
	msg_ZDA_str_ptr = (msg_ZDA_str *)msg_struct_ptr;
	
	printf ("\n~~~~\t%s\t~~~~~~~~~\n", msg_ZDA_str_ptr->label);
	
	while (*NMEA_string++ != ',');
		
	hexToDecimal(NMEA_string, 9, time_holder);
	
	while (*NMEA_string++ != ',');
	
	hexToDecimal(NMEA_string, 10, date_holder);
	
	nmeaTimeExtract (msg_ZDA_str_ptr, time_holder, date_holder);
		
	printf ("From ZDA\n");
	printf ("Hour = %d\n", msg_ZDA_str_ptr->hour);
	printf ("Min  = %d\n", msg_ZDA_str_ptr->min);
	printf ("Sec  = %d\n", msg_ZDA_str_ptr->sec);
	printf ("Day  = %d\n", msg_ZDA_str_ptr->day);
	printf ("Month= %d\n", msg_ZDA_str_ptr->month);
	printf ("Year = %d\n", msg_ZDA_str_ptr->year);
	
} 

void ggaMsgExtract (INT8 *NMEA_string, void *msg_struct_ptr)
{
	INT8 time_holder[10] 	= {0};
	INT8 lat_holder	[20] 	= {0};
	INT8 long_holder[20] 	= {0};
	INT8 alt_holder [10] 	= {0};
	INT8 fix_holder	[2]		= {0};
	INT8 sats_count	[2]		= {0};
	INT16 index 			= 0;
	f_32 temp_lat_sec		= 0;
	msg_GGA_str *msg_GGA_str_ptr;
	
	
	msg_GGA_str_ptr = (msg_GGA_str *)msg_struct_ptr;
	
	printf ("\n~~~~\t%s\t~~~~~~~~~\n", msg_GGA_str_ptr->label);
	
	/* Time Extraction */
	
	while (*NMEA_string++ != ',');
	
	hexToDecimal(NMEA_string, 9, time_holder);
	
	msg_GGA_str_ptr->hour	=	(time_holder[0]*10) + time_holder[1];
	msg_GGA_str_ptr->min	=	(time_holder[2]*10) + time_holder[3];
	msg_GGA_str_ptr->sec	=	(time_holder[4]*10) + time_holder[5];
	
	/* Latittude extraction */
	
	while (*NMEA_string++ != ',');
	
	hexToDecimal(NMEA_string, 9, lat_holder);
	
	msg_GGA_str_ptr->lat_deg	=	(lat_holder[0]*10) + lat_holder[1];
	msg_GGA_str_ptr->lat_min	=	(lat_holder[2]*10) + lat_holder[3];
	
	temp_lat_sec	=	(lat_holder[4]*1000) + (lat_holder[5]*100) + \
						(lat_holder[6]*10) + lat_holder[7];
						
	temp_lat_sec = ((temp_lat_sec/10000) * 180) / 3.142;
	
	msg_GGA_str_ptr->lat_sec = (INT16)temp_lat_sec;		

	
	while (*NMEA_string++ != ',');
	
	msg_GGA_str_ptr->lat_dir	=	*NMEA_string;
	
	/* Longitude extraction */
	
	while (*NMEA_string++ != ',');
	
	hexToDecimal(NMEA_string, 9, long_holder);
	
	msg_GGA_str_ptr->lon_deg	=	(long_holder[0]*100) + (long_holder[1]*10) + long_holder[2];
	msg_GGA_str_ptr->lon_min	=	(long_holder[3]*10) + long_holder[4];
	
	temp_lat_sec	=	(long_holder[5]*1000) + (long_holder[6]*100) + \
						(long_holder[7]*10) + long_holder[8];
						
	temp_lat_sec 	= 	((temp_lat_sec/10000) * 180) / 3.142;
	
	msg_GGA_str_ptr->lon_sec = (INT16)temp_lat_sec;		

	
	while (*NMEA_string++ != ',');
	
	msg_GGA_str_ptr->lon_dir	=	*NMEA_string;
	
	/* Fix extraction */
	
	while (*NMEA_string++ != ',');
	
	hexToDecimal(NMEA_string, 1, fix_holder);
	
	msg_GGA_str_ptr->fix	=	fix_holder[0];
	
	/* Number of satellite extraction */
	
	while (*NMEA_string++ != ',');
	
	hexToDecimal(NMEA_string, 2, sats_count);
	
	msg_GGA_str_ptr->number_of_sats	=	(sats_count[0]*10) + sats_count[1];
	
	/* Skipping HDOP, Left for future */
		
	while (*NMEA_string++ != ',');
	
	/* Altitude extraction */
	
	while (*NMEA_string++ != ',');
	
	hexToDecimal(NMEA_string, 7, alt_holder);
	
	msg_GGA_str_ptr->altitude = (alt_holder[0]*10000) * (alt_holder[1]*1000)+\
					(alt_holder[2]*100) + (alt_holder[3]*10) + alt_holder[4]; 
		
	/* Printing results */
	
	printf ("Hour\t:\t%d\n", msg_GGA_str_ptr->hour);
	printf ("Min\t:\t%d\n", msg_GGA_str_ptr->min);
	printf ("Sec\t:\t%d\n", msg_GGA_str_ptr->sec);
	
	printf ("Lat\t:\t%d%c%d%c%d%c %c\n", msg_GGA_str_ptr->lat_deg,167,\
		msg_GGA_str_ptr->lat_min,39, msg_GGA_str_ptr->lat_sec, 34,\
		msg_GGA_str_ptr->lat_dir);
		
	printf ("Lat\t:\t%d%c%d%c%d%c %c\n", msg_GGA_str_ptr->lon_deg,167,\
		msg_GGA_str_ptr->lon_min,39, msg_GGA_str_ptr->lon_sec, 34,\
		msg_GGA_str_ptr->lon_dir);
		
	printf ("Alt\t:\t%dm\n", msg_GGA_str_ptr->altitude);
		
	printf ("Fix\t:\t%d\n", msg_GGA_str_ptr->fix);
	
	printf ("Sats\t:\t%d\n", msg_GGA_str_ptr->number_of_sats);
 
	
} 



INT32 nmeaTimeExtract (msg_ZDA_str *msg_struct_ptr, INT8 *time_holder, INT8 *date_holder)
{
	UINT16	time_valid 	= 0;
	UINT16	date_valid	= 0;	

	msg_struct_ptr->hour	=	(time_holder[0] * 10) + time_holder[1];
	msg_struct_ptr->min		=	(time_holder[2] * 10) + time_holder[3];
	msg_struct_ptr->sec		=	(time_holder[4] * 10) + time_holder[5];
	
	msg_struct_ptr->day		=	(date_holder[0] * 10) + date_holder[1];
	msg_struct_ptr->month	=	(date_holder[2] * 10) + date_holder[3];
	msg_struct_ptr->year	=	(date_holder[4] * 1000) + (date_holder[5] * 100)\
								  + (date_holder[6] * 10) + (date_holder[7] * 1);
								  
								  	
	if ((msg_struct_ptr->hour >= 0) && (msg_struct_ptr->hour <= 24))
	{
		if ((msg_struct_ptr->min >= 0) && (msg_struct_ptr->min <= 60))
		{
			if ((msg_struct_ptr->sec >= 0) && (msg_struct_ptr->sec <= 60))
			{
				time_valid = 1;
			}
		}
	}
	
	if ((msg_struct_ptr->day >= 0) && (msg_struct_ptr->day <= 31))			/* February?? and otherthings?? Add if needed */
	{
		if ((msg_struct_ptr->month >= 0) && (msg_struct_ptr->month <= 60))
		{
			if ((msg_struct_ptr->year >= 1970) && (msg_struct_ptr->year <= 2100))
			{
				date_valid = 1;
			}
		}
	} 	
	
	
	if (time_valid & date_valid)
	{
		return (1);
	}			
	
	else 
	{
		return (ERR);
	}						
}


void hexToDecimal (INT8 *NMEA_string, INT16 number_of_bytes, INT8 *decimal_array)
{
	printf ("\n");
	while (number_of_bytes)
	{
		if (*NMEA_string >= 'a' && *NMEA_string <= 'f')
		{
			*decimal_array = *NMEA_string - 'a' + 10;
			printf ("I should never come here");
		}
		
		else if (*NMEA_string >= '0' && *NMEA_string <= '9')
		{
			*decimal_array = *NMEA_string - '0';
		}
		
		else
		{
			/* Do nothing just go to next byte*/
			number_of_bytes--;
			NMEA_string++;
			
			/* Here comes the magic! it skips next few lines if it is '.'
			or ',' or something else!! */
			continue; 
		}
		
		number_of_bytes--;
		NMEA_string++;
		decimal_array++;
		
	}
}

