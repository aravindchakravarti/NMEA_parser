/*
 ************************************************************************************************
 * License Information  	:		GPL
 *************************************************************************************************
 *
 * File Information
 *    File name				:  		nmea_parser.c
 *    Abstract             	:		This file extracts the NMEA 0183 standard GNSS messages. 
 *									In the current implementation, this software can "de-compose" 
 *									ZDA and GGA message, which is sufficient for time, date, position 
 *									information
 *									However software is written in such a way that, it can be easily 
 *									scalable to other messages too.
 *
 * Compilation Information
 *    Compiler/Assembler   	:   	Currently used DEV_C++. But, with minimum modifications, it is po-
 *									ssible to import it to embedded applications too.
 *    Version              	:   	5.5.3
 *    
 *
 * Hardware Platform       	:   	Generic. (Tested on 32 bit)
 *
 *  Author(s)           	:   	Aravind D Chakravarti 
 *									Email: aravind.chakravarti@gmail.com
 *                          	
 * <Version Number>  <Author>  	<date>		<Change Request>	<Modification(s)>
 *  1.0             Aravind    	12-07-2016			--			Base Version
 *	                         
 * Functions				:	msg_callback_str* nmeaHeaderFinder (INT8 *NMEA_string)
 *								void zdaMsgExtract (INT8 *NMEA_string, void *msg_struct_ptr)
 *								void ggaMsgExtract (INT8 *NMEA_string, void *msg_struct_ptr)
 *								INT32 nmeaTimeExtract (msg_ZDA_str *msg_struct_ptr, INT8 *time_holder,\
 *														INT8 *date_holder)
 *								void asciiToDecimal (INT8 *NMEA_string, INT16 number_of_bytes,\
 *														INT8 *decimal_array)
 *
 * Known issues/limitations	:	Extracts only ZDA and GGA messages
 *								Floats values such as time as purposefully considered as integers, but
 *								very easy to change/update if required
 * 
 ***************************************************************************************************	
 */

 /* Header files */
#include "stdio.h"
#include "nmea_parser.h"		/* Definitions of structures */

/* Functions prototypes */
void 				zdaMsgExtract 	(INT8 *, void *);
void				ggaMsgExtract	(INT8 *, void *);
void 				asciiToDecimal 	(INT8 *, INT16 , INT8 *);
msg_callback_str* 	nmeaHeaderFinder(INT8 *);
INT32 				nmeaTimeExtract (msg_ZDA_str *, INT8 *, INT8 *);

/* This structure array initializes ZDA message array.
This structure array holds all time structures of ZDA message from
GPS and GLONASS */
msg_ZDA_str msg_ZDA[] = {	{"$GPZDA",0,0,0,0,0,0},
							{"$GLZDA",0,0,0,0,0,0},
							/* Add any other message you need here. Ex:
							{"$IRZDA, 0,0,0,0,0,0} -> for IRNSS */
						};

/* Similarly for GGA message */
msg_GGA_str msg_GGA[] = {	{"$GPGGA",0,0,0,0,0,0,0,0,0,0,0,0,0},
							{"$GLGGA",0,0,0,0,0,0,0,0,0,0,0,0,0},
							/* Add any other message you need here. Ex:
							{"$IRGGA, 0,0,0,0,0,0} -> for IRNSS */
						};
			
/* Hmmm... What is this? This structure contains all call back functions.
And to that call back function it will also send the respective structure 
address which have been declared above */							
msg_callback_str msg_callback[]= {	{"$GPZDA", &msg_ZDA[0], &zdaMsgExtract},
									{"$GLZDA", &msg_ZDA[1], &zdaMsgExtract},
									/* Add any other message you need here. Ex:
									{"$IRZDA, *msg_ZDA[2], &zdaMsgExtract} -> for IRNSS */
									{"$GPGGA", &msg_GGA[0], &ggaMsgExtract},
									{"$GLGGA", &msg_GGA[1], &ggaMsgExtract}
								};

/* Pointer to above structure */
msg_callback_str *msg_callback_ptr;		

/* In order to have 'Searching' we need to know how many messages have been available to 
decode */				
UINT16	number_of_msgs = sizeof(msg_callback)/sizeof(msg_callback_str);

