// A fragment of world_object.cpp (0x800b6d90): CWorldObject::ServiceDataStreamer
// runs the compartment load requests until one has to wait: a request takes a
// buffer from the pool or reuses the buffer of a loaded compartment (which is
// taken out of the world volume and then freed), reads the compartment's CPT
// file and then its CDB file asynchronously in chunks of 524288 bytes per
// unit of time (128- and 16-byte aligned, adding to the load progress), then
// builds the compartment at the end of the list (an inline push), adds it to
// the world volume and retires the request. The request and list views, the
// state numbers and the filename callbacks' arguments are inferred.
// CWorldObject::HandleExplosion
// passes the explosion to each of the world object's compartments, and
// GetCompartment finds the compartment with the given index (its word at
// +0x50, the texture-swap set of compartment_texswap.cpp), or none. The
// classes and functions are named by the mangled symbols; the compartment
// list view follows world_object_draw.cpp (the compartment is a 104-byte
// view here). The rest of the file is not part of this unit.
class CExplosion;
class CCDBObject;
class CPTHeader;
class CDB;

inline void* operator new(unsigned long, void* place) {
    return place;
}

extern "C" {
int FILESYS_opensync(const char*, int, int, int*);
int FILESYS_closesync(int, int);
int ASYNCFILE_read(int, int, void*, int);
int ASYNCFILE_getstatus(int);
int ASYNCFILE_release(int, int*, int*);
}

class CCompartment {
public:
    CCompartment();
    CCompartment(const CCompartment&);
    ~CCompartment();
    void HandleExplosion(const CExplosion&);
    void Init(int, char*, CPTHeader*, CDB*);

    virtual void unknown08();
    virtual void unknown0c();
    virtual void unknown10();
    virtual void unknown14();
    virtual void unknown18();
    virtual void unknown1c();
    virtual void unknown20();
    virtual void unknown24();
    virtual void unknown28();
    virtual void unknown2c();
    virtual void unknown30();
    virtual CCDBObject* GetCDBObject(int);

    unsigned char unknown04[76];
    int m_index;
    char* m_buffer;
    unsigned char unknown58[8];
    bool m_active;
    bool m_loaded;
    unsigned char unknown62[6];
} __attribute__((aligned(8)));

class CWorldVolume {
public:
    void AddSubVolume(const CCDBObject&);
    void RemoveSubVolume(const CCDBObject&);

    unsigned char unknown00[88];
};

/* inferred: a pending compartment load */
struct WorldRequestView {
    WorldRequestView* prev;
    WorldRequestView* next;
    int id;
    int state;
    int file;
    int read;
    bool reuse;
    unsigned char unknown19[3];
    char* buffer;
    char* cursor;
    int offset;
    CPTHeader* cpt;
    CDB* cdb;
};

struct WorldCompartmentNodeView {
    WorldCompartmentNodeView(WorldCompartmentNodeView* p, WorldCompartmentNodeView* n, const CCompartment& c)
        : prev(p), next(n), compartment(c) {}

    WorldCompartmentNodeView* prev;
    WorldCompartmentNodeView* next;
    CCompartment compartment;
};

struct WorldListLinkView {
    WorldCompartmentNodeView* prev;
    WorldCompartmentNodeView* next;
};

struct WorldRequestLinkView {
    WorldRequestView* prev;
    WorldRequestView* next;
};

/* inferred: the size word of a loaded file header (+4) */
struct LoadedHeaderView {
    unsigned char unknown00[4];
    int size;
};

class CWorldObject {
public:
    void ServiceDataStreamer(float);
    void PushCompartment(WorldCompartmentNodeView* pos, const CCompartment& value) {
        WorldCompartmentNodeView* prev = pos->prev;
        WorldCompartmentNodeView* node = m_compartmentFree;
        m_compartmentFree = node->next;
        WorldCompartmentNodeView* added = new (node) WorldCompartmentNodeView(0, 0, value);
        m_compartmentCount++;
        prev->next = added;
        added->prev = prev;
        added->next = pos;
        pos->prev = added;
    }
    WorldCompartmentNodeView* AllocCompartmentNode() {
        WorldCompartmentNodeView* node = m_compartmentFree;
        m_compartmentFree = node->next;
        return node;
    }
    void HandleExplosion(const CExplosion&);
    CCompartment* GetCompartment(int);
    static void (*GetCptFilename)(int, char*, int);
    static void (*GetCdbFilename)(int, char*, int);

    unsigned char unknown000[40];
    char** m_bufferFree;
    unsigned char unknown02c[4];
    int m_bufferCount;
    int m_buffersUsed;
    unsigned char unknown038[8];
    int m_compartmentCount;
    unsigned char unknown044[8];
    WorldCompartmentNodeView* m_compartmentFree;
    WorldListLinkView m_compartments;
    unsigned char unknown058[112];
    int m_requestCount;
    unsigned char unknown0cc[8];
    WorldRequestView* m_requestFree;
    WorldRequestLinkView m_requests;
    unsigned char unknown0e0[48];
    CWorldVolume m_world;
    float m_progress;
};

