// A fragment of AISplinePath.cpp (0x80050ae4): the CAISplinePathTraversal constructor clears the words at +0 and +16 (the
// members are inferred). The classes
// and functions are named by the symbols. The rest of the file is in other
// units.
class CAISplinePath;

class CAISplinePathTraversal {
public:
    CAISplinePathTraversal();

    int m_index;
    unsigned char unknown04[12];
    CAISplinePath* m_path;
};

CAISplinePathTraversal::CAISplinePathTraversal() {
    m_index = 0;
    m_path = 0;
}
