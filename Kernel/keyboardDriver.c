#include <keyboardDriver.h>
#include <lib.h>

#define HID_KEYMAP_SIZE 256
#define SHIFT_PRESSED_INDEX 1
#define ALTGR_PRESSED_INDEX 2
#define BUFFER_SIZE 1024

static const char HID_keymap[HID_KEYMAP_SIZE][3] = {
    [4]  = {'a', 'A'},
    [5]  = {'b', 'B'},
    [6]  = {'c', 'C'},
    [7]  = {'d', 'D'},
    [8]  = {'e', 'E'},
    [9]  = {'f', 'F'},
    [10] = {'g', 'G'},
    [11] = {'h', 'H'},
    [12] = {'i', 'I'},
    [13] = {'j', 'J'},
    [14] = {'k', 'K'},
    [15] = {'l', 'L'},
    [16] = {'m', 'M'},
    [17] = {'n', 'N'},
    [18] = {'o', 'O'},
    [19] = {'p', 'P'},
    [20] = {'q', 'Q', '@'},
    [21] = {'r', 'R'},
    [22] = {'s', 'S'},
    [23] = {'t', 'T'},
    [24] = {'u', 'U'},
    [25] = {'v', 'V'},
    [26] = {'w', 'W'},
    [27] = {'x', 'X'},
    [28] = {'y', 'Y'},
    [29] = {'z', 'Z'},
    [30] = {'1', '!'},
    [31] = {'2', '"'},
    [32] = {'3', '#'},
    [33] = {'4', '$'},
    [34] = {'5', '%'},
    [35] = {'6', '&'},
    [36] = {'7', '/'},
    [37] = {'8', '('},
    [38] = {'9', ')'},
    [39] = {'0', '='},
    [40] = {'\n', '\n'},     /* Enter      */
    /* ... */
    [42] = {'\b', '\b'},     /* Backspace  */
    [43] = {'\t', '\t'},     /* Tab        */
    [44] = {' ', ' '},       /* Space      */
    [45] = {'\'', '?', '\\'},
    [46] = {'¿', '¡'},
    [47] = {'´', '¨'},
    [48] = {'+', '*', '~'},
    [49] = {'\\', '|', '¬'},
    [50] = {'}', ']', '`'},
    [51] = {'ñ', 'Ñ'},       /* Ñ, usa el mismo keycode que ; en US */
    [52] = {'{', '[', '^'},
    [53] = {'|', '°'},
    [54] = {',', ';'},
    [55] = {'.', ':'},
    [56] = {'-', '_'},
    [57] = {},               /* Caps lock  */
    /* ... */
    [76] = {'\x7f', '\x7f'}, /* Delete                    */
    /* ... */
    [84] = {'/'},            /* Keypad /                  */
    [85] = {'*'},            /* Keypad *                  */
    [86] = {'-'},            /* Keypad -                  */
    [87] = {'+'},            /* Keypad +                  */
    [88] = {'\n'},           /* Keypad Enter              */
    [89] = {'1'},            /* Keypad 1 and End          */
    [90] = {'2'},            /* Keypad 2 and Down Arrow   */
    [91] = {'3'},            /* ...                       */
    [92] = {'4'},
    [93] = {'5'},
    [94] = {'6'},
    [95] = {'7'},
    [96] = {'8'},
    [97] = {'9'},
    [98] = {'0'},
    [99] = {'.', '\x7f'},    /* Keypad . and Delete       */
    [100] = {'<', '>'},
    [103] = {'=', '='},      /* Keypad =                  */
};

#define PS2_KEYMAP_SIZE 128
#define PS2_LSHIFT_MAKE 0x2A
#define PS2_RSHIFT_MAKE 0x36
#define PS2_ALTGR_MAKE 0x38
#define PS2_CAPSLOCK_MAKE 0x3A
#define PS2_ESC_MAKE 0x01

