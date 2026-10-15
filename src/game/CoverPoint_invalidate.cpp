// A fragment of CoverPoint.cpp (0x800526b0): CCoverPoint::Invalidate clears
// the eight flag bits of the cover point data's byte at +112 and fires script
// event 62 on every AI object (from the global list, next at +4) whose cover
// point (+656) is this one. CCoverPoint, CAIObject, BSObject and the
// functions are named by the symbols; the data view, the flag order (the
// bits are cleared in the order 0x08, 0x40, 0x80, 0x01, 0x02, 0x04, 0x10,
// 0x20; their names are not known) and the object views are inferred.
struct BSObject;

void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

/* inferred: the cover point data's flag byte at +112 */
struct CoverPointDataView {
    unsigned char unknown00[112];
    unsigned char m_flag80 : 1;
    unsigned char m_flag40 : 1;
    unsigned char m_flag20 : 1;
    unsigned char m_flag10 : 1;
    unsigned char m_flag08 : 1;
    unsigned char m_flag04 : 1;
    unsigned char m_flag02 : 1;
    unsigned char m_flag01 : 1;
};

class CCoverPoint {
public:
    void Invalidate();

    unsigned char unknown00[4];
    CoverPointDataView* m_data;
};

struct AIFilterScriptView {
    unsigned char unknown00[12];
    BSObject* script;
};

class CAIObject {
public:
    static CAIObject* GetGlobalListStart();

    unsigned char unknown000[4];
    CAIObject* m_next;
    AIFilterScriptView* m_filter;
    unsigned char unknown00c[644];
    CCoverPoint* m_coverPoint;
};

void CCoverPoint::Invalidate() {
    CoverPointDataView* data = m_data;
    data->m_flag08 = 0;
    data->m_flag40 = 0;
    data->m_flag80 = 0;
    data->m_flag01 = 0;
    data->m_flag02 = 0;
    data->m_flag04 = 0;
    data->m_flag10 = 0;
    data->m_flag20 = 0;
    for (CAIObject* object = CAIObject::GetGlobalListStart(); object; object = object->m_next) {
        if (object->m_coverPoint == this)
            BSObjectTriggerEvent(object->m_filter->script, 62, 0, 0, true);
    }
}
