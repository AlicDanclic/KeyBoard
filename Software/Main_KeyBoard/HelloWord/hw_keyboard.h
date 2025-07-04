#ifndef HELLO_WORD_KEYBOARD_FW_HW_KEYBOARD_H
#define HELLO_WORD_KEYBOARD_FW_HW_KEYBOARD_H

#include "spi.h"


class HWKeyboard
{
public:
    explicit HWKeyboard(SPI_HandleTypeDef* _spi) :
        spiHandle(_spi)
    {
        scanBuffer = &spiBuffer[1];

        // Enable clock
        HAL_GPIO_WritePin(CE_GPIO_Port,CE_Pin,GPIO_PIN_RESET);

        // Init RGB buffer
        for (uint8_t i = 0; i < HWKeyboard::LED_NUMBER; i++)
            SetRgbBufferByID(i, HWKeyboard::Color_t{0, 0, 0});
    }


    static const uint8_t IO_NUMBER = 11 * 8; // Used 11 pcs 74HC165
    static const uint8_t KEY_NUMBER = 82;
    static const uint8_t TOUCHPAD_NUMBER = 6;
    static const uint8_t LED_NUMBER = 85;
    static const uint16_t KEY_REPORT_SIZE = 1 + 16;
    static const uint16_t RAW_REPORT_SIZE = 1 + 32;
    static const uint16_t HID_REPORT_SIZE = KEY_REPORT_SIZE + RAW_REPORT_SIZE;

    enum KeyCode_t : int16_t
    {
        /*------------------------- HID report data -------------------------*/
        LEFT_CTRL = -8,LEFT_SHIFT = -7,LEFT_ALT = -6,LEFT_GUI = -5,
        RIGHT_CTRL = -4,RIGHT_SHIFT = -3,RIGHT_ALT = -2,RIGHT_GUI = -1,

        RESERVED = 0,ERROR_ROLL_OVER,POST_FAIL,ERROR_UNDEFINED,
        A,B,C,D,E,F,G,H,I,J,K,L,M,
        N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
        NUM_1/*1!*/,NUM_2/*2@*/,NUM_3/*3#*/,NUM_4/*4$*/,NUM_5/*5%*/,
        NUM_6/*6^*/,NUM_7/*7&*/,NUM_8/*8**/,NUM_9/*9(*/,NUM_0/*0)*/,
        ENTER,ESC,BACKSPACE,TAB,SPACE,
        MINUS/*-_*/,EQUAL/*=+*/,LEFT_U_BRACE/*[{*/,RIGHT_U_BRACE/*]}*/,
        BACKSLASH/*\|*/,NONE_US/**/,SEMI_COLON/*;:*/,QUOTE/*'"*/,
        GRAVE_ACCENT/*`~*/,COMMA/*,<*/,PERIOD/*.>*/,SLASH/*/?*/,
        CAP_LOCK,F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,F11,F12,
        PRINT,SCROLL_LOCK,PAUSE,INSERT,HOME,PAGE_UP,DELETE,END,PAGE_DOWN,
        RIGHT_ARROW,LEFT_ARROW,DOWN_ARROW,UP_ARROW,PAD_NUM_LOCK,
        PAD_SLASH,PAD_ASTERISK,PAD_MINUS,PAD_PLUS,PAD_ENTER,
        PAD_NUM_1,PAD_NUM_2,PAD_NUM_3,PAD_NUM_4,PAD_NUM_5,
        PAD_NUM_6,PAD_NUM_7,PAD_NUM_8,PAD_NUM_9,PAD_NUM_0,
        PAD_PERIOD , NONUS_BACKSLASH,APPLICATION,POWER,PAD_EQUAL,
        F13,F14,F15,F16,F17,F18,F19,F20,F21,F22,F23,F24, EXECUTE,
        HELP,MENU,SELECT,STOP,AGAIN,UNDO,CUT,COPY,PASTE,FIND,MUTE,VOLUME_UP,VOLUME_DOWN,
        FN = 1000
        /*------------------------- HID report data -------------------------*/
    };

