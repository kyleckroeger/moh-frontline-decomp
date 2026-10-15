// A fragment of Pause.cpp (0x800ddebc): Pause::Reset clears the paused flag
// (+108) and empties the objective list, clearing every objective's
// completion flag, text and type. Pause is named by the mangled symbol and is
// a non-virtual view; the objective list view follows Pause.cpp, its clear
// helper is inferred. The rest of
// the file is in other units.
struct ObjectiveView {
    unsigned int text;
    unsigned char type;
    bool completed;
    unsigned char unknown06[2];
};

struct ObjectiveListView {
    /* inferred inline helper: clearing the list in one needs it for the register use */
    void Clear() {
        count = 0;
        for (int i = 0; i < 100; i++) {
            objectives[i].completed = false;
            objectives[i].text = 0;
            objectives[i].type = 0;
        }
    }

    int count;
    ObjectiveView objectives[100];
};

extern ObjectiveListView theObjectiveList;

class Pause {
public:
    void Reset();

    unsigned char unknown00[108];
    bool m_paused;
};

void Pause::Reset() {
    m_paused = false;
    theObjectiveList.Clear();
}