/*
* Function Information
*
* Function Name				:		main 
*
* Function Description		:		Entry point for 'C'.
*									This is a test implementation, in this, program reads
*									the data from a file. Pointed by file pointer.
*
* Parameters passed			:		None
*
* Parameters returned		:		None
*
* Functions called			:		nmeaHeaderFinder
*
* Global variables used		:		*msg_callback_ptr
*
* Global variables modified	:		*msg_callback_ptr
*
*/									
main ()
{
	FILE 	*fp;
	INT8 	ch		= 0;
	UINT8 	count	= 0;
	INT8	NMEA_string[80];
	
	printf ("\nHi\nWelcome to NMEA parser\n");
	
	/* File pointer, pointing to text file which holds NMEA message */
	fp = fopen ("nmea.txt","r");
	
	if (fp == NULL)
	{
		printf ("File open ERROR!\n");
	}
	else
	{
		/* Read the NMEA message from file */
		fscanf(fp, "%s", NMEA_string);
	}
	
	/* Check which NMEA message is there in the NMEA_string[], later
	call corresponding NMEA message extraction function*/
	msg_callback_ptr = nmeaHeaderFinder (&NMEA_string[0]);
	
	printf ("\n****\t%s\t****\n", msg_callback_ptr->header);
	
	/* Call function which should extract input message. Also , send 
	corresponding structure in which program can save its data */
	msg_callback_ptr->callBackFunction(NMEA_string, msg_callback_ptr->msg_struct_ptr);
	
}

