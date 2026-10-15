// UI studio shutdown and the registration of the client's transform,
// resource and message callbacks in the studio record. UISShutdown stops
// every thread group, last first (a thread action 1 for the group's id and
// thread, processed at once), then clears the event flag and the studio's
// active word. The callback slots, the group table and the action record
// are inferred from their offsets in an inferred view of UISInfo_t, and the
// callback types are left generic. The UIS files are compiled with -inline deferred,auto, so the
// functions are listed in reverse image order. The rest of the file is not
// part of this unit.
typedef void (*UISCallback)(void);

struct UISThreadGroupInfoT {
    unsigned short group;
    unsigned short thread;
    int value04;
};

struct UISThreadGroupEntryView {
    unsigned short group;
    unsigned short thread;
    unsigned char unknown04[8];
};

struct UISParam_t {
    int value;
};

enum UISThreadActionT {};

struct UISInfo_t {
    int active;
    unsigned char unknown04[8];
    UISCallback message;
    UISCallback resourceLoad;
    UISCallback resourceFree;
    UISCallback transform;
    unsigned char unknown1c[16];
    int groupCount;
    UISThreadGroupEntryView* groups;
};

void UISAddThreadAction(UISInfo_t*, UISThreadActionT, UISThreadGroupInfoT*, long, UISParam_t*);
void UISProcessThreadAction(UISInfo_t*, unsigned char);
extern unsigned char uisEventNotProcess;

extern "C" {
void UISShutdown(UISInfo_t* info) {
    for (int i = info->groupCount - 1; i >= 0; i--) {
        unsigned short id;
        unsigned short thread;
        UISThreadGroupEntryView* entry;
        UISThreadGroupInfoT group;
        UISParam_t param;
        entry = &info->groups[i];
        thread = entry->thread;
        id = entry->group;
        param.value = 0;
        group.group = id;
        group.thread = thread;
        UISAddThreadAction(info, (UISThreadActionT)1, &group, 1, &param);
        UISProcessThreadAction(info, 0);
    }
    uisEventNotProcess = 0;
    info->active = 0;
}

void UISRegisterMessageFnc(UISInfo_t* info, UISCallback message) {
    info->message = message;
}

void UISRegisterResourceFncs(UISInfo_t* info, UISCallback load, UISCallback release) {
    info->resourceLoad = load;
    info->resourceFree = release;
}

void UISRegisterTransformFncs(UISInfo_t* info, UISCallback transform) {
    info->transform = transform;
}
}
