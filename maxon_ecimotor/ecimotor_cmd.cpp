#include "ecimotor.h"

#include <iostream>
#include <list>
#include <math.h>
#include <string.h>
#include <sstream>
#include <Windows.h>
#include <thread>



///////////////////////////////////////////////////
#ifndef MMC_SUCCESS
#define MMC_SUCCESS 0
#endif

#ifndef MMC_FAILED
#define MMC_FAILED 1
#endif

void  LogInfo(std::string message);

void LogInfo(std::string message)
{
    std::cout << message << std::endl;
}

void SeparatorLine()
{
    const int lineLength = 65;
    for (int i = 0; i < lineLength; i++)
    {
        std::cout << "-";
    }
    std::cout << std::endl;
}

using std::this_thread::sleep_for;
/////////////////////////////////////////////////////


void ecimotor::vcsStart()
{
    m_KeyHandle = nullptr;

    /*
    /////////////////////////////////////////////////////////////////////////////////////////////
    //user setting parameters
    const char* deviceName = "EPOS4";                      //EPOS4
    const char* protocolStackName = "CANopen";             //USB=MAAXON SERIAL V2, CAN=CANopen
    const char* interfaceName = "Kvaser_Leaf Light v2 0";  //USB=USB, CAN=Kvaser_Leaf Light v2 0
    const char* portName = "CAN0";                         //USB=USB0, CAN=CAN0
    DWORD errorCode = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////
    */
      /////////////////////////////////////////////////////////////////////////////////////////////
    //user setting parameters
    const char* deviceName = "EPOS4";                      //EPOS4
    const char* protocolStackName = "MAXON SERIAL V2";             //USB=MAXON SERIAL V2, CAN=CANopen
    const char* interfaceName = "USB";  //USB=USB, CAN=Kvaser_Leaf Light v2 0
    const char* portName = "USB0";                         //USB=USB0, CAN=CAN0
    DWORD errorCode = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////


    //Open device
    HANDLE hNewKeyHandle;

    //Settings
    m_oImmediately = TRUE;
    m_oUpdateActive = FALSE;
    m_usNodeId = 1;

    //Close Previous Device
    if (m_KeyHandle)
    {
        if (m_KeyHandle) VCS_CloseDevice(m_KeyHandle, &m_ulErrorCode);
        m_KeyHandle = 0;
    }



    //Dialog mode
    //hNewKeyHandle = VCS_OpenDeviceDlg(&m_ulErrorCode);

    //No Dailog mode
    hNewKeyHandle = VCS_OpenDevice(deviceName, protocolStackName, interfaceName, portName, &errorCode);

    //CAN mode
    //VCS_SetProtocolStackSettings(hNewKeyHandle, 1000000, 500, &m_ulErrorCode);
    //std::cout << "Error code:" << m_ulErrorCode << std::endl;



    if (hNewKeyHandle)
    {
        m_KeyHandle = hNewKeyHandle;

        //Clear Error History
        if (VCS_ClearFault(m_KeyHandle, m_usNodeId, &m_ulErrorCode))
        {
            //Read Operation Mode
            if (VCS_GetOperationMode(m_KeyHandle, m_usNodeId, &m_bMode, &m_ulErrorCode))
            {
                //Read Position Profile Objects
                if (VCS_GetPositionProfile(m_KeyHandle, m_usNodeId, &m_ulProfileVelocity, &m_ulProfileAcceleration, &m_ulProfileDeceleration, &m_ulErrorCode))
                {
                    //Write Profile Position Mode
                    if (VCS_SetOperationMode(m_KeyHandle, m_usNodeId, OMD_PROFILE_POSITION_MODE, &m_ulErrorCode))
                    {
                        //Write Profile Position Objects
                        if (VCS_SetPositionProfile(m_KeyHandle, m_usNodeId, 7000, 10000, 10000, &m_ulErrorCode))
                        {
                            //Read Actual Position
                            if (VCS_GetPositionIs(m_KeyHandle, m_usNodeId, &m_lStartPosition, &m_ulErrorCode))
                            {
                                std::cout << "m_lStartPosition" << m_lStartPosition << std::endl;
                            }
                        }
                    }
                }
            }
        }

        std::cout << "Error code:" << m_ulErrorCode << std::endl;

        SeparatorLine();
    }
    else
    {
        std::cout << "Can't open device!\n";
    }

    m_oInitialisation = TRUE;

}

void ecimotor::vcsEnable()
{
    BOOL oFault = FALSE;


    if (!VCS_GetFaultState(m_KeyHandle, m_usNodeId, &oFault, &m_ulErrorCode))
    {
        //ShowErrorInformation(m_ulErrorCode);
        return;
    }

    if (oFault)
    {
        if (!VCS_ClearFault(m_KeyHandle, m_usNodeId, &m_ulErrorCode))
        {
            //ShowErrorInformation(m_ulErrorCode);
            return;
        }
    }

    if (!VCS_SetEnableState(m_KeyHandle, m_usNodeId, &m_ulErrorCode))
    {
        //ShowErrorInformation(m_ulErrorCode);
    }
}

