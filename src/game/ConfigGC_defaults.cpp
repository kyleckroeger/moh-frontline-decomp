// A fragment of ConfigGC.cpp (0x800f7b58): CDualConfig::CopyDefaultConfig
// writes one of the five default button layouts into the current config text
// (as SetUpDefaultConfig does; other values leave it) and copies the text,
// terminator included, into the buffer.
// CDualConfig::GetDefaultCustomControllerValues fills a custom controller
// layout with its 13 default button assignments. The class and function are
// named by the mangled symbol; what each value means is not known. The rest
// of the file is in other units.
extern "C" void* memcpy(void*, const void*, unsigned long);

extern char currConfigText[14];

class CDualConfig {
public:
    void CopyDefaultConfig(int, char*);
    void GetDefaultCustomControllerValues(char*);
};

void CDualConfig::CopyDefaultConfig(int config, char* buttons) {
    switch (config) {
    case 1:
        currConfigText[0] = 1;
        currConfigText[1] = 3;
        currConfigText[2] = 2;
        currConfigText[3] = 0;
        currConfigText[4] = 0;
        currConfigText[5] = 1;
        currConfigText[6] = 4;
        currConfigText[7] = 5;
        currConfigText[8] = 6;
        currConfigText[9] = 9;
        currConfigText[10] = 8;
        currConfigText[11] = 3;
        currConfigText[12] = 2;
        break;
    case 2:
        currConfigText[0] = 1;
        currConfigText[1] = 3;
        currConfigText[2] = 0;
        currConfigText[3] = 4;
        currConfigText[4] = 0;
        currConfigText[5] = 9;
        currConfigText[6] = 6;
        currConfigText[7] = 2;
        currConfigText[8] = 3;
        currConfigText[9] = 8;
        currConfigText[10] = 7;
        currConfigText[11] = 1;
        currConfigText[12] = 3;
        break;
    case 3:
        currConfigText[0] = 1;
        currConfigText[1] = 3;
        currConfigText[2] = 2;
        currConfigText[3] = 0;
        currConfigText[4] = 0;
        currConfigText[5] = 1;
        currConfigText[6] = 4;
        currConfigText[7] = 5;
        currConfigText[8] = 6;
        currConfigText[9] = 9;
        currConfigText[10] = 8;
        currConfigText[11] = 2;
        currConfigText[12] = 3;
        break;
    case 4:
        currConfigText[0] = 1;
        currConfigText[1] = 3;
        currConfigText[2] = 2;
        currConfigText[3] = 0;
        currConfigText[4] = 0;
        currConfigText[5] = 1;
        currConfigText[6] = 4;
        currConfigText[7] = 5;
        currConfigText[8] = 2;
        currConfigText[9] = 9;
        currConfigText[10] = 8;
        currConfigText[11] = 6;
        currConfigText[12] = 3;
        break;
    case 5:
        currConfigText[0] = 1;
        currConfigText[1] = 3;
        currConfigText[2] = 0;
        currConfigText[3] = 2;
        currConfigText[4] = 0;
        currConfigText[5] = 1;
        currConfigText[6] = 4;
        currConfigText[7] = 5;
        currConfigText[8] = 6;
        currConfigText[9] = 9;
        currConfigText[10] = 8;
        currConfigText[11] = 3;
        currConfigText[12] = 2;
        break;
    }
    memcpy(buttons, currConfigText, 14);
}


void CDualConfig::GetDefaultCustomControllerValues(char* values) {
    values[0] = 1;
    values[1] = 3;
    values[2] = 2;
    values[3] = 0;
    values[4] = 0;
    values[5] = 1;
    values[6] = 4;
    values[7] = 5;
    values[8] = 6;
    values[9] = 9;
    values[10] = 8;
    values[11] = 3;
    values[12] = 2;
}
