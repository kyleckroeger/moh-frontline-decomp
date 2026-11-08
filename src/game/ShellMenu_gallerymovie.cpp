// ShellMenu.cpp: CShellMenu::PlayGalleryMovie. Class layouts are inferred views of what this function
// uses. ShellMenu_block.h recreates the constant block its offsets depend on.
#include "ShellMenu_block.h"

extern "C" int sprintf(char*, const char*, ...);
class CShellMenu {
public:
    void PlayGalleryMovie(int, int, int, int);
    void PlayCreditsSlideShow();
};
void CShellMenu::PlayGalleryMovie(int index, int type, int credits, int extras) {
    char name[64];
    if (credits) {
        PlayCreditsSlideShow();
    } else if (extras) {
        sprintf(name, "data\\Movies\\extras%d.mpc", extras);
    } else {
        switch (type) {
        case 0:
            sprintf(name, "data\\Movies\\history%d.mpc", index);
            break;
        case 1:
            sprintf(name, "data\\Movies\\making%d.mpc", index);
            break;
        }
    }
}