// scan-code → { unshifted, shifted, AltGr }
// only “make” codes; when you read a break code (0x80+make) mask it off:
//   uint8_t sc = getKeyPressed() & 0x7F;
//   char c = la_keymap[sc][ shift ? 1 : (altgr ? 2 : 0) ];
static const char ps2_make_keymap[PS2_KEYMAP_SIZE][3] = {
    [0x02] = {'1','!' , '\0'},  // 1  and !
    [0x03] = {'2','"','\0'},    // 2  and "
    [0x04] = {'3','#','\0'},    // 3  and #
    [0x05] = {'4','$','\0'},    // 4  and $
    [0x06] = {'5','%','\0'},    // 5  and %
    [0x07] = {'6','&','\0'},    // 6  and &
    [0x08] = {'7','/','\0'},    // 7  and /
    [0x09] = {'8','(','\0'},    // 8  and (
    [0x0A] = {'9',')','\0'},    // 9  and )
    [0x0B] = {'0','=' ,'\0'},   // 0  and =
    [0x0C] = {'-','_' ,'\0'},   // -  and _
    [0x0D] = {'+','*','~'},     // =-key → +  and *, AltGr+ = → ~
    [0x0E] = {'\b','\b','\0'},  // Backspace
    [0x0F] = {'\t','\t','\0'},  // Tab

    [0x10] = {'q','Q','@'},     // q  and Q, AltGr+Q → @
    [0x11] = {'w','W','\0'},
    [0x12] = {'e','E','€'},     // AltGr+E → €
    [0x13] = {'r','R','\0'},
    [0x14] = {'t','T','\0'},
    [0x15] = {'y','Y','\0'},
    [0x16] = {'u','U','\0'},
    [0x17] = {'i','I','\0'},
    [0x18] = {'o','O','\0'},
    [0x19] = {'p','P','\0'},

    [0x1A] = {'{','[','^'},     // AltGr+[ → ^
    [0x1B] = {'}','}','`'},     // unshifted }, shifted ], AltGr+ ] → `

    [0x1C] = {'\n','\n','\0'},  // Enter

    [0x1E] = {'a','A','\0'},
    [0x1F] = {'s','S','\0'},
    [0x20] = {'d','D','\0'},
    [0x21] = {'f','F','\0'},
    [0x22] = {'g','G','\0'},
    [0x23] = {'h','H','\0'},
    [0x24] = {'j','J','\0'},
    [0x25] = {'k','K','\0'},
    [0x26] = {'l','L','\0'},

    [0x27] = {'ñ','Ñ','\0'},     // semicolon key → ñ
    [0x28] = {'\'','?','\\'},    // apostrophe → '  and ?, AltGr → backslash
    [0x29] = {'¿','¡','\0'},     // backtick key → ¿  and ¡

    [0x2B] = {'\\','|','¬'},     // backslash key → \  and |, AltGr → ¬

    [0x2C] = {'z','Z','\0'},
    [0x2D] = {'x','X','\0'},
    [0x2E] = {'c','C','\0'},
    [0x2F] = {'v','V','\0'},
    [0x30] = {'b','B','\0'},
    [0x31] = {'n','N','\0'},
    [0x32] = {'m','M','\0'},
    [0x33] = {',',';' ,'\0'},    // comma key → ,  and ;
    [0x34] = {'.',':' ,'\0'},    // period  → .  and :
    [0x35] = {'/','?' ,'\0'},    // slash   → /  and ?

    [0x39] = {' ',' ' ,'\0'},    // Space
};


static char buffer[BUFFER_SIZE] = {0};
static int currentKey = 0;
static int nextToRead = 0;
int shiftPressed = 0;
int altGrPressed = 0;
int capsLock = 0;

void writeInBuffer(){
    unsigned int scanCode = getKeyPressed();
    uint8_t pressed = !(scanCode & 0x80);
    uint8_t makeCode = scanCode & 0x7F;
    switch(makeCode){
        case PS2_LSHIFT_MAKE:
        case PS2_RSHIFT_MAKE:
            shiftPressed = pressed ? 1 : 0;
            break;
        case PS2_ALTGR_MAKE:
            altGrPressed = pressed ? 1 : 0;
            break;
        case PS2_CAPSLOCK_MAKE:
            if(pressed){
                capsLock = !capsLock;
            }
            break;
        case PS2_ESC_MAKE:
            if(pressed){
                saveRegisters();
            }
            break;
        
    }

    if(pressed){
        int index;
        index = shiftPressed ? SHIFT_PRESSED_INDEX : 0;
        index = altGrPressed ? ALTGR_PRESSED_INDEX : index;
        if(isMappedChar(makeCode, index)){
            char c = ps2_make_keymap[makeCode][index];
            if(isAlpha(c)){
                if(capsLock){
                    index = (index == SHIFT_PRESSED_INDEX) ? 0 : ((index == 0) ? SHIFT_PRESSED_INDEX : index);
                    c = ps2_make_keymap[makeCode][index];
                }
            }
            buffer[currentKey++ % BUFFER_SIZE] = c;
        }
    }
    else{
        buffer[currentKey++ % BUFFER_SIZE] = 0;
    }
    currentKey = currentKey % BUFFER_SIZE;
}

unsigned char nextFromBuffer(){
    if(nextToRead == currentKey){
        return 0;
    }
    unsigned char ans = buffer[nextToRead];
    buffer[nextToRead] = 0;
    nextToRead = (nextToRead + 1) % BUFFER_SIZE;
    return ans;
}

uint8_t isMappedChar(uint8_t makeCode, int index){
    if(makeCode >= PS2_KEYMAP_SIZE){
        return 0;
    }
    return ps2_make_keymap[makeCode][index] != 0;
}

uint8_t isAlpha(char c){
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == 164 || c == 165;
}