void ecimotor::vcsDisable()
{

    if (!VCS_SetDisableState(m_KeyHandle, m_usNodeId, &m_ulErrorCode))
    {
        //ShowErrorInformation(m_ulErrorCode);
    }
}


void ecimotor::vcsSetPosition(const long ltargetPositon)
{
    // set position
    m_lTargetPosition = ltargetPositon;

    BOOL oEnable = TRUE;
    BOOL oResult = VCS_GetEnableState(m_KeyHandle, m_usNodeId, &oEnable, &m_ulErrorCode);
    WORD m_pInputs;

    //VCS_SetAllDigitalOutputs(m_KeyHandle, m_usNodeId,DOC_GENERAL_PURPOSE_A, &m_ulErrorCode);
    VCS_DigitalOutputConfiguration(m_KeyHandle, m_usNodeId, 1 , 15 , 1, 1, 1, &m_ulErrorCode);
    VCS_GetAllDigitalOutputs(m_KeyHandle, m_usNodeId, &m_pInputs, &m_ulErrorCode);
    std::cout << m_pInputs << std::endl;

    SeparatorLine();
    LogInfo("VCS_ActivateProfilePositionMode start");
    SeparatorLine();

    //VCS_SetAllDigitalOutputs(m_KeyHandle, m_usNodeId, 1, &m_ulErrorCode);
    VCS_DigitalOutputConfiguration(m_KeyHandle, m_usNodeId,1, 15, 0, 1, 1, &m_ulErrorCode);
    VCS_GetAllDigitalOutputs(m_KeyHandle, m_usNodeId, &m_pInputs, &m_ulErrorCode);
    std::cout << m_pInputs << std::endl;

    // 
    if (VCS_GetPositionIs(m_KeyHandle, m_usNodeId, &m_lActualValue, &m_ulErrorCode))
    {
        if ((!oEnable) && (abs(m_lActualValue - m_lTargetPosition) >= 50))
            vcsEnable();

        if (VCS_ActivateProfilePositionMode(m_KeyHandle, m_usNodeId, &m_ulErrorCode) == 0)

            /////////////////////////////////////////////////////////////////////////////////
            //position mode

            std::stringstream msg;

        std::list<long> positionList;

        positionList.push_back(200000);
        positionList.push_back(-200000);
        positionList.push_back(0);

        //////////////////////////////////////////////////////////////////////////////////

        for (std::list<long>::iterator it = positionList.begin(); it != positionList.end(); it++)
        {
            long m_lTargetPosition = (*it);

            std::stringstream msg;
            msg << "move to position = " << m_lTargetPosition << ", node = " << m_usNodeId;

            LogInfo(msg.str());


            /// ///////////////////////////////////////////////////////////////////////////////


            if (!VCS_MoveToPosition(m_KeyHandle, m_usNodeId, m_lTargetPosition, m_oRadio, m_oImmediately, &m_ulErrorCode))
            {
                std::cout << m_ulErrorCode << std::endl;

                break;
            }

            sleep_for(std::chrono::milliseconds(1600));

            vcsIsArrivePosition();

            sleep_for(std::chrono::milliseconds(500));

        }

        SeparatorLine();


        /*
        if (lResult == MMC_SUCCESS)
        {
            LogInfo("halt position movement");
        */

        VCS_HaltPositionMovement(m_KeyHandle, m_usNodeId, &m_ulErrorCode);

        LogInfo("halt position movement");
        // LogError("VCS_HaltPositionMovement", lResult, p_rlErrorCode);
         //lResult = MMC_FAILED;

// }


    }

    SeparatorLine();
}

bool ecimotor::vcsIsArrivePosition()
{
    /*
    VCS_GetPositionIs(m_KeyHandle, m_usNodeId, &m_lActualValue, &m_ulErrorCode);

        std::cout << "m_lActualValue " << m_lActualValue << std::endl;
    */

    if (VCS_GetPositionIs(m_KeyHandle, m_usNodeId, &m_lActualValue, &m_ulErrorCode))
    {

        std::cout << "ActualValue = " << m_lActualValue << std::endl;


        /*
       if (abs(m_lActualValue + 80000) < 50)
        {
            m_lTargetPosition = m_lActualValue;//this->vcsDisable();
            return true;
        }
        else
            return false;
    }
    else
    {
        */
        return true;

    }

}


void ecimotor::vcsStop()
{
    this->vcsDisable();

    //Restore Settings
    if (m_oInitialisation)
    {
        VCS_SetDisableState(m_KeyHandle, m_usNodeId, &m_ulErrorCode);
        VCS_SetOperationMode(m_KeyHandle, m_usNodeId, m_bMode, &m_ulErrorCode);
        VCS_SetPositionProfile(m_KeyHandle, m_usNodeId, m_ulProfileVelocity, m_ulProfileAcceleration, m_ulProfileDeceleration, &m_ulErrorCode);
    }


    //Close Device
    if (m_KeyHandle) VCS_CloseDevice(m_KeyHandle, &m_ulErrorCode);

    SeparatorLine();

    return;

}
