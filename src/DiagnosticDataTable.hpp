/**
 * @file DiagnosticDataTable.hpp
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


#ifndef _DIAGNOSTICDATATABLE_H_INCLUDED
#define _DIAGNOSTICDATATABLE_H_INCLUDED


// Platform header
#include <stdio.h>
#include <string.h>
#if defined(_MSC_VER) || defined(__GNUC__)
#  define HAS_STRNCPY
#endif

// Package header
#include "MbusDataTableInterface.hpp"


/*****************************************************************************
 * Device identification objects for Modbus function 43/14
 *****************************************************************************/

char *VENDOR_NAME = "proconX Pty Ltd";
char *PRODUCT_CODE = "FT-MBSV";
char *VENDOR_URL = "http://www.modbusdriver.com";
char *PRODUCT_NAME = "FieldTalk";
char *MODEL_NAME = "Modbus Slave C++ Library";
char *USER_APPLICATION_NAME = "diagslave";
char CUSTOM_OBJECT[100] = "Custom data 123";


/*****************************************************************************
 * DiagnosticMbusDataTable class declaration
 *****************************************************************************/

/**
 * @brief This base class implements a Modbus server (or slave).
 *
 * These methods apply to all protocol flavours via inheritance. For a more
 * detailed description see section @ref mbusslave.  It provides functions
 * to start-up, run and shutdown a Modbus server.  The server processes
 * data and control functions received from a Modbus master. This
 * implementation implements all Bit Access and 16 Bits Access
 * Function Codes.  In addition some
 * frequently used Diagnostics Funtion Codes are also
 * implemented.
 *
 * @ingroup mbusslave
 * @see MbusSlaveServer
 * @see mbusslave
 */
class DiagnosticMbusDataTable: public MbusDataTableInterface
{

public:

   DiagnosticMbusDataTable(int slaveAddr)
   {
      this->slaveAddr = slaveAddr;
      memset(regData, 0, sizeof(regData));
      memset(bitData, 0, sizeof(bitData));
   }


   ~DiagnosticMbusDataTable()
   {
   }


   char readExceptionStatus()
   {
      printf("\rSlave %3d: readExceptionStatus\n", slaveAddr);
      return 0x55;
   }


   int readInputDiscretesTable(int startRef,
                               char bitArr[],
                               int refCnt)
   {
      printf("\rSlave %3d: readInputDiscretes from %d, %d references\n",
             slaveAddr, startRef, refCnt);

      // Adjust Modbus reference counting
      startRef--;

      //
      // Validate range
      //
      if (startRef + refCnt > int(sizeof(bitData) / sizeof(char)))
         return 0;

      //
      // Copy data
      //
      memcpy(bitArr, &bitData[startRef], refCnt * sizeof(char));
      return 1;
   }


   int readCoilsTable(int startRef,
                      char bitArr[],
                      int refCnt)
   {
      printf("\rSlave %3d: readCoils from %d, %d references\n",
             slaveAddr, startRef, refCnt);

      // Adjust Modbus reference counting
      startRef--;

      //
      // Validate range
      //
      if (startRef + refCnt > int(sizeof(bitData) / sizeof(char)))
         return 0;

      //
      // Copy data
      //
      memcpy(bitArr, &bitData[startRef], refCnt * sizeof(char));
      return 1;
   }


   int writeCoilsTable(int startRef,
                       const char bitArr[],
                       int refCnt)
   {
      printf("\rSlave %3d: writeCoils from %d, %d references\n",
             slaveAddr, startRef, refCnt);

      // Adjust Modbus reference counting
      startRef--;

      //
      // Validate range
      //
      if (startRef + refCnt > int(sizeof(bitData) / sizeof(char)))
         return 0;

      //
      // Copy data
      //
      memcpy(&bitData[startRef], bitArr, refCnt * sizeof(char));
      return 1;
   }


   int readInputRegistersTable(int startRef,
                               short regArr[],
                               int refCnt)
   {
      printf("\rSlave %3d: readInputRegisters from %d, %d references\n",
             slaveAddr, startRef, refCnt);

      // Adjust Modbus reference counting
      startRef--;

      //
      // Validate range
      //
      if (startRef + refCnt > int(sizeof(regData) / sizeof(short)))
         return 0;

      //
      // Copy data
      //
      memcpy(regArr, &regData[startRef], refCnt * sizeof(short));
      return 1;
   }


   int readHoldingRegistersTable(int startRef,
                                 short regArr[],
                                 int refCnt)
   {
      printf("\rSlave %3d: readHoldingRegisters from %d, %d references\n",
             slaveAddr, startRef, refCnt);

      // Adjust Modbus reference counting
      startRef--;

      //
      // Validate range
      //
      if (startRef + refCnt > int(sizeof(regData) / sizeof(short)))
         return 0;

      //
      // Copy data
      //
      memcpy(regArr, &regData[startRef], refCnt * sizeof(short));
      return 1;
   }


   int writeHoldingRegistersTable(int startRef,
                                  const short regArr[],
                                  int refCnt)
   {
      printf("\rSlave %3d: writeHoldingRegisters from %d, %d references\n",
             slaveAddr, startRef, refCnt);

      // Adjust Modbus reference counting
      startRef--;

      //
      // Validate range
      //
      if (startRef + refCnt > int(sizeof(regData) / sizeof(short)))
         return 0;

      //
      // Copy data
      //
      memcpy(&regData[startRef], regArr, refCnt * sizeof(short));
      return 1;
   }


