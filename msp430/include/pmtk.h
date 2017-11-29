#ifndef GPS_H
#define GPS_H

#include <stdint.h>
#include "ringbuffer.h"


/*******************************************************************************
  RMC - Recommended Minimum Navigation Information
  
  This is one of the sentences commonly emitted by GPS units.
                                                           12
         1         2 3       4 5        6  7   8   9    10 11| 13
         |         | |       | |        |  |   |   |    |  | |  |
  $GPRMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,xxxx,x.x,a,m,*hh<CR><LF>
  
  Field Number:
  1. UTC Time
  2. Status, V=Navigation receiver warning A=Valid
  3. Latitude
  4. N or S
  5. Longitude
  6. E or W
  7. Speed over ground, knots
  8. Track made good, degrees true
  9. Date, ddmmyy
  10. Magnetic Variation, degrees
  11. E or W
  12. FAA mode indicator (NMEA 2.3 and later)
  13. Checksum

  A status of V means the GPS has a valid fix that is below an internal
  quality threshold, e.g. because the dilution of precision is too high or an
  elevation mask test failed.
 *******************************************************************************/


/*******************************************************************************
  GGA - Global Positioning System Fix Data

  This is one of the sentences commonly emitted by GPS units.  Time, Position
  and fix related data for a GPS receiver.

                                                       11
         1         2       3 4        5 6 7  8   9  10 |  12 13  14   15
         |         |       | |        | | |  |   |   | |   | |   |    |
  $GPGGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh<CR><LF>
  
  Field Number:
  1. Universal Time Coordinated (UTC)
  2. Latitude
  3. N or S (North or South)
  4. Longitude
  5. E or W (East or West)
  6. GPS Quality Indicator,
    0 - fix not available,
    1 - GPS fix,
    2 - Differential GPS fix (values above 2 are 2.3 features)
    3 = PPS fix
    4 = Real Time Kinematic
    5 = Float RTK
    6 = estimated (dead reckoning)
    7 = Manual input mode
    8 = Simulation mode
  7. Number of satellites in view, 00 - 12
  8. Horizontal Dilution of precision (meters)
  9. Antenna Altitude above/below mean-sea-level (geoid) (in meters)
  10. Units of antenna altitude, meters
  11. Geoidal separation, the difference between the WGS-84 earth ellipsoid and
      mean-sea-level (geoid), "-" means mean-sea-level below ellipsoid
  12. Units of geoidal separation, meters
  13. Age of differential GPS data, time in seconds since last SC104 type 1 or 9
      update, null field when DGPS is not used
  14. Differential reference station ID, 0000-1023
  15. Checksum
 *******************************************************************************/


#define PMTK_ACK                       "001"
#define PMTK_SYS_MSG                   "010"
#define PMTK_TXT_MSG                   "011"
#define PMTK_CMD_HOT_START             "101"
#define PMTK_CMD_WARM_START            "102"
#define PMTK_CMD_COLD_START            "103"
#define PMTK_CMD_FULL_COLD_START       "104"
#define PMTK_SET_NMEA_UPDATERATE       "220"
#define PMTK_SET_NMEA_BAUDRATE         "251"
#define PMTK_API_SET_DGPS_MODE         "301"
#define PMTK_API_Q_DGPS_MODE           "401"
#define PMTK_API_DT_GPS_MODE           "501"
#define PMTK_API_SET_SBAS_ENABLED      "313"
#define PMTK_API_Q_SBAS_ENABLED        "413"
#define PMTK_DT_SBAS_ENABLED           "513"
#define PMTK_API_SET_NMEA_OUTPUT       "314"
#define PMTK_API_Q_NMEA_OUTPUT         "414"
#define PMTK_API_DT_NMEA_OUTPUT        "514"
#define PMTK_API_SET_SBAS_MODE         "319"
#define PMTK_API_Q_SBAS_MODE           "419"
#define PMTK_API_DT_SBAS_MODE          "519"
#define PMTK_Q_RELEASE                 "605"
#define PMTK_DT_RELEASE                "705"
#define PMTK_Q_EPO_INFO                "607"
#define PMTK_DT_EPO_INFO               "707"
#define PMTK_CMD_CLEAR_EPO             "127"
#define PMTK_SET_NAV_SPEED_THRESHOLD1  "397"
#define PMTK_SET_NAV_SPEED_THRESHOLD2  "386"
#define PMTK_Q_NAV_THRESHOLD           "447"
#define PMTK_DT_NAV_THRESHOLD          "527"
#define PMTK_CMD_STANDBY_MODE          "161"
#define PMTK_SET_AL_DEE_CFG            "223"
#define PMTK_CMD_PERIODIC_MODE         "225"
#define PMTK_CMD_AIC_MODE              "286"
#define PMTK_CMD_EASY_ENABLE           "869"
#define PMTK_LOCUS_CONFIG              "187"
#define PMTK_API_SET_DATUM             "330"
#define PMTK_API_Q_DATUM               "430"
#define PMTK_API_DT_DATUM              "530"
#define PMTK_API_SET_SUPPORT_QZSS_NMEA "351"
#define PMTK_API_SET_STOP_QZSS         "352"

/* Settings for packet type PMTK_API_SET_NMEA_OUTPUT */
#define NMEA_SEN_GLL  '0'       /* lat-long */
#define NMEA_SEN_RMC  '1'       /* recommended minimum */
#define NMEA_SEN_VTG  '2'       /* ground course and speed */
#define NMEA_SEN_GGA  '3'       /* GPS fix data, including altitude */
#define NMEA_SEN_GSA  '4'       /* GNSS DOPS and active satellites */
#define NMEA_SEN_GSV  '5'       /* GNSS satellites in view */

