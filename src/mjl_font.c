/***************************************************************************
*                             Majestic Labs © 2023
*
* File: mjl_font.c
* Workspace: MJL Driver Library 
* Version: v1.0.0
* Author: C. Cheney
*
* Brief: The majestic lab font
*   
*
* 2023.04.17  - Document Created
********************************************************************************/
#include "mjl_font.h"


/* Bitmaps for digits 0-9 */
const uint8_t zero_8x16[UI_TEXT_8x16_LEN]     = {0xFE, 0xFE, 0x03, 0x03, 0x03, 0x03, 0xFE, 0xFE, 0x7F, 0x7F, 0xC0, 0xC0, 0xC0, 0xC0, 0x7F, 0x7F};
const uint8_t one_8x16[UI_TEXT_8x16_LEN]      = {0x00, 0x10, 0x1C, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xC0, 0xC0, 0xC0, 0xFF, 0xFF, 0xC0, 0xC0, 0xC0};
const uint8_t two_8x16[UI_TEXT_8x16_LEN]      = {0x1C, 0x9E, 0x83, 0x83, 0x83, 0xFF, 0xFE, 0x00, 0xFF, 0xFF, 0xC1, 0xC1, 0xC1, 0xC1, 0xC0, 0x00};
const uint8_t three_8x16[UI_TEXT_8x16_LEN]    = {0x1E, 0x1F, 0x03, 0xC3, 0xC3, 0xFF, 0x7E, 0x00, 0x78, 0xF8, 0xC0, 0xC1, 0xC1, 0xFF, 0x7F, 0x00};
const uint8_t four_8x16[UI_TEXT_8x16_LEN]     = {0x80, 0xE0, 0xF8, 0x9C, 0x8E, 0xFF, 0xFF, 0x80, 0x01, 0x01, 0x01, 0x01, 0x01, 0xFF, 0xFF, 0x01};
const uint8_t five_8x16[UI_TEXT_8x16_LEN]     = {0x7F, 0xFF, 0xC3, 0xC3, 0xC3, 0xC3, 0x83, 0x00, 0x78, 0xF8, 0xC0, 0xC0, 0xC0, 0xFF, 0x7F, 0x00};
const uint8_t six_8x16[UI_TEXT_8x16_LEN]      = {0xFC, 0xFE, 0x87, 0x83, 0x83, 0x9F, 0x9E, 0x00, 0x3F, 0xFF, 0xC1, 0xC1, 0xC1, 0xFF, 0x7F, 0x00};
const uint8_t seven_8x16[UI_TEXT_8x16_LEN]    = {0x03, 0x03, 0x03, 0x83, 0xE3, 0xFF, 0x3F, 0x07, 0x00, 0xC0, 0xF8, 0xFF, 0x0F, 0x01, 0x00, 0x00};
const uint8_t eight_8x16[UI_TEXT_8x16_LEN]    = {0x3E, 0xFE, 0xC3, 0xC3, 0xC3, 0xC3, 0xFE, 0x3E, 0x3F, 0x7F, 0xC0, 0xC0, 0xC0, 0xC0, 0x7F, 0x3F};
const uint8_t nine_8x16[UI_TEXT_8x16_LEN]     = {0x7C, 0xFE, 0x83, 0x83, 0x83, 0x83, 0xFE, 0xFC, 0x38, 0x78, 0xE1, 0xC1, 0xC1, 0xC1, 0xFF, 0x7F};
const uint8_t* digits_8x16[10]  = {zero_8x16, one_8x16, two_8x16, three_8x16, four_8x16, five_8x16, six_8x16, seven_8x16, eight_8x16, nine_8x16};
/* Icon bitmaps */
const uint8_t batteryOutline[UI_ICON_BATTERY_LEN]= {0x3C, 0xFF, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xFF};
const uint8_t battery_1bar[UI_ICON_BATTERY_LEN]=   {0x3C, 0xFF, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xBD, 0xBD, 0x81, 0xFF};

