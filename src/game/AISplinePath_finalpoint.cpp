// A fragment of AISplinePath.cpp (0x8004fcbc):
// CAISplinePathTraversal::GetFinalPointForward expands the path's segment
// at the index at +4 of the path (+16) at t = 0 into the output position.
// GetNextPointForward advances the traversal: at the end of the path (states
// 5 and 6) it waits until the final point is reached (the squared distance
// stops shrinking by half, an inferred inline), then triggers script event
// 55, stops the AI walk and finishes (state 7); otherwise, once the current
// point is reached (always outside states 1 and 2), it moves the curve
// parameter to the closest point and, past the segment's end, either starts
// the final approach (5, or 6 for the A* states) or steps to the next
// segment (event 161), else expands the next point (event 54 and states 3/4
// on the first step) and modifies it. In state 2 the AI object's A* walk is
// updated, otherwise the point is returned.
// The classes and functions are named by the symbols; the segment view and
// Expand's body follow AISplinePath_segment.cpp. Expand is defined later in
// the original file but inlined here (the file's inlining is deferred);
// this unit marks it inline so that it is inlined without being emitted.
// The path view, the output's CVector3 view of the position and the index's
// meaning are inferred. The rest of the file is in other units.
class CVector3 {
public:
    void operator+=(const CVector3& o) {
        x += o.x;
        y += o.y;
        z += o.z;
    }
    void operator*=(float s) {
        x *= s;
        y *= s;
        z *= s;
    }

    float x;
    float y;
    float z;
    float w;
};

class CAIFilterRealPosition {
public:
    float GetDistanceSquaredXYReal(const CAIFilterRealPosition&) const;

    float x;
    float y;
    float z;
};

class CAIFilterRealVector3;

struct BSObject;
void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

class CAIObject {
public:
    void UpdateAStarPathWalk();

    unsigned char unknown000[536];
    int m_walkState;
};

/* inferred: the filter object's AI object (+8) and script object (+12) */
class CAIFilterObject {
public:
    unsigned char unknown00[8];
    CAIObject* m_ai;
    BSObject* m_script;
};

class CAISplinePathSegment {
public:
    void Expand(float, CVector3&);
    float GetClosestParameter(float, const CAIFilterRealVector3&);

    unsigned char unknown00[8];
    CVector3 m_coefficients[4];
};

inline void CAISplinePathSegment::Expand(float t, CVector3& out) {
    out.x = t * m_coefficients[0].x;
    out.y = t * m_coefficients[0].y;
    out.z = t * m_coefficients[0].z;
    out += m_coefficients[1];
    out *= t;
    out += m_coefficients[2];
    out *= t;
    out += m_coefficients[3];
}

/* inferred: the path's segments and the final segment's index */
class CAISplinePath {
public:
    CAISplinePathSegment* m_segments;
    int m_last;
};

class CAISplinePathTraversal {
public:
    void GetFinalPointForward(CAIFilterRealPosition*);
    void GetNextPointForward(const CAIFilterRealPosition&, CAIFilterRealPosition*, CAIFilterObject*);
    void SetupNextSegmentForward(const CAIFilterRealPosition&);
    void MakeModifiedNextPoint();

    /* inferred: the point is reached once the distance stops shrinking fast */
    bool IsPointReached(const CAIFilterRealPosition& position) {
        float distance = ((CAIFilterRealPosition&)m_point).GetDistanceSquaredXYReal(position);
        if (2.0f * (m_distance - distance) >= distance)
            return true;
        m_distance = distance;
        return false;
    }

    int m_state;
    float m_t;
    float m_distance;
    unsigned int m_index;
    CAISplinePath* m_path;
    CAISplinePathSegment* m_segment;
    CVector3 m_point;
};

void CAISplinePathTraversal::GetFinalPointForward(CAIFilterRealPosition* point) {
    m_path->m_segments[m_path->m_last].Expand(0.0f, *(CVector3*)point);
}

void CAISplinePathTraversal::GetNextPointForward(const CAIFilterRealPosition& position, CAIFilterRealPosition* out,
                                                 CAIFilterObject* filter) {
    bool reached = true;
    if (m_state == 5 || m_state == 6) {
        GetFinalPointForward((CAIFilterRealPosition*)&m_point);
        if (IsPointReached(position)) {
            if (filter) {
                BSObjectTriggerEvent(filter->m_script, 55, 0, 0, false);
                filter->m_ai->m_walkState = 0;
            }
            m_state = 7;
        }
    } else {
        if (m_state == 1 || m_state == 2)
            reached = IsPointReached(position);
        if (reached) {
            float t = m_segment->GetClosestParameter(m_t, (const CAIFilterRealVector3&)position);
            if (t >= 0.0f)
                m_t = t;
            if (m_t >= 1.0f) {
                if (m_index == m_path->m_last - 1) {
                    m_distance = 0.0f;
                    if (m_state == 2 || m_state == 4)
                        m_state = 6;
                    else
                        m_state = 5;
                    GetFinalPointForward((CAIFilterRealPosition*)&m_point);
                } else {
                    if (filter)
                        BSObjectTriggerEvent(filter->m_script, 161, 0, 0, false);
                    m_index++;
                    m_segment++;
                    SetupNextSegmentForward(position);
                }
            } else {
                if (m_state == 1) {
                    if (filter)
                        BSObjectTriggerEvent(filter->m_script, 54, 0, 0, false);
                    m_state = 3;
                } else if (m_state == 2) {
                    if (filter)
                        BSObjectTriggerEvent(filter->m_script, 54, 0, 0, false);
                    m_state = 4;
                }
                m_segment->Expand(m_t, m_point);
                MakeModifiedNextPoint();
            }
        }
    }
    if (m_state == 2)
        filter->m_ai->UpdateAStarPathWalk();
    else {
        float y;
        float z = m_point.z;
        y = m_point.y;
        float x = m_point.x;
        out->x = x;
        out->y = y;
        out->z = z;
    }
}
