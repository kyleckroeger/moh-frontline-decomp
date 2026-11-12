// world_object.cpp: CWorldObject's two filename hooks (.sdata), which name a compartment's
// .cpt and .cdb files; both start at the big-file versions. CWorldObject is a view holding
// only these static members. Checked by research/tools/check_data.py.

void GetCptFilenameFromBigFile(int, char*, int);
void GetCdbFilenameFromBigFile(int, char*, int);

class CWorldObject {
public:
    static void (*GetCptFilename)(int, char*, int);
    static void (*GetCdbFilename)(int, char*, int);
};

void (*CWorldObject::GetCptFilename)(int, char*, int) = GetCptFilenameFromBigFile;
void (*CWorldObject::GetCdbFilename)(int, char*, int) = GetCdbFilenameFromBigFile;