const uint8_t historyOutline[UI_ICON_HISTORY_LEN]= {0x00, 0xF8, 0x08, 0x08, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x08, 0x08, 0xF8, 0x00,
                                   0x00, 0xFF, 0x00, 0x00, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x00, 0x00, 0xFF, 0x00, 
                                   0xC0, 0xDF, 0xD0, 0xD0, 0xD1, 0xD1, 0xD1, 0xD1, 0xD1, 0xD1, 0xD1, 0xD1, 0xD0, 0xD0, 0xDF, 0xC0};
const uint8_t resetOutline[UI_ICON_RESET_LEN]  = {0x00, 0x00, 0x80, 0xE0, 0x30, 0x18, 0x18, 0x18, 0x18, 0x98, 0x18, 0x18, 0x30, 0x60, 0xC0, 0x00, 
                                   0x04, 0x0C, 0x1F, 0x0C, 0x04, 0x00, 0x20, 0x30, 0x18, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x81, 0xFF, 
                                   0xC0, 0xC0, 0xC0, 0xC6, 0xCC, 0xD8, 0xD8, 0xD8, 0xD8, 0xD8, 0xD8, 0xD8, 0xCC, 0xC6, 0xC3, 0xC0};

/*  Alphabet */
const uint8_t A_8x16[UI_TEXT_8x16_LEN] = {0x00, 0xE0, 0x7C, 0x1F, 0x1F, 0x7C, 0xE0, 0x00, 0xFC, 0x1F, 0x18, 0x18, 0x18, 0x18, 0x1F, 0xFC};
const uint8_t B_8x16[UI_TEXT_8x16_LEN] = {0xFF, 0xFF, 0x83, 0x83, 0x83, 0xC3, 0x7E, 0x3C, 0xFF, 0xFF, 0xC1, 0xC1, 0xC1, 0xC3, 0x7E, 0x3C};
const uint8_t C_8x16[UI_TEXT_8x16_LEN] = {0xFC, 0xFE, 0x03, 0x03, 0x03, 0x03, 0x1E, 0x1C, 0x3F, 0x7F, 0xE0, 0xC0, 0xC0, 0x60, 0x78, 0x38};
const uint8_t D_8x16[UI_TEXT_8x16_LEN] = {0xFF, 0xFF, 0x03, 0x03, 0x03, 0x03, 0xFE, 0xFC, 0xFF, 0xFF, 0xC0, 0xC0, 0xC0, 0xC0, 0x7F, 0x3F};
const uint8_t E_8x16[UI_TEXT_8x16_LEN] = {0xFF, 0xFF, 0x83, 0x83, 0x83, 0x83, 0x83, 0x01, 0xFF, 0xFF, 0xC1, 0xC1, 0xC1, 0xC1, 0xC1, 0xC0};
const uint8_t F_8x16[UI_TEXT_8x16_LEN] = {0xFF, 0xFF, 0x83, 0x83, 0x83, 0x83, 0x03, 0x03, 0xFF, 0xFF, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00};
const uint8_t G_8x16[UI_TEXT_8x16_LEN] = {0xFC, 0xFF, 0x01, 0x01, 0x01, 0x83, 0x8F, 0x8E, 0x3F, 0x7F, 0xC0, 0x80, 0x80, 0x81, 0xFF, 0x7F};
const uint8_t H_8x16[UI_TEXT_8x16_LEN] = {0xFF, 0xFF, 0x80, 0x80, 0x80, 0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x01, 0x01, 0x01, 0xFF, 0xFF};
const uint8_t I_8x16[UI_TEXT_8x16_LEN] = {0x03, 0x03, 0x03, 0xFF, 0xFF, 0x03, 0x03, 0x03, 0xC0, 0xC0, 0xC0, 0xFF, 0xFF, 0xC0, 0xC0, 0xC0};
const uint8_t J_8x16[UI_TEXT_8x16_LEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x70, 0xF0, 0xC0, 0xC0, 0xC0, 0xC0, 0xFF, 0x3F};
const uint8_t K_8x16[UI_TEXT_8x16_LEN] = {0xFF, 0xFF, 0xC0, 0xC0, 0xF8, 0xFE, 0x07, 0x01, 0xFF, 0xFF, 0x03, 0x01, 0x01, 0x3F, 0xFE, 0xF0};
const uint8_t L_8x16[UI_TEXT_8x16_LEN] = {0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0};
const uint8_t M_8x16[UI_TEXT_8x16_LEN] = {0xFF, 0xF8, 0xE0, 0x00, 0x00, 0xE0, 0xF8, 0xFF, 0xFF, 0x00, 0x01, 0x0F, 0x0F, 0x01, 0x00, 0xFF};
const uint8_t N_8x16[UI_TEXT_8x16_LEN] = {0xFF, 0xFF, 0xFC, 0xF0, 0xC0, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x03, 0x0F, 0x3F, 0xFF, 0xFF};
const uint8_t O_8x16[UI_TEXT_8x16_LEN] = {0xFE, 0xFE, 0x03, 0x03, 0x03, 0x03, 0xFE, 0xFE, 0x7F, 0x7F, 0xC0, 0xC0, 0xC0, 0xC0, 0x7F, 0x7F};
const uint8_t P_8x16[UI_TEXT_8x16_LEN] = {0xFF, 0xFF, 0x83, 0x83, 0x83, 0xC3, 0x7E, 0x3C, 0xFF, 0xFF, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00};
const uint8_t Q_8x16[UI_TEXT_8x16_LEN] = {0xFE, 0xFE, 0x03, 0x03, 0x03, 0x03, 0xFE, 0xFE, 0x7F, 0xFF, 0xC0, 0xC0, 0xF0, 0x70, 0xFF, 0xDF};
const uint8_t R_8x16[UI_TEXT_8x16_LEN] = {0xFF, 0xFF, 0x83, 0x83, 0x83, 0x83, 0xFF, 0xFE, 0xFF, 0xFF, 0x01, 0x01, 0x0F, 0x3F, 0xF9, 0xE0};
const uint8_t S_8x16[UI_TEXT_8x16_LEN] = {0xFE, 0xFF, 0x83, 0x83, 0x83, 0x83, 0x83, 0x03, 0xC0, 0xC1, 0xC1, 0xC1, 0xC1, 0xC1, 0xFF, 0x7F};
const uint8_t T_8x16[UI_TEXT_8x16_LEN] = {0x03, 0x03, 0x03, 0xFF, 0xFF, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00};
const uint8_t U_8x16[UI_TEXT_8x16_LEN] = {0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x3F, 0xFF, 0xC0, 0xC0, 0xC0, 0xC0, 0xFF, 0x3F};
const uint8_t V_8x16[UI_TEXT_8x16_LEN] = {0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x0F, 0x3F, 0x30, 0xF0, 0xF0, 0x30, 0x3F, 0x0F};
const uint8_t W_8x16[UI_TEXT_8x16_LEN] = {0xFF, 0xFF, 0x00, 0xE0, 0xE0, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0x01, 0x01, 0x3F, 0xFF, 0xFF};
const uint8_t X_8x16[UI_TEXT_8x16_LEN] = {0x01, 0x07, 0x3C, 0xF0, 0xF0, 0x3C, 0x07, 0x01, 0x80, 0xE0, 0x3C, 0x0F, 0x0F, 0x3C, 0xE0, 0x80};
const uint8_t Y_8x16[UI_TEXT_8x16_LEN] = {0x1F, 0x7C, 0xE0, 0x00, 0x00, 0xE0, 0x7C, 0x1F, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0x01, 0x00, 0x00};
const uint8_t Z_8x16[UI_TEXT_8x16_LEN] = {0x03, 0x03, 0x03, 0x83, 0xC3, 0xF3, 0x7F, 0x1F, 0xF0, 0xFC, 0xDE, 0xC7, 0xC3, 0xC0, 0xC0, 0xC0};
const uint8_t* alphabet_8x16[26] = {A_8x16, B_8x16, C_8x16, D_8x16, E_8x16, F_8x16, G_8x16, H_8x16, I_8x16, J_8x16, K_8x16, L_8x16, M_8x16, N_8x16, O_8x16, P_8x16, Q_8x16, R_8x16, S_8x16, T_8x16, U_8x16, V_8x16, W_8x16, X_8x16, Y_8x16, Z_8x16};




/* [] END OF FILE */