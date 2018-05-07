/**
 * @file diagslave.cpp
 *
 * @if NOTICE
 *
 * Copyright (c) proconX Pty Ltd. All rights reserved.
 *
 * The following source file constitutes example program code and is
 * intended merely to illustrate useful programming techniques.  The user
 * is responsible for applying the code correctly.
 *
 * THIS SOFTWARE IS PROVIDED BY PROCONX AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL PROCONX OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @endif
 */


// Platform header
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef _WIN32
#  include "getopt.h"
#else
#  include <unistd.h>
#endif

// Include FieldTalk package header
#include "MbusRtuSlaveProtocol.hpp"
#include "MbusAsciiSlaveProtocol.hpp"
#include "MbusTcpSlaveProtocol.hpp"
#include "DiagnosticDataTable.hpp"


/*****************************************************************************
 * String constants
 *****************************************************************************/

const char versionStr[]= "2.12";
const char progName[] = "diagslave";
const char bannerStr[] =
"%s %s - FieldTalk(tm) Modbus(R) Diagnostic Slave Simulator\n"
"Copyright (c) 2002-2012 proconX Pty Ltd\n"
"Visit http://www.modbusdriver.com for Modbus libraries and tools.\n"
"\n";

const char usageStr[] =
"%s [OPTIONS] [SERIALPORT]\n"
"Arguments: \n"
"SERIALPORT    Serial port when using Modbus ASCII or Modbus RTU protocol \n"
"              COM1, COM2 ...                on Windows \n"
"              /dev/ttyS0, /dev/ttyS1 ...    on Linux \n"
"              /dev/ser1, /dev/ser2 ...      on QNX \n"
"General options:\n"
"-m ascii      Modbus ASCII protocol\n"
"-m rtu        Modbus RTU protocol (default)\n"
"-m tcp        MODBUS/TCP protocol\n"
"-o #          Master activity time-out in seconds (1.0 - 100, 3 s is default)\n"
"-c #          Connection time-out in seconds (1.0 - 3600, 60 s is default)\n"
"-a #          Slave address (1-255 for RTU/ASCII, 0-255 for TCP)\n"
"Options for MODBUS/TCP:\n"
"-p #          TCP port number (502 is default)\n"
"Options for Modbus ASCII and Modbus RTU:\n"
"-b #          Baudrate (e.g. 9600, 19200, ...) (19200 is default)\n"
"-d #          Databits (7 or 8 for ASCII protocol, 8 for RTU)\n"
"-s #          Stopbits (1 or 2, 1 is default)\n"
"-p none       No parity\n"
"-p even       Even parity (default)\n"
"-p odd        Odd parity\n"
"-4 #          RS-Master mode, RTS on while transmitting and another # ms after\n"
"";


/*****************************************************************************
 * Enums
 *****************************************************************************/

enum
{
   RTU,   ///< Modbus RTU protocol
   ASCII, ///< Modbus ASCII protocol
   TCP    ///< MODBUS/TCP protocol
};


/*****************************************************************************
 * Gobal configuration data
 *****************************************************************************/

int address = -1;
long timeOut = 3000;
long connectionTo = 60000;
long baudRate = 19200;
int dataBits = 8;
int stopBits = 1;
int parity = MbusSerialSlaveProtocol::SER_PARITY_EVEN;
int protocol = -1;
char *portName = NULL;
int port = 502;
int rs485Mode = 0;


/*****************************************************************************
 * Protocol and data table
 *****************************************************************************/

DiagnosticMbusDataTable *dataTablePtrArr[256];
MbusSlaveServer *mbusServerPtr = NULL;


/*****************************************************************************
 * Function implementation
 *****************************************************************************/

/**
 * Prints a usage message on stdout and exits
 */
void printUsage()
{
   printf(bannerStr, progName, versionStr);
   printf("Usage: ");
   printf(usageStr, progName);
   exit(EXIT_SUCCESS);
}


/**
 * Prints version info on stdout
 */
void printVersion()
{
   printf(bannerStr, progName, versionStr);
}


/**
 * Prints the current configuration on stdout
 */