/* Settings for packet type PMTK_CMD_PERIODIC_MODE */
#define BACK_TO_NORMAL_MODE        '0'
#define PERIODIC_BACKUP_MODE       '1'
#define PERIOD_STANDBY_MODE        '2'
#define PERPETUAL_BACKUP_MODE      '4'
#define ALWAYS_LOCATE_STANDBY_MODE '8'
#define ALWAYS_LOCATE_BACKUP_MODE  '9'


/* Compute checksum for sentence */
uint8_t pmtk_checksum(ringbuffer *rb);

/* Confirm that checksum is correct */
uint8_t pmtk_checksum_confirm(ringbuffer *rb);

/* Put a type header for a PMTK string in the buffer */
bool pmtk_make_string_header(ringbuffer *rb, char* type);

/* Once the body is in the buffer, make the PMTK footer */
bool pmtk_make_string_footer(ringbuffer *rb);


/*******************************************************************************
 The following #defines are from the Adafruit GPS library, and this text is
 included to comply with their license.

 This is the Adafruit GPS library - the ultimate GPS library
 for the ultimate GPS module!

 Tested and works great with the Adafruit Ultimate GPS module
 using MTK33x9 chipset
     ------> http://www.adafruit.com/products/746
 Pick one up today at the Adafruit electronics shop 
 and help support open source hardware & software! -ada

 Adafruit invests time and resources providing this open source code, 
 please support Adafruit and open-source hardware by purchasing 
 products from Adafruit!

 Written by Limor Fried/Ladyada  for Adafruit Industries.  
 BSD license, check license.txt for more information
 All text above must be included in any redistribution
 *******************************************************************************/

/*
 * Different commands to set the update rate from once a second (1 Hz) to 10
 * times a second (10Hz) Note that these only control the rate at which the
 * position is echoed.  To actually speed up the position fix you must also send
 * one of the position fix rate commands below too.
 */
#define PMTK_SET_NMEA_UPDATE_100_MILLIHERTZ  "$PMTK220,10000*2F"
#define PMTK_SET_NMEA_UPDATE_200_MILLIHERTZ  "$PMTK220,5000*1B"
#define PMTK_SET_NMEA_UPDATE_1HZ  "$PMTK220,1000*1F"
#define PMTK_SET_NMEA_UPDATE_2HZ  "$PMTK220,500*2B"
#define PMTK_SET_NMEA_UPDATE_5HZ  "$PMTK220,200*2C"
#define PMTK_SET_NMEA_UPDATE_10HZ "$PMTK220,100*2F"

/* Position fix update rate commands. */
#define PMTK_API_SET_FIX_CTL_100_MILLIHERTZ  "$PMTK300,10000,0,0,0,0*2C"
#define PMTK_API_SET_FIX_CTL_200_MILLIHERTZ  "$PMTK300,5000,0,0,0,0*18"
#define PMTK_API_SET_FIX_CTL_1HZ  "$PMTK300,1000,0,0,0,0*1C"
#define PMTK_API_SET_FIX_CTL_5HZ  "$PMTK300,200,0,0,0,0*2F"
/* Can't fix position faster than 5 times a second! */

#define PMTK_SET_BAUD_9600  "$PMTK251,9600*17"
#define PMTK_SET_BAUD_57600 "$PMTK251,57600*2C"

/* Turn on only the second sentence (GPRMC) */
#define PMTK_SET_NMEA_OUTPUT_RMCONLY "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"
/* Turn on GPRMC and GGA */
#define PMTK_SET_NMEA_OUTPUT_RMCGGA  "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
/* Turn on ALL THE DATA */
#define PMTK_SET_NMEA_OUTPUT_ALLDATA "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
/* Turn off output */
#define PMTK_SET_NMEA_OUTPUT_OFF     "$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"

/*
 * To generate your own sentences, check out the MTK command datasheet and use a
 * checksum calculator such as http://www.hhhh.org/wiml/proj/nmeaxor.html
 */
#define PMTK_LOCUS_STARTLOG     "$PMTK185,0*22"
#define PMTK_LOCUS_STOPLOG      "$PMTK185,1*23"
#define PMTK_LOCUS_STARTSTOPACK "$PMTK001,185,3*3C"
#define PMTK_LOCUS_QUERY_STATUS "$PMTK183*38"
#define PMTK_LOCUS_ERASE_FLASH  "$PMTK184,1*22"
#define LOCUS_OVERLAP  0
#define LOCUS_FULLSTOP 1

#define PMTK_ENABLE_SBAS "$PMTK313,1*2E"
#define PMTK_ENABLE_WAAS "$PMTK301,2*2E"

/* Standby command & boot successful message */
#define PMTK_STANDBY         "$PMTK161,0*28"
#define PMTK_STANDBY_SUCCESS "$PMTK001,161,3*36"  /* not needed currently */
#define PMTK_AWAKE           "$PMTK010,002*2D"

/* Ask for the release and version */
//#define PMTK_Q_RELEASE "$PMTK605*31"

/* Request for updates on antenna status */
#define PGCMD_ANTENNA   "$PGCMD,33,1*6C" 
#define PGCMD_NOANTENNA "$PGCMD,33,0*6D" 

/* End of Adafruit GPS #defines */


#endif
