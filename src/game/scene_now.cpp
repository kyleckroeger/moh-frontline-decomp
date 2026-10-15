// A fragment of scene.cpp (0x800db9e0): CScene's immediate node changes, run
// directly or later from the queued commands (scene.cpp). RemoveNode_now
// unlinks a node that is in the scene from the node list and from the
// animated-object, static-object or light list it belongs to, removes its
// extents unless it is a light, and drops the vector entries (+280) that
// refer to it, flagging the players involved; AddNode_now links a node into
// the node list and the matching typed list and adds its extents unless it
// is a light; SetCamera_now and SetWorld_now replace the camera (+28; kept
// out of the lists in multiplayer) or world (+24) node; AddPlayer_now adds a
// player once. CScene, ISceneNode and the As* accessors are named by the
// symbols (vtable slots from __vt__10ISceneNode, the others are
// placeholders); the scene, node, list and vector-element layouts, the
// player flag and the link/unlink helpers are inferred. These functions are
// not inlined into the command functions in the original, so they are a
// separate unit here (deferred inlining in one TU would inline them).
// Compiled with deferred inlining, so the functions appear in reverse.
extern bool g_bInMultiplayerMode;

class CStaticObject;
class CPlayerObject;
class CAnimObject;
class CLight;

// ISceneNode's virtual functions up to AsLight (+180); the slots not used
// here are placeholders named by offset (names from __vt__10ISceneNode).
class ISceneNode {
public:
    virtual void unknown008();
    virtual void unknown00c();
    virtual void unknown010();
    virtual void unknown014();
    virtual void unknown018();
    virtual void unknown01c();
    virtual void unknown020();
    virtual void unknown024();
    virtual void unknown028();
    virtual void unknown02c();
    virtual void unknown030();
    virtual void unknown034();
    virtual void unknown038();
    virtual void unknown03c();
    virtual void unknown040();
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown04c();
    virtual void unknown050();
    virtual void unknown054();
    virtual void unknown058();
    virtual void unknown05c();
    virtual void unknown060();
    virtual void unknown064();
    virtual void unknown068();
    virtual void unknown06c();
    virtual void unknown070();
    virtual CStaticObject* AsStaticObject();
    virtual void unknown078();
    virtual void unknown07c();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual CAnimObject* AsAnimObject();
    virtual void unknown090();
    virtual void unknown094();
    virtual void unknown098();
    virtual void unknown09c();
    virtual CPlayerObject* AsPlayerObject() const;
    virtual void unknown0a4();
    virtual void unknown0a8();
    virtual void unknown0ac();
    virtual void unknown0b0();
    virtual CLight* AsLight();

    ISceneNode* m_prev;
    ISceneNode* m_next;
};

class CPlayerObject {
public:
    unsigned char unknown000[916];
    unsigned char m_flags394a : 3;
    bool m_flag394 : 1;
    unsigned char m_flags394b : 4;
};

// A 12-byte element of the scene's vector at +280: two nodes and a value
// (a view; the element type is not named by the symbols).
struct SceneNodeLinkView {
    ISceneNode* a;
    ISceneNode* b;
    float value;
};

namespace dwi {
template <class T>
class fast_vec {
public:
    T* begin() { return m_data; }
    T* end() { return m_data + m_size; }
    void erase(T* it) {
        T* last = m_data + m_size;
        for (T* src = it + 1; src != last;)
            *it++ = *src++;
        m_size--;
    }

    T* m_data;
    int m_unknown4;
    int m_size;
    int m_capacity;
    bool m_owned;
    int m_growth;
};
}

class CAnimObject {
public:
    unsigned char unknown00[36];
    CAnimObject* m_prev;
    CAnimObject* m_next;
};

class CStaticObject {
public:
    unsigned char unknown00[40];
    CStaticObject* m_prev;
    CStaticObject* m_next;
};

class CLight {
public:
    unsigned char unknown00[36];
    CLight* m_prev;
    CLight* m_next;
};

class CScene {
public:
    void RemoveNode_now(ISceneNode*);
    void AddNode_now(ISceneNode*);
    void SetCamera_now(ISceneNode*);
    void SetWorld_now(ISceneNode*);
    void AddPlayer_now(ISceneNode*);
    void AddNodeExtents(ISceneNode&);
    void RemoveNodeExtents(ISceneNode&);

    void Link(ISceneNode* node) {
        if (m_tail)
            m_tail->m_next = node;
        node->m_prev = m_tail;
        if (!m_head)
            m_head = node;
        m_tail = node;
        m_nodeCount++;
    }
    bool IsLinked(ISceneNode* node) {
        if (!node)
            return false;
        if (m_head == node)
            return true;
        if (m_tail == node)
            return true;
        if (node->m_next)
            return true;
        if (node->m_prev)
            return true;
        return false;
    }
    void Unlink(ISceneNode* node) {
        ISceneNode* prev = node->m_prev;
        ISceneNode* next = node->m_next;
        if (m_head == node)
            m_head = next;
        if (m_tail == node)
            m_tail = prev;
        if (prev)
            prev->m_next = next;
        if (next)
            next->m_prev = prev;
        node->m_prev = 0;
        node->m_next = 0;
        m_nodeCount--;
    }