   int readFileRecord(int refType, int fileNo, int startRef,
                      short regArr[], int refCnt)
   {
      printf("\rSlave %3d: readFileRecord type %d, file %d from %d, %d references\n",
             slaveAddr, refType, fileNo, startRef, refCnt);

      //
      // Only reference type 6 is supported in this example. Please note
      // that the MODBUS Application Protocol Specification V1.1b actually
      // requires this field to be set to 6.
      //
      if (refType != 6)
         return 0;

      //
      // Only file type 3 (input registers) and file type 4 (holding registers)
      // are supported in this example.
      //
      if ((fileNo != 3) && (fileNo != 4))
         return 0;

      //
      // Validate range
      //
      if (startRef + refCnt > int(sizeof(regData) / sizeof(short)))
         return 0;

      //
      // Copy data
      //
      memcpy(regArr, &regData[startRef], refCnt * sizeof(short));
      return 1;
   }


   int writeFileRecord(int refType, int fileNo, int startRef,
                       short regArr[], int refCnt)
   {
      printf("\rSlave %3d: writeFileRecord type %d, file %d from %d, %d references\n",
             slaveAddr, refType, fileNo, startRef, refCnt);

      //
      // Only reference type 6 is supported in this example. Please note
      // that the MODBUS Application Protocol Specification V1.1b actually
      // requires this field to be set to 6.
      //
      if (refType != 6)
         return 0;

      //
      // Only file type 3 (input registers) and file type 4 (holding registers)
      // are supported in this example.
      //
      if ((fileNo != 3) && (fileNo != 4))
         return 0;

      //
      // Validate range
      //
      if (startRef + refCnt > int(sizeof(regData) / sizeof(short)))
         return 0;

      //
      // Copy data
      //
      memcpy(&regData[startRef], regArr, refCnt * sizeof(short));
      return 1;
   }


   int getSlaveId(char bufferArr[], int maxBufSize)
   {
#ifdef HAS_STRNCPY
      strncpy(bufferArr, PRODUCT_NAME, maxBufSize);
#else
      strcpy(bufferArr, PRODUCT_NAME);
#endif
      return strlen(PRODUCT_NAME);
   }


   int getRunIndicatorStatus()
   {
      printf("\rSlave %3d: reportSlaveId\n", slaveAddr);
      return 1; // 1 = running
   }


   int getDeviceIdObject(int objId, char bufferArr[], int maxBufSize)
   {
      switch (objId)
      {
         //
         // VendorName
         //
         case 0:
            if (bufferArr)
            {
               printf("\rSlave %3d: getDeviceIdObject VendorName\n", slaveAddr);
#ifdef HAS_STRNCPY
               strncpy(bufferArr, VENDOR_NAME, maxBufSize);
#else
               strcpy(bufferArr, VENDOR_NAME);
#endif
            }
         return strlen(VENDOR_NAME);

         //
         // ProductCode
         //
         case 1:
            if (bufferArr)
            {
               printf("\rSlave %3d: getDeviceIdObject ProductCode\n", slaveAddr);
#ifdef HAS_STRNCPY
               strncpy(bufferArr, PRODUCT_CODE, maxBufSize);
#else
               strcpy(bufferArr, PRODUCT_CODE);
#endif
            }
         return strlen(PRODUCT_CODE);

         //
         // MajorMinorRevision
         //
         case 2:
            if (bufferArr)
            {
               printf("\rSlave %3d: getDeviceIdObject MajorMinorRevision\n", slaveAddr);
#ifdef HAS_STRNCPY
               strncpy(bufferArr, MbusSlaveServer::getPackageVersion(), maxBufSize);
#else
               strcpy(bufferArr, MbusSlaveServer::getPackageVersion());
#endif
            }
         return strlen(MbusSlaveServer::getPackageVersion());

         //
         // VendorUrl
         //
         case 3:
            if (bufferArr)
            {
               printf("\rSlave %3d: getDeviceIdObject VendorUrl\n", slaveAddr);
#ifdef HAS_STRNCPY
               strncpy(bufferArr, VENDOR_URL, maxBufSize);
#else
               strcpy(bufferArr, VENDOR_URL);
#endif
            }
         return strlen(VENDOR_URL);

         //
         // ProductName
         //
         case 4:
            if (bufferArr)
            {
               printf("\rSlave %3d: getDeviceIdObject ProductName\n", slaveAddr);
#ifdef HAS_STRNCPY
               strncpy(bufferArr, PRODUCT_NAME, maxBufSize);
#else
               strcpy(bufferArr, PRODUCT_NAME);
#endif
            }
         return strlen(PRODUCT_NAME);

         //
         // ModelName
         //
         case 5:
            if (bufferArr)
            {
               printf("\rSlave %3d: getDeviceIdObject ModelName\n", slaveAddr);
#ifdef HAS_STRNCPY
               strncpy(bufferArr, MODEL_NAME, maxBufSize);
#else
               strcpy(bufferArr, MODEL_NAME);
#endif
            }
         return strlen(MODEL_NAME);

         //
         // UserApplicationName
         //
         case 6:
            if (bufferArr)
            {
               printf("\rSlave %3d: getDeviceIdObject UserApplicationName\n", slaveAddr);
#ifdef HAS_STRNCPY
               strncpy(bufferArr, USER_APPLICATION_NAME, maxBufSize);
#else
               strcpy(bufferArr, USER_APPLICATION_NAME);
#endif
            }
         return strlen(USER_APPLICATION_NAME);

         //
         // CustomObject
         //
         case 128:
            if (bufferArr)
            {
               printf("\rSlave %3d: getDeviceIdObject CustomObject 128\n", slaveAddr);
               memcpy(bufferArr, CUSTOM_OBJECT, sizeof(CUSTOM_OBJECT));
            }
         return sizeof(CUSTOM_OBJECT);
      }
      return 0; // Return 0 for object not found
   }


  private:

   int slaveAddr;
   short regData[0x10000];
   char bitData[2000];

};


#endif // ifdef ..._H_INCLUDED
