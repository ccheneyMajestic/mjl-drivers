/***************************************************************************
*                                Majestic Labs © 2023
* File: SL_ble.h
* Workspace: Strength Loop v4
* Version: v1.0.0
* Author: C. Cheney
*
* Brief: Bluetooth application for Strength Loop 
*
* 2023.05.03  - Document Created
********************************************************************************/
#include "SL_ble.h"

/* BLE State */
SL_BLE_S bleState;

/*******************************************************************************
* Function Name: strengthLoop_ble()
********************************************************************************
* \brief
*   Bluetooth callback function for strengthloop. Handle all BLE functionality  
*
* \param eventCode [in]
* Code of the latest event that occured 
*
* \param eventParam [in]
*   Pointer to the struct (type depends on eventCode)
*
* \return
*  Error code of the operation
*******************************************************************************/
void strengthLoop_ble(uint32_t eventCode, void * eventParam){
    /* Act depending on the event*/
    switch(eventCode){
        /**********************************************************
        *                       General Events
        ***********************************************************/
        /* Stack initialized; ready for advertisement*/
        case CYBLE_EVT_STACK_ON: {
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            break;
        }
        /* Restart Advertisement on timeout */
        case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP: {
            if(CyBle_GetState() == CYBLE_STATE_DISCONNECTED) {
                CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            }
            break;
        }
        /**********************************************************
        *                       GAP Events
        ***********************************************************/
        /* Start adversiting when disconnected */
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED: {
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);

            break;
        }
        /**********************************************************
        *                       GATT Events
        ***********************************************************/
        /* This event is generated at the GAP peripheral end after connection
        is completed with the peer central device */
        case CYBLE_EVT_GATT_CONNECT_IND: {
            bleState.connHandle = *  (CYBLE_CONN_HANDLE_T *) eventParam;
            break;
        }

        /* Unhandled event */
        default: {
            break;
        }
    /* End Switch */
    }
}

/* [] END OF FILE */