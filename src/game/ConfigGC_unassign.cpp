// A fragment of ConfigGC.cpp (0x800f9f30): the file's initialised data (the
// axis, digital and analog button name tables, the default, current and
// visual configuration texts, the button types, the per-button
// configuration choices, the hotspot positions and the hotbox tables, in
// the original's order and with its values), then
// CDualConfig::WillBeUnassigned, which finds the button whose assignment
// the selected button's new value would take over (-1 when none), and
// CDualConfig::UnassignPreviousAssignedButton, which marks that button
// unassigned (7, or 5 for the analog triggers) in both texts and then
// clears the trigger buttons that conflict with the selected one. The data
// and the functions are named by the symbols; the data's element types,
// the selected-button member (+20) and the comparison forms (a loop index
// compared as a char, a type compared with the selected button's type) are
// inferred. The code addresses the file's data from one base, so this unit
// defines all of it; the strings are the original's pooled ones. The rest of
// the file is in other units.
char* axisText[15] = {
    "Left X Axis",
    "Left Y Axis",
    "Right X Axis",
    "Right Y Axis",
    "L / R Axis",
    "Unassigned",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
};

char* digitalText[15] = {
    "Y",
    "X",
    "A",
    "B",
    "L",
    "R",
    "Z",
    "Unassigned",
    "DPad Up",
    "DPad Down",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
};

char* analogText[15] = {
    "L",
    "R",
    "Unassigned",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
};

char defaultConfigText[14] = { 1, 3, 2, 0, 0, 1, 4, 5, 6, 9, 8, 3, 2, 0 };

char currConfigText[14] = { 1, 3, 2, 0, 0, 1, 4, 5, 6, 9, 8, 3, 2, 0 };

char visualConfigText[14] = { 1, 3, 2, 0, 0, 1, 4, 5, 6, 9, 8, 3, 2, 0 };

char buttonType[14] = { 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

char configArray[14][15] = {
    { 0, 1, 2, 3, 9, 123, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 1, 2, 3, 9, 123, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 1, 2, 3, 9, 123, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 1, 2, 3, 9, 123, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 11, 10, 8, 9, 6, 5, 4, 123, 3, 2, -1, -1, -1, -1, -1 },
    { 11, 10, 8, 9, 6, 5, 4, 123, 3, 2, -1, -1, -1, -1, -1 },
    { 11, 10, 8, 9, 6, 5, 4, 123, -1, -1, -1, -1, -1, -1, -1 },
    { 11, 10, 8, 9, 6, 5, 4, 123, 3, 2, -1, -1, -1, -1, -1 },
    { 11, 10, 8, 9, 6, 5, 4, 123, 3, 2, -1, -1, -1, -1, -1 },
    { 11, 10, 8, 9, 6, 5, 4, 123, 3, 2, -1, -1, -1, -1, -1 },
    { 11, 10, 8, 9, 6, 5, 4, 123, 3, 2, -1, -1, -1, -1, -1 },
    { 11, 10, 8, 9, 6, 5, 4, 123, 3, 2, -1, -1, -1, -1, -1 },
    { 11, 10, 8, 9, 6, 5, 4, 123, 3, 2, -1, -1, -1, -1, -1 },
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
};

short hotSpotXLoc[18] = { 150, 206, 150, 94, 150, 150, 500, 555, 500, 444, 538, 538, 538, 505, 208, 430, 538, 320 };

short hotSpotYLoc[18] = { 160, 190, 220, 190, 280, 305, 310, 340, 370, 340, 180, 213, 246, 280, 100, 100, 148, 302 };

char AxisPlusHotBoxLoc[15] = { 1, 0, 7, 6, 14, 17, 123, 123, 123, 123, 123, 123, 123, 123, 123 };

char AxisMinusHotBoxLoc[15] = { 3, 2, 9, 8, 15, 17, 123, 123, 123, 123, 123, 123, 123, 123, 123 };

char ButtonHotBoxLoc[15] = { 10, 11, 12, 13, 14, 15, 16, 17, 4, 5, 123, 123, 123, 123, 123 };


class CDualConfig {
public:
    int WillBeUnassigned();
    void UnassignPreviousAssignedButton();

    unsigned char unknown00[20];
    int m_selected;
};

int CDualConfig::WillBeUnassigned() {
    int result = -1;
    for (int i = 0; (char)i < 13; i++) {
        if ((char)i == m_selected)
            continue;
        if (buttonType[i] == 0 && visualConfigText[m_selected] == 7)
            continue;
        if (buttonType[i] == 1 && visualConfigText[m_selected] == 7)
            continue;
        if (buttonType[i] == 2 && visualConfigText[m_selected] == 5)
            continue;
        if (currConfigText[i] != visualConfigText[m_selected])
            continue;
        if (buttonType[i] == 0 && buttonType[i] == buttonType[m_selected])
            result = i;
        else if (buttonType[i] == 2 && buttonType[i] == buttonType[m_selected])
            result = i;
        else if (buttonType[i] == 1 && buttonType[i] == buttonType[m_selected])
            result = i;
        else if (buttonType[i] == 1 && buttonType[m_selected] == 0)
            result = i;
        else if (buttonType[i] == 0 && buttonType[m_selected] == 1)
            result = i;
    }
    return result;
}

void CDualConfig::UnassignPreviousAssignedButton() {
    for (char i = 0; i < 13; i++) {
        if (i != m_selected && currConfigText[i] == currConfigText[m_selected]) {
            if (buttonType[i] == 0 && buttonType[i] == buttonType[m_selected]) {
                visualConfigText[i] = 7;
                currConfigText[i] = 7;
            } else if (buttonType[i] == 2 && buttonType[i] == buttonType[m_selected]) {
                visualConfigText[i] = 5;
                currConfigText[i] = 5;
            } else if (buttonType[i] == 1 && buttonType[i] == buttonType[m_selected]) {
                visualConfigText[i] = 7;
                currConfigText[i] = 7;
            } else if (buttonType[i] == 1 && buttonType[m_selected] == 0) {
                visualConfigText[i] = 7;
                currConfigText[i] = 7;
            } else if (buttonType[i] == 0 && buttonType[m_selected] == 1) {
                visualConfigText[i] = 7;
                currConfigText[i] = 7;
            }
        }
    }
    switch (buttonType[m_selected]) {
    case 2:
        if (currConfigText[m_selected] == 4) {
            int k;
            int value = 4;
            for (k = 0; k < 2; k++, value++) {
                char* type = buttonType;
                char* curr = currConfigText;
                char* visual = visualConfigText;
                for (int j = 0; j < 13; j++, type++, curr++, visual++) {
                    if (*type != 2 && *curr == value) {
                        *visual = 7;
                        *curr = 7;
                    }
                }
            }
        }
        return;
    case 0:
    case 1:
        if (currConfigText[m_selected] == 4 || currConfigText[m_selected] == 5) {
            for (int j = 0; j < 13; j++) {
                if (buttonType[j] == 2 && currConfigText[j] == 4) {
                    visualConfigText[j] = 5;
                    currConfigText[j] = 5;
                }
            }
        }
        break;
    }
}
