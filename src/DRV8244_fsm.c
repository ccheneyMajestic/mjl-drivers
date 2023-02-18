/***************************************************************************
*                              Majestic Labs  © 2022
*                               
*
* File: DRV8244_fsm.h
* Workspace: Cloud Chamber
* Project: finite state machine (fsm)
* Version: 1.0.0
* Authors: C. Cheney
*
* Brief:
*   Header for DRV8244_fsm.cpp, the template for the DRV fsm
*
* 2022.04.04  - Document Created
********************************************************************************/
#include "DRV8244_fsm.h"

/* Private Helper functions */
uint32_t fsm_template_actionName1(fsm_template_state_s *const state);

/* Private Helper functions */
uint32_t fsm_template_processState_state1(fsm_template_state_s *const state);


/*******************************************************************************
* Function Name: fsm_template_init()
****************************************************************************//**
* \brief
*  Initialize the template finite state machine, using the data in the configuration
*   Structure. Must be called before any other template functions
*
* \param state [in/out]
*  Pointer to the state struct  
*
* \param cfg [in]
*  Pointer to the configuration struct  
*
* \return
*  Error of the command
*******************************************************************************/
uint32_t fsm_template_init(fsm_template_state_s *const state, fsm_template_cfg_s *const cfg){
    uint32_t error = 0;
    /* Input validation */
    if((NULL == cfg->fn_getTime)) {
        error |= error_template_common_invalid;
    }
    /* Initialize state object */
    if(0u == error) {
        /* copy input params */
        state->fn_getTime = cfg->fn_getTime;
        state->object_actuator1 = cfg->template_stateObject1;
        state->object_actuator2 = cfg->template_stateObject2;
        state->object_sensor1 = cfg->template_stateObject3;
        state->object_sensor2 = cfg->template_stateObject4;

        /* Set defaults */
        state->current_state = fsm_template_STATE_1;
        state->next_state = fsm_template_STATE_1;
        state->prev_state = fsm_template_STATE_2;
        state->parameter1 = 0;
        state->parameter2 = 0;
        state->time_LastUpdated = 0;
        /* Indicate success */
        state->output = 0;
        state->_init = true;
    }
    else {
        state->_init = false;
    }
    return error;
}


/*******************************************************************************
* Function Name: fsm_template_processState()
****************************************************************************//**
* \brief
*  Processes the system state and transitions
*
* \param state [in/out]
*  Pointer to the state struct  
*
* \return
*  Error of the command
*******************************************************************************/
uint32_t fsm_template_processState(fsm_template_state_s *const state){
    uint32_t error = 0;
    /* Ensure object is initialized */
    if(false == state->_init){
        error |= ERROR_TEMPLATE_COMMON_INIT;
    }
    /* Main structure */
    if(0u == error){
        uint32_t elaspedTime = state->fn_getTime() - state->timeLastUpdated;
        /* Read sensors */


        /* Universal actions -- Handle immediate state changes */
        if( (true == state->parameter1) && (true == state->parameter2) ) {
            state->next_state = fsm_template_STATE_2;
        }
        else if (/* // Condition 2 */){

        }
        /* Proceed with specific actions */
        else {
            /* Process state */
            switch(state->current_state) {
                /* State 1 description */
                case fsm_template_STATE_1: {
                    error |= fsm_template_processState_state1(state);
                    break;
                }
                /* State 2 description */
                case fsm_template_STATE_2: {
                    error |= fsm_template_processState_state2(state);
                    break;
                }
                /* Should never arrive here */
                default:{
                    error|= ERROR_TEMPLATE_COMMON_STATE;
                }
            }
        }
        

    }

    return error;
}

/*******************************************************************************
* Function Name: fsm_template_actionName1()
****************************************************************************//**
* \brief
*  Perform the action 
*
* \param state [in/out]
*  Pointer to the state struct  
*
* \return
*  Error of the command
*******************************************************************************/
uint32_t fsm_template_actionName1(fsm_template_state_s *const state){
    uint32_t error = 0;
    /* Action function */
    // ...

    return error;
}

/*******************************************************************************
* Function Name: fsm_template_processState_state1()
****************************************************************************//**
* \brief
*  State processing action function 1 template
*
* \param state [in/out]
*  Pointer to the state struct  
*
* \return
*  Error of the command
*******************************************************************************/
uint32_t fsm_template_processState_state1(fsm_template_state_s *const state);
    uint32_t error = 0;

    return error;
}

/*******************************************************************************
* Function Name: fsm_template_processState_state2()
****************************************************************************//**
* \brief
*  State processing action function 2 template
*
* \param state [in/out]
*  Pointer to the state struct  
*
* \return
*  Error of the command
*******************************************************************************/
uint32_t fsm_template_processState_state2(fsm_template_state_s *const state) {
    uint32_t error = 0;

    return error;
}

/* [] END OF FILE */