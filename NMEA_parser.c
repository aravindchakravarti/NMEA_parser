#include "stdio.h"
#include "NMEA_parser.h"

#define DEBUG_PRINT

enum NMEA_message_enum NMEA_message;

void 				NMEAInit 			(void);
NMEA_data_stuct* 	nmeaHeaderFinder 	(INT8 *);


UINT16	number_of_msgs = sizeof(NMEA_info)/sizeof(NMEA_data_stuct);

void hexToDecimal (INT8 *, INT16 , INT8 *);

main ()
{
	FILE 	*fp;
	INT8 	ch		= 0;
	UINT8 	count	= 0;
	INT8	NMEA_string[80];
	
	
	#ifdef DEBUG_PRINT
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

	NMEAInit();
	#endif
	
	NMEA_data_stuct_ptr = nmeaHeaderFinder(&NMEA_string[0]);
	NMEA_data_stuct_ptr->nmeaMsgDataExtract(&NMEA_string[0]);
}

void NMEAInit (void)
{
	
}

NMEA_data_stuct* nmeaHeaderFinder (INT8 *NMEA_string)
{
	INT8 	header[7];
	INT8	index	= 0;
	INT32 	return_value = 0;
	
	NMEA_data_stuct *NMEA_info_ptr;
	
	NMEA_info_ptr = &NMEA_info[0];
	
	for (index = 0; index <6; index++)
	{
		header[index] = *NMEA_string++;
	}
	
	header[index] = '\0';
	
	#ifdef DEBUG_PRINT
	printf ("\nMy Current number of messages are : %d\n", number_of_msgs);
	#endif
	
	for (index = 0; index < number_of_msgs; index++)
	{
		//LocalStringCompare (&header[0], NMEA_string);
		return_value = strcmp (header, NMEA_info_ptr->msg);
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
	
	#ifdef DEBUG_PRINT
	printf ("****\n");
	printf ("%s\n", header);
	printf ("%s\n", NMEA_info_ptr->msg);
	#endif
	
	return (NMEA_info_ptr);
	
}

void gpzdaMsgExtract (INT8 *NMEA_string)
{
	INT8 decimal_array[20] = {0};
	INT16 index = 0;
	
	printf ("So, Here we come to extract GPZDA");
	
	/* Not only this is incrementing the pointer but at the end of finding 
	comma, it is also skipping it!! */
	while (*NMEA_string++ != ',');
		
	hexToDecimal(NMEA_string, 9, decimal_array);
	
	NMEA_data_stuct_ptr->hour 	= (decimal_array[0] * 10) + decimal_array[1];
	NMEA_data_stuct_ptr->min 	= (decimal_array[2] * 10) + decimal_array[3];
	NMEA_data_stuct_ptr->sec 	= (decimal_array[4] * 10) + decimal_array[5];
	
	printf ("Hour = %d\n", NMEA_data_stuct_ptr->hour);
	printf ("Min  = %d\n", NMEA_data_stuct_ptr->min);
	printf ("Sec  = %d\n", NMEA_data_stuct_ptr->sec);
}

void gpggaMsgExtract (INT8 *NMEA_string)
{
	printf ("So, Here we come to extract GPGGA");
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
		
		else
		{
			*decimal_array = *NMEA_string - '0';
		}
		
		number_of_bytes--;
		decimal_array++;
		NMEA_string++;
	}
}

/* int LocalStringCompare (INT8 *array_1, INT8 *array_2)
{
	for ( ; *array_1 == *array_2; array_1++, array_2++)
	{
		if (*array_1 == '\0');
		{
			return (0);
		}
	}
} */