void printConfig()
{
   printf(bannerStr, progName, versionStr);
   printf("Protocol configuration: ");
   switch (protocol)
   {
      case RTU:
         printf("Modbus RTU\n");
      break;
      case ASCII:
         printf("Modbus ASCII\n");
      break;
      case TCP:
         printf("MODBUS/TCP\n");
      break;
      default:
         printf("unknown\n");
      break;
   }
   printf("Slave configuration: ");
   printf("address = %d, ", address);
   printf("master activity t/o = %.2f\n", ((float) timeOut) / 1000.0F);
   if (protocol == TCP)
   {
      printf("TCP configuration: ");
      printf("port = %d, ", port);
      printf("connection t/o = %.2f\n", ((float) connectionTo) / 1000.0F);
   }
   else
   {
      printf("Serial port configuration: ");
      printf("%s, ", portName);
      printf("%ld, ", baudRate);
      printf("%d, ", dataBits);
      printf("%d, ", stopBits);
      switch (parity)
      {
         case MbusSerialSlaveProtocol::SER_PARITY_NONE:
            printf("none\n");
         break;
         case MbusSerialSlaveProtocol::SER_PARITY_EVEN:
            printf("even\n");
         break;
         case MbusSerialSlaveProtocol::SER_PARITY_ODD:
            printf("odd\n");
         break;
         default:
            printf("unknown\n");
         break;
      }
   }
   printf("\n");
}


/**
 * Prints bad option error message and exits program
 *
 * @param text Option error message
 */
void exitBadOption(const char *const text)
{
   fprintf(stderr, "%s: %s! Try -h for help.\n", progName, text);
   exit(EXIT_FAILURE);
}


/**
 * Scans and parses the command line options.
 *
 * @param argc Argument count
 * @param argv Argument value string array
 */
void scanOptions(int argc, char **argv)
{
   int c;

   // Check for --version option
   for (c = 1; c < argc; c++)
   {
      if (strcmp (argv[c], "--version") == 0)
      {
         printVersion();
         exit(EXIT_SUCCESS);
      }
   }

   // Check for --help option
   for (c = 1; c < argc; c++)
   {
      if (strcmp (argv[c], "--help") == 0)
         printUsage();
   }

   opterr = 0; // Disable getopt's error messages
   for(;;)
   {
      c = getopt(argc, argv, "h4:a:b:d:s:p:m:o:c:");
      if (c == -1)
         break;

      switch (c)
      {
         case 'm':
            if (strcmp(optarg, "tcp") == 0)
            {
               protocol = TCP;
            }
            else
               if (strcmp(optarg, "rtu") == 0)
               {
                  protocol = RTU;
               }
               else
                  if (strcmp(optarg, "ascii") == 0)
                  {
                     protocol = ASCII;
                  }
                  else
                  {
                     exitBadOption("Invalid protocol parameter");
                  }
         break;
         case 'a':
            address = strtol(optarg, NULL, 0);
            if ((address < -1) || (address > 255))
               exitBadOption("Invalid address parameter");
         break;
         case '4':
            rs485Mode = (int) strtol(optarg, NULL, 0);
            if ((rs485Mode <= 0) || (rs485Mode > 1000))
               exitBadOption("Invalid RTS delay parameter");
         break;
         case 'o':
            timeOut = (int) (strtod(optarg, NULL) * 1000.0);
            if ((timeOut < 1000) || (timeOut > 100000))
               exitBadOption("Invalid time-out parameter");
         break;
         case 'c':
            connectionTo = (int) (strtod(optarg, NULL) * 1000.0);
            if ((connectionTo < 10000) || (connectionTo > 3600000))
               exitBadOption("Invalid connection time-out parameter");
         break;
         case 'b':
            baudRate = strtol(optarg, NULL, 0);
            if (baudRate == 0)
               exitBadOption("Invalid baudrate parameter");
         break;
         case 'd':
            dataBits = (int) strtol(optarg, NULL, 0);
            if ((dataBits != 7) && (dataBits != 8))
               exitBadOption("Invalid databits parameter");
         break;
         case 's':
            stopBits = (int) strtol(optarg, NULL, 0);
            if ((stopBits != 1) && (stopBits != 2))
               exitBadOption("Invalid stopbits parameter");
         break;
         case 'p':
            if (strcmp(optarg, "none") == 0)
            {
               parity = MbusSerialSlaveProtocol::SER_PARITY_NONE;
            }
            else
               if (strcmp(optarg, "odd") == 0)
               {
                  parity = MbusSerialSlaveProtocol::SER_PARITY_ODD;
               }
               else
                  if (strcmp(optarg, "even") == 0)
                  {
                     parity = MbusSerialSlaveProtocol::SER_PARITY_EVEN;
                  }
                  else
                  {
                     port = strtol(optarg, NULL, 0);
                     if ((port <= 0) || (port > 0xFFFF))
                        exitBadOption("Invalid parity or port parameter");
                  }
         break;
         case 'h':
            printUsage();
         break;
         default:
            exitBadOption("Unrecognized option or missing option parameter");
         break;
      }
   }

   //
   // No protocol parameter specified, auto-detect
   //
   if (protocol == -1)
   {
      if ((argc - optind) == 0)
         protocol = TCP;
      else
         protocol = RTU;
   }

   if (protocol == TCP)
   {
      if ((argc - optind) != 0)
         exitBadOption("Invalid number of parameters");
   }
   else
   {
      if ((argc - optind) != 1)
         exitBadOption("Invalid number of parameters");
      else
         portName = argv[optind];
   }
}




