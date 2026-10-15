// A fragment of jaw_controller.cpp (0x8009c13c): CJawController's
// SetMotionType, which stores the jaw type and sets the motion's timing and
// amplitude values for it (types 1 to 7, others reset to a long idle; some
// types take a random count), keeping the previous phase and starting a new
// one, and Update, which advances the motion by the elapsed ticks (at 59.94
// per second), starts the next phase or idles when one ends, and turns the
// phase position into the jaw value (a quarter sine wave scaled between the
// values at +0x1c and +0x20). The class, functions and EJawType are named by
// the mangled symbols; the members and their meaning are inferred from
// offsets (as in
// jaw_controller.cpp), and the case order follows the code layout. The
// constants are entries of the file's .sdata2 pool and the switch table is
// in .data. The rest of the file is in other units.
unsigned int MathFunRandomUI32Raw();
extern "C" double sin(double);

enum EJawType {};

class CJawController {
public:
    void SetMotionType(EJawType);
    void Update(int);

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

void CJawController::SetMotionType(EJawType type) {
    m_value3c = type;
    switch (m_value3c) {
    case 5:
        m_value08 = 0.4f;
        m_value0c = 1.0f;
        m_value10 = 5.0f;
        m_value1c = 0.0f;
        m_value20 = 1.0f;
        m_value18 = 10;
        m_value24 = m_value28;
        m_value28 = 6;
        m_value14 = m_value18;
        m_value2c = m_value34 = 2.0f;
        m_value38 = 1;
        break;
    case 6:
        m_value08 = 6.0f;
        m_value0c = 8.0f;
        m_value10 = 4.0f;
        m_value1c = 0.0f;
        m_value20 = 0.4f;
        m_value18 = MathFunRandomUI32Raw() % 5 + 5;
        m_value24 = m_value28;
        m_value28 = 1;
        m_value14 = m_value18;
        m_value2c = m_value34 = 2.0f;
        m_value38 = 1;
        break;
    case 2:
        m_value08 = 0.6f;
        m_value0c = 0.4f;
        m_value10 = 0.2f;
        m_value1c = 0.1f;
        m_value20 = 0.5f;
        m_value18 = MathFunRandomUI32Raw() % 10 + 5;
        m_value24 = m_value28;
        m_value28 = 1;
        m_value14 = m_value18;
        m_value2c = m_value34 = 2.0f;
        m_value38 = 1;
        break;
    case 3:
        m_value08 = 0.4f;
        m_value0c = 0.4f;
        m_value10 = 0.1f;
        m_value1c = 0.15f;
        m_value20 = 0.5f;
        m_value18 = MathFunRandomUI32Raw() % 10 + 5;
        m_value24 = m_value28;
        m_value28 = 9;
        m_value14 = m_value18;
        m_value2c = m_value34 = 1.0f;
        m_value38 = 1;
        break;
    case 7:
        m_value08 = 0.4f;
        m_value0c = 0.4f;
        m_value10 = 0.1f;
        m_value1c = 0.15f;
        m_value20 = 0.5f;
        m_value18 = 10;
        m_value24 = m_value28;
        m_value28 = 9;
        m_value14 = m_value18;
        m_value2c = m_value34 = 0.1f;
        m_value38 = 1;
        break;
    case 4:
        m_value08 = 0.2f;
        m_value0c = 0.5f;
        m_value10 = 0.0f;
        m_value1c = 0.2f;
        m_value20 = 0.8f;
        m_value18 = 6;
        m_value24 = m_value28;
        m_value28 = 9;
        m_value14 = m_value18;
        m_value2c = m_value34 = 1.5f;
        m_value38 = 1;
        break;
    case 1:
        m_value08 = 0.15f;
        m_value0c = 5.0f;
        m_value10 = 0.0f;
        m_value1c = 0.0f;
        m_value20 = 0.7f;
        m_value18 = 1;
        m_value24 = m_value28;
        m_value28 = 9;
        m_value14 = m_value18;
        m_value2c = m_value34 = 0.0f;
        m_value38 = 1;
        break;
    default:
        m_value08 = 0.0f;
        m_value0c = 86400.0f;
        m_value10 = 0.0f;
        m_value1c = 0.0f;
        m_value20 = 0.0f;
        m_value18 = 1;
        m_value24 = m_value28;
        m_value28 = 9;
        m_value14 = m_value18;
        m_value2c = m_value34 = 0.0f;
        m_value38 = 0;
        break;
    }
}

void CJawController::Update(int ticks) {
    m_value30 = 0.0f;
    if (m_value2c <= 0.0f && m_value18 == 0)
        return;
    m_value2c -= (float)ticks / 59.94f;
    if (m_value38) {
        if (m_value2c <= 0.0f)
            m_value38 = 0;
    } else if (m_value2c <= 0.0f) {
        if (m_value14) {
            m_value00 = m_value0c;
            m_value04 = m_value00 + m_value08;
            m_value2c = m_value04 + m_value10;
            m_value14--;
        } else if (m_value3c == 4) {
            m_value00 = 86400.0f;
            m_value04 = 0.5f + m_value00;
            m_value2c = 0.5f + m_value04;
            m_value18 = 1;
            m_value14 = 1;
            m_value20 = 0.5f;
        } else if (m_value3c == 1) {
            m_value2c = 86400.0f;
            m_value38 = 1;
        } else {
            m_value14 = m_value18;
            m_value2c = m_value34;
            m_value38 = 1;
        }
    } else if (m_value2c > m_value00 && m_value2c < m_value04) {
        m_value30 = 1.0f - (m_value2c - m_value00) / (m_value04 - m_value00);
    } else if (m_value2c <= m_value00) {
        m_value30 = m_value2c / m_value00;
    }
    m_value30 *= (float)sin(0.017453292f * (90.0f * m_value30));
    m_value30 = m_value30 * (m_value20 - m_value1c) + m_value1c;
}
