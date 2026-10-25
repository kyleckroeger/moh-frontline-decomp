// A fragment of mainloop.cpp (0x8007a810): Gameloop_Loop runs one game-loop
// step: it resumes and pauses debug timer 4, updates the script system
// (BSUtilUpdate, timed into profile bin 11 with OSGetTick and counted), then
// for each player places the player's camera: the camera's world matrix is
// the player's view matrix (+192) times the player's local-to-world matrix,
// the field of view comes from the player's angle in degrees (halved aspect
// for two players), and the camera is made current, updated and made current
// again. The functions and globals are named by the mangled symbols; the
// profile bin, player, camera (virtual slots by offset) and scene views are
// inferred; the scene view must have its real size (else small-data
// addressing). The rest of the file is in other units.
extern "C" unsigned long OSGetTick();

struct SProfileBin {
    int m_id;
    int m_values[5];
};

extern SProfileBin g_ProfileBins[31];
extern int g_NumPlayers;

void DebugProfTimerResume(int);
void DebugProfTimerPause(int);
void BSUtilUpdate(float);

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    void Multiply(const CMatrix&, const CMatrix&);

    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));

class CCamera {
public:
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c();
    virtual void vf20();
    virtual void AttemptUpdate(float);
    virtual void vf28();
    virtual void vf2c();
    virtual void vf30();
    virtual void vf34();
    virtual void vf38();
    virtual void vf3c();
    virtual void vf40();
    virtual void vf44();
    virtual void vf48();
    virtual void vf4c();
    virtual void vf50();
    virtual void vf54();
    virtual void vf58();
    virtual void vf5c();
    virtual void vf60();
    virtual void vf64();
    virtual void vf68();
    virtual void vf6c();
    virtual void vf70();
    virtual void vf74();
    virtual void vf78();
    virtual void vf7c();
    virtual void vf80();
    virtual void vf84();
    virtual void vf88();
    virtual void vf8c();
    virtual void vf90();
    virtual void vf94();
    virtual void vf98();
    virtual void vf9c();
    virtual void vfa0();
    virtual void vfa4();
    virtual void vfa8();
    virtual void vfac();
    virtual void vfb0();
    virtual void vfb4();
    virtual void vfb8();
    virtual void vfbc();
    virtual void vfc0();
    virtual void vfc4();
    virtual void vfc8();
    virtual void vfcc();
    virtual void vfd0();
    virtual void vfd4();
    virtual void vfd8();
    virtual void vfdc();
    virtual void vfe0();
    virtual void vfe4();
    virtual void vfe8(const CMatrix&);
    virtual void vfec();
    virtual void vff0();
    virtual void vff4();
    virtual void vff8();
    virtual void vffc();
    virtual void vf100();
    virtual void vf104();
    virtual void vf108();
    virtual void vf10c();
    virtual void vf110();
    void SetPerspective(float, float);

    static const float ASPECT_4x3;
};

class CPlayerObject {
public:
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c();
    virtual void vf20();
    virtual void vf24();
    virtual void vf28();
    virtual void vf2c();
    virtual void vf30();
    virtual void vf34();
    virtual void vf38();
    virtual void GetTMLocalToWorld(CMatrix&) const;

    unsigned char unknown004[188];
    CMatrix m_matrix;
    unsigned char unknown100[812];
    float m_fov;
    unsigned char unknown430[928];
    CCamera m_camera;
};

class CScene {
public:
    CPlayerObject* GetPlayer(int) const;
    void SetCamera(CCamera*);

    unsigned char unknown000[328];
};

extern CScene g_scene;

void Gameloop_Loop(float delta) {
    DebugProfTimerResume(4);
    DebugProfTimerPause(4);
    g_ProfileBins[11].m_values[1] -= OSGetTick();
    BSUtilUpdate(delta);
    g_ProfileBins[11].m_values[1] += OSGetTick();
    g_ProfileBins[11].m_values[4]++;
    CCamera* camera;
    const CMatrix* matrix;
    int i;
    CPlayerObject* player;
    for (i = 0; i < g_NumPlayers; i++) {
        player = g_scene.GetPlayer(i);
        camera = &player->m_camera;
        matrix = &player->m_matrix;
        CMatrix local;
        player->GetTMLocalToWorld(local);
        CMatrix world;
        world.Multiply(*matrix, local);
        camera->vfe8(world);
        camera->vf110();
        float fov = 0.017453292f * player->m_fov;
        if (g_NumPlayers == 2)
            camera->SetPerspective(fov, 0.5f * fov * CCamera::ASPECT_4x3);
        else
            camera->SetPerspective(fov, fov * CCamera::ASPECT_4x3);
        g_scene.SetCamera(camera);
        camera->AttemptUpdate(delta);
        g_scene.SetCamera(camera);
    }
}