    ISceneNode* m_head;
    ISceneNode* m_tail;
    ISceneNode* m_players[4];
    ISceneNode* m_world;
    ISceneNode* m_camera;
    CStaticObject* m_staticHead;
    CStaticObject* m_staticTail;
    CAnimObject* m_animHead;
    CAnimObject* m_animTail;
    CLight* m_lightHead;
    CLight* m_lightTail;
    int m_nodeCount;
    int m_staticCount;
    int m_animCount;
    int m_lightCount;
    bool m_updating;
    unsigned char unknown049[3];
    int m_playerCount;
    unsigned char unknown050[200];
    dwi::fast_vec<SceneNodeLinkView> m_links;
};

void CScene::AddPlayer_now(ISceneNode* node) {
    for (int i = 0; i < m_playerCount; i++) {
        if (node == m_players[i])
            return;
    }
    m_players[m_playerCount++] = node;
    Link(node);
    AddNodeExtents(*node);
}

void CScene::SetWorld_now(ISceneNode* world) {
    if (world == m_world)
        return;
    if (m_world) {
        Unlink(m_world);
        RemoveNodeExtents(*m_world);
    }
    if (world) {
        Link(world);
        AddNodeExtents(*world);
    }
    m_world = world;
}

void CScene::SetCamera_now(ISceneNode* camera) {
    if (camera == m_camera)
        return;
    if (m_camera && !g_bInMultiplayerMode) {
        Unlink(m_camera);
        RemoveNodeExtents(*m_camera);
    }
    if (camera && !g_bInMultiplayerMode) {
        Link(camera);
        AddNodeExtents(*camera);
    }
    m_camera = camera;
}

void CScene::AddNode_now(ISceneNode* node) {
    Link(node);
    CAnimObject* anim = node->AsAnimObject();
    CStaticObject* object = node->AsStaticObject();
    CLight* light = node->AsLight();
    if (anim) {
        if (m_animTail)
            m_animTail->m_next = anim;
        anim->m_prev = m_animTail;
        if (!m_animHead)
            m_animHead = anim;
        m_animTail = anim;
        m_animCount++;
    } else if (object) {
        if (m_staticTail)
            m_staticTail->m_next = object;
        object->m_prev = m_staticTail;
        if (!m_staticHead)
            m_staticHead = object;
        m_staticTail = object;
        m_staticCount++;
    } else if (light) {
        if (m_lightTail)
            m_lightTail->m_next = light;
        light->m_prev = m_lightTail;
        if (!m_lightHead)
            m_lightHead = light;
        m_lightTail = light;
        m_lightCount++;
    }
    if (!light)
        AddNodeExtents(*node);
}

void CScene::RemoveNode_now(ISceneNode* node) {
    if (!IsLinked(node))
        return;
    Unlink(node);
    CStaticObject* object = node->AsStaticObject();
    CAnimObject* anim = node->AsAnimObject();
    CLight* light = node->AsLight();
    if (anim) {
        CAnimObject* next;
        CAnimObject* prev;
        prev = anim->m_prev;
        next = anim->m_next;
        if (m_animHead == anim)
            m_animHead = next;
        if (m_animTail == anim)
            m_animTail = prev;
        if (prev)
            prev->m_next = next;
        if (next)
            next->m_prev = prev;
        anim->m_prev = 0;
        anim->m_next = 0;
        m_animCount--;
    } else if (object) {
        CStaticObject* next;
        CStaticObject* prev;
        prev = object->m_prev;
        next = object->m_next;
        if (m_staticHead == object)
            m_staticHead = next;
        if (m_staticTail == object)
            m_staticTail = prev;
        if (prev)
            prev->m_next = next;
        if (next)
            next->m_prev = prev;
        object->m_prev = 0;
        object->m_next = 0;
        m_staticCount--;
    } else if (light) {
        CLight* next;
        CLight* prev;
        prev = light->m_prev;
        next = light->m_next;
        if (m_lightHead == light)
            m_lightHead = next;
        if (m_lightTail == light)
            m_lightTail = prev;
        if (prev)
            prev->m_next = next;
        if (next)
            next->m_prev = prev;
        light->m_prev = 0;
        light->m_next = 0;
        m_lightCount--;
    }
    if (!light)
        RemoveNodeExtents(*node);
    SceneNodeLinkView* it = m_links.begin();
    while (it != m_links.end()) {
        if (it->a == node || it->b == node) {
            CPlayerObject* a = it->a->AsPlayerObject();
            CPlayerObject* b = it->b->AsPlayerObject();
            if (a)
                a->m_flag394 = true;
            if (b)
                b->m_flag394 = true;
            m_links.erase(it);
        } else {
            it++;
        }
    }
}