/*
* Function Information
*
* Function Name				:		nmeaHeaderFinder 
*
* Function Description		:		This function finds the header of the message sent to it.
*									It returns the structure pointer in which message extract 
*									function will be present.
*
* Parameters passed			:		INT8 *NMEA_string: This array pointer will be pointing 
*											to the first letter of NMEA message which is read
*											by file pointer
*
* Parameters returned		:		msg_callback_str*: This structure pointer will be pointing
*											to the respective message structure. E.g., if GPZDA
*											message is sent to this function, then it will be
*											pointing to  msg_callback[0].
*
* Functions called			:		nmeaHeaderFinder
*
* Global variables used		:		*msg_callback_ptr
*
* Global variables modified	:		*msg_callback_ptr
*
*/
msg_callback_str* nmeaHeaderFinder (INT8 *NMEA_string)
{
	INT8 	header[7];
	INT8	index	= 0;
	INT32 	return_value = 0;
	
	/*pointer to the type msg_callback_str structure */
	msg_callback_str *NMEA_info_ptr;
	
	/* We will begin with searching for "$GPZDA" */
	NMEA_info_ptr = &msg_callback[0];
	
	/* Copy first 6 bytes. Which is header in NMEA 0183 */
	for (index = 0; index <6; index++)
	{
		header[index] = *NMEA_string++;
	}
	
	/* Without this string compare won't work :) */
	header[index] = '\0';
	

	printf ("\nMy Current number of messages are : %d\n", number_of_msgs);
	
	/* Search till end of number of messages we can decode */
	for (index = 0; index < number_of_msgs; index++)
	{
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

/*
* Function Information
*
* Function Name				:		zdaMsgExtract 
*
* Function Description		:		This function extracts ZDA message which can be from either
*									GPS or GLONASS or any other if it is implemented.
* 									$GPZDA,	 115242.00,15,06,2015,-00,00*4E
* 									<header>,<Time>,<date>,<local time zone>*<checksum>
*
* Parameters passed			:		INT8 *NMEA_string: This array pointer will be pointing 
*											to the first letter of NMEA message which we have to 
*											decode
*									void *msg_struct_ptr: This is where we have to store the results
*											Later it has been typcasted to msg_ZDA_str*
*
* Parameters returned		:		None.
*
* Functions called			:		asciiToDecimal
*									nmeaTimeExtract
*
* Global variables used		:		msg_ZDA_str structure
*
* Global variables modified	:		msg_ZDA_str structure
*
*/
void zdaMsgExtract (INT8 *NMEA_string, void *msg_struct_ptr)
{
	INT8 time_holder[10] 	= {0};			/*Optimize these values if required */
	INT8 date_holder[10] 	= {0};
	INT16 index 			= 0;
	msg_ZDA_str *msg_ZDA_str_ptr;
		
	msg_ZDA_str_ptr = (msg_ZDA_str *)msg_struct_ptr;
	
	printf ("\n~~~~\t%s\t~~~~~~~~~\n", msg_ZDA_str_ptr->label);
	
	/*Skip header*/
	while (*NMEA_string++ != ',');
		
	/*Send next 9 bytes for ascii to decimal conversion */
	asciiToDecimal(NMEA_string, 9, time_holder);
	
	while (*NMEA_string++ != ',');
	
	asciiToDecimal(NMEA_string, 10, date_holder);
	
	/*Send time and date 'arrays'  to build time and date and fill it in ZDA 
	structure*/
	nmeaTimeExtract (msg_ZDA_str_ptr, time_holder, date_holder);
	
	/*Debug print */
	printf ("From ZDA\n");
	printf ("Hour = %d\n", msg_ZDA_str_ptr->hour);
	printf ("Min  = %d\n", msg_ZDA_str_ptr->min);
	printf ("Sec  = %d\n", msg_ZDA_str_ptr->sec);
	printf ("Day  = %d\n", msg_ZDA_str_ptr->day);
	printf ("Month= %d\n", msg_ZDA_str_ptr->month);
	printf ("Year = %d\n", msg_ZDA_str_ptr->year);
	
} 

/*
* Function Information
*
* Function Name				:		ggaMsgExtract 
*
* Function Description		:		This function extracts GGA message which can be from either
*									GPS or GLONASS or any other if it is implemented.
* 				$GPGGA,115242.00,1257.3421,N,07738.2280,E,1,09,00.8,00824.5,M,-088.4,M,,*48
* 				<header>,<Time>,<Latittude>,<Longitude>,<fix>,<HDOP>,<Altitude>,<MSL>,<*<checksum>
*
* Parameters passed			:		INT8 *NMEA_string: This array pointer will be pointing 
*											to the first letter of NMEA message which we have to 
*											decode
*									void *msg_struct_ptr: This is where we have to store the results
*											Later it has been typcasted to msg_GGA_str*
*
* Parameters returned		:		None.
*
* Functions called			:		asciiToDecimal
*									
* Global variables used		:		msg_GGA_str structure
*
* Global variables modified	:		msg_GGA_str structure
*
*/
void ggaMsgExtract (INT8 *NMEA_string, void *msg_struct_ptr)
{
	INT8 time_holder[10] 	= {0};		/*Temporaryly hold time */
	INT8 lat_holder	[20] 	= {0};		/*Latittude */
	INT8 long_holder[20] 	= {0};		/*So on.... */
	INT8 alt_holder [10] 	= {0};
	INT8 fix_holder	[2]		= {0};
	INT8 sats_count	[2]		= {0};
	INT16 index 			= 0;		
	f_32 temp_lat_sec		= 0;		/*We need to convert radian lat long to seconds*/
	msg_GGA_str *msg_GGA_str_ptr;
	
	
	msg_GGA_str_ptr = (msg_GGA_str *)msg_struct_ptr;
	
	printf ("\n~~~~\t%s\t~~~~~~~~~\n", msg_GGA_str_ptr->label);
	
	/* Time Extraction */
	
	while (*NMEA_string++ != ',');
	
	asciiToDecimal(NMEA_string, 9, time_holder);
	
	msg_GGA_str_ptr->hour	=	(time_holder[0]*10) + time_holder[1];
	msg_GGA_str_ptr->min	=	(time_holder[2]*10) + time_holder[3];
	msg_GGA_str_ptr->sec	=	(time_holder[4]*10) + time_holder[5];
	
	/* Latittude extraction */
	
	while (*NMEA_string++ != ',');
	
	asciiToDecimal(NMEA_string, 9, lat_holder);
	
	msg_GGA_str_ptr->lat_deg	=	(lat_holder[0]*10) + lat_holder[1];
	msg_GGA_str_ptr->lat_min	=	(lat_holder[2]*10) + lat_holder[3];
	
	temp_lat_sec	=	(lat_holder[4]*1000) + (lat_holder[5]*100) + \
						(lat_holder[6]*10) + lat_holder[7];
	
	/* Conversion from radian to seconds */
	temp_lat_sec = ((temp_lat_sec/10000) * 180) / 3.142;
	
	msg_GGA_str_ptr->lat_sec = (INT16)temp_lat_sec;		

	
	while (*NMEA_string++ != ',');
	
	msg_GGA_str_ptr->lat_dir	=	*NMEA_string;
	
	/* Longitude extraction */
	
	while (*NMEA_string++ != ',');
	
	asciiToDecimal(NMEA_string, 9, long_holder);
	
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
	
	asciiToDecimal(NMEA_string, 1, fix_holder);
	
	msg_GGA_str_ptr->fix	=	fix_holder[0];
	
	/* Number of satellite extraction */
	
	while (*NMEA_string++ != ',');
	
	asciiToDecimal(NMEA_string, 2, sats_count);
	
	msg_GGA_str_ptr->number_of_sats	=	(sats_count[0]*10) + sats_count[1];
	
	/* Skipping HDOP, Left for future */
		
	while (*NMEA_string++ != ',');
	
	/* Altitude extraction */
	
	while (*NMEA_string++ != ',');
	
	asciiToDecimal(NMEA_string, 7, alt_holder);
	
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

/*
* Function Information
*
* Function Name				:		ggaMsgExtract 
*
* Function Description		:		Very simple function just fills ZDA structure with extracted bytes
*
* Parameters passed			:		msg_ZDA_str *msg_struct_ptr : Structure to which we have to save
*										results
*									INT8 *time_holder : Array which contains time information in bytes
*									INT8 *date_holder : Array which contains date information in bytes
*
* Parameters returned		:		INT32 : -1 if error 
*											+1 if no error
*
* Functions called			:		None
*									
* Global variables used		:		None 
*
* Global variables modified	:		None 
*
*/

INT32 nmeaTimeExtract (msg_ZDA_str *msg_struct_ptr, INT8 *time_holder, INT8 *date_holder)
{
	UINT16	time_valid 	= 0;
	UINT16	date_valid	= 0;	

	/* Is there any better way? Help!! */
	msg_struct_ptr->hour	=	(time_holder[0] * 10) + time_holder[1];
	msg_struct_ptr->min		=	(time_holder[2] * 10) + time_holder[3];
	msg_struct_ptr->sec		=	(time_holder[4] * 10) + time_holder[5];
	
	msg_struct_ptr->day		=	(date_holder[0] * 10) + date_holder[1];
	msg_struct_ptr->month	=	(date_holder[2] * 10) + date_holder[3];
	msg_struct_ptr->year	=	(date_holder[4] * 1000) + (date_holder[5] * 100)\
								  + (date_holder[6] * 10) + (date_holder[7] * 1);
								  
	
	/*	Testing extracted time date */
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
	
	/* February?? and otherthings?? Add if needed */
	if ((msg_struct_ptr->day >= 0) && (msg_struct_ptr->day <= 31))			
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

/*
* Function Information
*
* Function Name				:		asciiToDecimal 
*
* Function Description		:		This will accept an characters numbers and it will convert
*									it into integers. Skipped if any 'alphabets' or other characters found
*
* Parameters passed			:		INT8 *NMEA_string: Pointer to array from which we need to convert the data
*									INT16 number_of_bytes: How many bytes we need to convert?.
*									INT8 *decimal_array: In this array we have to store the results
*
* Parameters returned		:		None
*
* Functions called			:		None
*									
* Global variables used		:		None 
*
* Global variables modified	:		None 
*
*/
void asciiToDecimal (INT8 *NMEA_string, INT16 number_of_bytes, INT8 *decimal_array)
{
	printf ("\n");
	while (number_of_bytes)
	{
		/* 0 to 9*/
		if (*NMEA_string >= '0' && *NMEA_string <= '9')
		{
			*decimal_array = *NMEA_string - '0';
		}
		
		/*
		else if (*NMEA_string >= 'a' && *NMEA_string <= 'f')
		{
			*decimal_array = *NMEA_string - 'a' + 10;
			printf ("I should never come here for time, date :)");
		}*/
				
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

