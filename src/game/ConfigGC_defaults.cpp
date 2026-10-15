// A fragment of ConfigGC.cpp (0x800f7da4):
// CDualConfig::GetDefaultCustomControllerValues fills a custom controller
// layout with its 13 default button assignments. The class and function are
// named by the mangled symbol; what each value means is not known. The rest
// of the file is in other units.
class CDualConfig {
public:
    void GetDefaultCustomControllerValues(char*);
};

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