    struct Color_t
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };
    enum SpiWs2812Byte_t : uint8_t
    {
        WS_HIGH = 0xFE,
        WS_LOW = 0xE0
    };

    uint8_t* ScanKeyStates();
    void ApplyDebounceFilter(uint32_t _filterTimeUs = 100);
    uint8_t* Remap(uint8_t _layer = 1);
    void SyncLights();
    bool FnPressed();
    bool KeyPressed(KeyCode_t _key);
    void Press(KeyCode_t _key);
    void Release(KeyCode_t _key);
    uint8_t* GetHidReportBuffer(uint8_t _reportId);
    uint8_t  GetTouchBarState(uint8_t _id = 0);
    void SetRgbBufferByID(uint8_t _keyId, Color_t _color, float _brightness = 1);


    int16_t keyMap[5][IO_NUMBER] = {
        {67,44,45,46,47,24,25,26,27,8,9,10,11,3,
            66,81,63,59,55,51,43,39,32,28,15,14,13,12,2,
            65,80,62,58,54,50,42,38,31,29,19,18,17,16,1,
            64,61,57,53,49,41,37,35,33,23,22,21,20,0,
            71,60,56,52,48,40,36,34,30,75,74,73,76,4,
            70,69,68,72,79,78,77,7,6,5,
            86,84,82,87,85,83}, // TouchBar index

        {ESC,F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,F11,F12,PAD_NUM_LOCK,
            GRAVE_ACCENT,NUM_1,NUM_2,NUM_3,NUM_4,NUM_5,NUM_6,NUM_7,NUM_8,NUM_9,NUM_0,MINUS,EQUAL,BACKSPACE,FN,
            TAB,Q,W,E,R,T,Y,U,I,O,P,LEFT_U_BRACE,RIGHT_U_BRACE,BACKSLASH,DELETE,
            CAP_LOCK,A,S,D,F,G,H,J,K,L,SEMI_COLON,QUOTE,ENTER,INSERT,
            LEFT_SHIFT,Z,X,C,V,B,N,M,COMMA,PERIOD,SLASH,RIGHT_SHIFT,UP_ARROW,HOME,
            LEFT_CTRL,LEFT_GUI,LEFT_ALT,SPACE,RIGHT_ALT,RIGHT_GUI,RIGHT_CTRL,LEFT_ARROW,DOWN_ARROW,RIGHT_ARROW },

        {ESC,F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,F11,F12,PAD_NUM_LOCK,
            GRAVE_ACCENT,NUM_1,NUM_2,NUM_3,NUM_4,NUM_5,NUM_6,NUM_7,NUM_8,NUM_9,NUM_0,MINUS,EQUAL,BACKSPACE,HOME,
            TAB,A,B,C,D,E,F,G,H,I,J,LEFT_U_BRACE,RIGHT_U_BRACE,BACKSLASH,DELETE,
            CAP_LOCK,K,L,M,N,O,P,Q,R,S,SEMI_COLON,QUOTE,ENTER,INSERT,
            LEFT_SHIFT,T,U,V,W,X,Y,Z,COMMA,PERIOD,SLASH,RIGHT_SHIFT,A,PAGE_DOWN,
            LEFT_CTRL,LEFT_GUI,LEFT_ALT,SPACE,RIGHT_ALT,FN,RIGHT_CTRL,LEFT_ARROW,DOWN_ARROW,RIGHT_ARROW }
    };

    volatile bool isRgbTxBusy;
    bool isCapsLocked = false;


private:
    SPI_HandleTypeDef* spiHandle;
    uint8_t spiBuffer[IO_NUMBER / 8 + 1]{};
    uint8_t* scanBuffer;
    uint8_t debounceBuffer[IO_NUMBER / 8 + 1]{};
    uint8_t hidBuffer[HID_REPORT_SIZE]{};
    uint8_t remapBuffer[IO_NUMBER / 8]{};
    uint8_t rgbBuffer[LED_NUMBER][3][8]{};
    uint8_t wsCommit[64] = {0};
    uint8_t brightnessPreDiv = 2; // 1/4
};


#endif
