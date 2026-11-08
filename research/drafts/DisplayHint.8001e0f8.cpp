struct StringTableEntryView {
    const char* text;
    int unknown04;
};

struct StringTableView {
    StringTableEntryView* entries;
    unsigned char unknown04[8];
    int count;
};

struct HintTextView {
    const char* text[2];
    int next;
};

struct AEMSEventView {
    int values[10];
};

extern StringTableView umTable;
extern HintTextView g_HintTextArray[20];
extern int g_iHighestObjectiveNumber;

bool GetObjectiveStatus(unsigned int);
void DisplayHintText(const char*);
void AEMS_SendEvent(void*);

inline const char* GetString(int index) {
    if (umTable.count > index)
        return umTable.entries[index].text;
    return 0;
}

static inline HintTextView* GetHint(int objective) {
    return &g_HintTextArray[objective - 1];
}

bool DisplayHint() {
    const char** texts;
    int* nextIndex;
    HintTextView* hint;
    int objective;
    int next;
    if (g_iHighestObjectiveNumber == 0) {
        DisplayHintText(GetString(729));
        return true;
    }
    for (objective = 1; objective <= g_iHighestObjectiveNumber; objective++) {
        if (!GetObjectiveStatus(objective))
            break;
    }
    if (objective > g_iHighestObjectiveNumber)
        objective = 20;
    hint = GetHint(objective);
    texts = hint->text;
    nextIndex = &hint->next;
    next = hint->next;
    if (texts[next]) {
        DisplayHintText(texts[next]);
        do {
            next++;
            if (next >= 2)
                next -= 2;
        } while (!texts[next]);
        *nextIndex = next;
    } else {
        DisplayHintText(GetString(729));
    }
    AEMSEventView event = {7, 3, 0, 0x1000, 0, 0, 2500, 0, 0, 0x7fff};
    AEMS_SendEvent(&event);
    return true;
}
