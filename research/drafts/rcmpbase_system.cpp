// rcmpbase.cpp: the movie player's global system object, as partial source.
namespace RCMP {
class RCMP_SYSTEM {
public:
    RCMP_SYSTEM() : m_alloc(0), m_free(0) {}
    virtual ~RCMP_SYSTEM() {}
    void* m_alloc;
    void* m_free;
};
RCMP_SYSTEM rcmp_sys;
}
