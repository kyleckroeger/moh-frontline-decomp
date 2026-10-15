// A fragment of jaw_controller.cpp (0x8009c6a8): the CJawController
// constructor, which clears its values (floats and words) and sets the word at
// +36 to -1 and the one at +56 to 1. The class is named by the mangled symbol;
// the members and their meaning are inferred from offsets. The rest of the
// file is not part of this unit.
class CJawController {
public:
    CJawController();

    float m_value00;
    float m_value04;
    float m_value08;
    float m_value0c;
    float m_value10;
    int m_value14;
    int m_value18;
    float m_value1c;
    float m_value20;
    int m_value24;
    int m_value28;
    float m_value2c;
    float m_value30;
    float m_value34;
    int m_value38;
    int m_value3c;
};

CJawController::CJawController()
    : m_value00(0.0f), m_value04(0.0f), m_value08(0.0f), m_value0c(0.0f), m_value10(0.0f), m_value14(0),
      m_value18(0), m_value1c(0.0f), m_value20(0.0f), m_value24(-1), m_value28(0), m_value2c(0.0f),
      m_value30(0.0f), m_value34(0.0f), m_value38(1), m_value3c(0) {
}