/**
 * Callback function which cecks a master's IP address and
 * either accepts or rejects a master's connection.
 *
 * @param masterIpAddrSz IPv4 Internet host address string
 * in the standard numbers-and-dots notation.
 *
 * @return Returns 1 to accept a connection or 0 to reject it.
 */
int validateMasterIpAddr(const char* masterIpAddrSz)
{
   printf("\nvalidateMasterIpAddr: accepting connection from %s\n",
          masterIpAddrSz);
   return 1;
}


/**
 * Starts up server
 */
void startupServer()
{
   int i;
   int result = -1;

   switch (protocol)
   {
      case RTU:
         mbusServerPtr = new MbusRtuSlaveProtocol();
         if (address == -1)
         {
            for (i = 1; i < 255; i++)
            mbusServerPtr->addDataTable(i, dataTablePtrArr[i]);
         }
         else
            mbusServerPtr->addDataTable(address, dataTablePtrArr[address]);
         mbusServerPtr->setTimeout(timeOut);
         if (rs485Mode > 0)
            ((MbusRtuSlaveProtocol *) mbusServerPtr)->enableRs485Mode(rs485Mode);
         result = ((MbusRtuSlaveProtocol *) mbusServerPtr)->startupServer(
                    portName, baudRate, dataBits, stopBits, parity);
      break;
      case ASCII:
         mbusServerPtr = new MbusAsciiSlaveProtocol();
         if (address == -1)
         {
            for (i = 1; i < 255; i++)
            mbusServerPtr->addDataTable(i, dataTablePtrArr[i]);
         }
         else
            mbusServerPtr->addDataTable(address, dataTablePtrArr[address]);
         mbusServerPtr->setTimeout(timeOut);
         if (rs485Mode > 0)
            ((MbusAsciiSlaveProtocol *) mbusServerPtr)->enableRs485Mode(rs485Mode);
         result = ((MbusAsciiSlaveProtocol *) mbusServerPtr)->startupServer(
                   portName, baudRate, dataBits, stopBits, parity);
      break;
      case TCP:
         mbusServerPtr = new MbusTcpSlaveProtocol();
         if (address == -1)
         {
            for (i = 0; i < 255; i++) // Note: TCP support slave addres of 0
               mbusServerPtr->addDataTable(i, dataTablePtrArr[i]);
         }
         else
            mbusServerPtr->addDataTable(address, dataTablePtrArr[address]);
         mbusServerPtr->setTimeout(timeOut);
         ((MbusTcpSlaveProtocol *) mbusServerPtr)->installIpAddrValidationCallBack(validateMasterIpAddr);
         ((MbusTcpSlaveProtocol *) mbusServerPtr)->setPort((unsigned short) port);
         ((MbusTcpSlaveProtocol *) mbusServerPtr)->setConnectionTimeOut(connectionTo);
         result = ((MbusTcpSlaveProtocol *) mbusServerPtr)->startupServer();
      break;
   }
   switch (result)
   {
      case FTALK_SUCCESS:
         printf("Server started up successfully.\n");
      break;
      case FTALK_ILLEGAL_ARGUMENT_ERROR:
         fprintf(stderr, "Configuration setting not supported!\n");
         exit(EXIT_FAILURE);
      break;
      default:
         fprintf(stderr, "%s!\n", getBusProtocolErrorText(result));
         exit(EXIT_FAILURE);
      break;
   }
}


/**
 * Shutdown server
 */
void shutdownServer()
{
   printf("Shutting down server.\n");
   delete mbusServerPtr;
}


/**
 * Run server
 */
void runServer()
{
   int result = FTALK_SUCCESS;

   printf("Listening to network (Ctrl-C to stop)\n");
   while (result == FTALK_SUCCESS)
   {
      result = mbusServerPtr->serverLoop();
      if (result != FTALK_SUCCESS)
         fprintf(stderr, "%s!\n", getBusProtocolErrorText(result));\
      else
      {
         printf(".");
         fflush(stdout);
      }
   }
}


/**
 * Main function
 *
 * @param argc Command line argument count
 * @param argv Command line argument value string array
 * @return Error code: 0 = OK, else error
 */
int main(int argc, char **argv)
{
   int i;

   // Construct data tables
   for (i = 0; i < 255; i++)
   {
      dataTablePtrArr[i] = new DiagnosticMbusDataTable(i);
   }

   scanOptions(argc, argv);
   printConfig();
   atexit(shutdownServer);
   startupServer();
   runServer();
   return EXIT_FAILURE;
}