void CWorldObject::ServiceDataStreamer(float dt) {
    WorldRequestView* request;
    WorldCompartmentNodeView* node;
    WorldCompartmentNodeView* end;
    int chunk;
    bool done;
    char cptName[128];
    char cdbName[128];

    if (dt < 1.0f)
        dt = 1.0f;
    end = (WorldCompartmentNodeView*)&m_compartments;
    done = false;
    chunk = (int)(524288.0f * dt);
    while (!done && m_requestCount != 0) {
        request = m_requests.next;
        switch (request->state) {
        case 0:
            if (m_buffersUsed < m_bufferCount) {
                char** buffer = m_bufferFree;
                m_bufferFree = (char**)*buffer;
                m_buffersUsed++;
                request->buffer = (char*)buffer;
            } else {
                WorldCompartmentNodeView* node;
                for (node = m_compartments.next; node != end; node = node->next) {
                    if (node->compartment.m_loaded)
                        break;
                }
                CCompartment* compartment = &node->compartment;
                request->buffer = compartment->m_buffer;
                request->reuse = true;
                compartment->m_active = false;
                CCDBObject* object = compartment->GetCDBObject(0);
                m_world.RemoveSubVolume(*object);
                done = true;
            }
            request->cursor = request->buffer;
            request->state = 1;
            break;
        case 1:
            if (request->reuse) {
                for (node = m_compartments.next; node != end; node = node->next) {
                    if (node->compartment.m_buffer == request->buffer) {
                        WorldCompartmentNodeView* next;
                        WorldCompartmentNodeView* prev = node->prev;
                        next = node->next;
                        prev->next = next;
                        next->prev = prev;
                        if (node)
                            node->compartment.~CCompartment();
                        node->prev = 0;
                        node->next = m_compartmentFree;
                        m_compartmentFree = node;
                        m_compartmentCount--;
                        break;
                    }
                }
            }
            request->state = 2;
            break;
        case 2: {
            request->cursor = (char*)(((int)request->cursor + 127) & ~127);
            request->offset = 0;
            request->cpt = (CPTHeader*)request->cursor;
            request->state = 3;
            GetCptFilename(request->id, cptName, 128);
            FILESYS_opensync(cptName, 33, 100, &request->file);
            break;
        }
        case 3:
            request->read = ASYNCFILE_read(request->file, request->offset, request->cursor, chunk);
            request->state = 4;
            done = true;
            break;
        case 4:
            if (ASYNCFILE_getstatus(request->read) == 1) {
                int got = 0;
                ASYNCFILE_release(request->read, 0, &got);
                request->cursor += got;
                request->offset += got;
                m_progress += (float)got / (float)((LoadedHeaderView*)request->cpt)->size;
                if (request->cursor - (char*)request->cpt < ((LoadedHeaderView*)request->cpt)->size) {
                    request->state = 3;
                } else {
                    FILESYS_closesync(request->file, 100);
                    request->state = 5;
                }
            } else {
                done = true;
            }
            break;
        case 5: {
            request->cursor = (char*)(((int)request->cursor + 15) & ~15);
            request->offset = 0;
            request->cdb = (CDB*)request->cursor;
            request->state = 6;
            GetCdbFilename(request->id, cdbName, 128);
            FILESYS_opensync(cdbName, 33, 100, &request->file);
            break;
        }
        case 6:
            request->read = ASYNCFILE_read(request->file, request->offset, request->cursor, chunk);
            request->state = 7;
            done = true;
            break;
        case 7:
            if (ASYNCFILE_getstatus(request->read) == 1) {
                int got = 0;
                int status = 0;
                ASYNCFILE_release(request->read, &status, &got);
                request->cursor += got;
                request->offset += got;
                m_progress += (float)got / (float)((LoadedHeaderView*)request->cdb)->size;
                if (request->cursor - (char*)request->cdb < ((LoadedHeaderView*)request->cdb)->size) {
                    request->state = 6;
                } else {
                    FILESYS_closesync(request->file, 100);
                    request->state = 8;
                }
            } else {
                done = true;
            }
            break;
        case 8: {
            {
                CCompartment compartment;
                PushCompartment(end, compartment);
            }
            CCompartment* added = &m_compartments.prev->compartment;
            added->Init(request->id, request->buffer, request->cpt, request->cdb);
            CCDBObject* object = added->GetCDBObject(0);
            m_world.AddSubVolume(*object);
            done = true;
            WorldRequestView* next;
            WorldRequestView* prev;
            WorldRequestView* first = m_requests.next;
            prev = first->prev;
            next = first->next;
            prev->next = next;
            next->prev = prev;
            first->prev = 0;
            first->next = m_requestFree;
            m_requestFree = first;
            m_requestCount--;
            break;
        }
        }
    }
}

void CWorldObject::HandleExplosion(const CExplosion& explosion) {
    WorldCompartmentNodeView* node = m_compartments.next;
    WorldCompartmentNodeView* end = (WorldCompartmentNodeView*)&m_compartments;
    for (; node != end; node = node->next)
        node->compartment.HandleExplosion(explosion);
}

CCompartment* CWorldObject::GetCompartment(int index) {
    WorldCompartmentNodeView* node = m_compartments.next;
    WorldCompartmentNodeView* end = (WorldCompartmentNodeView*)&m_compartments;
    for (; node != end; node = node->next) {
        if (index == node->compartment.m_index)
            return &node->compartment;
    }
    return 0;
}
