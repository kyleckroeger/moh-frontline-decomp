// A fragment of UIStudio.c (0x80088770): UISLoadScreen either queues thread
// action 0 with a group record holding the screen and resolution (the next
// two halfwords -1) when flag bit 1 of the studio record is set, returning
// 1, or loads the screen at once through _UISInternalLoad. UISInfo_t,
// UISThreadGroupInfoT, UISThreadActionT and UISParam_t are named by the
// mangled symbols; the views follow UIStudio_active.c (the halfword view of
// the group record is inferred) and the parameter meanings are inferred. The
// rest of the file is not part of this unit.
struct UISParam_t;

enum UISThreadActionT { UISThreadActionUnknown = 0 };

union UISThreadGroupInfoT {
    struct {
        short s0;
        short s2;
        long l4;
        long l8;
        long l12;
    } s;
    struct {
        unsigned short h0;
        unsigned short h2;
        unsigned short h4;
        unsigned short h6;
    } h;
};

struct UISInfo_t {
    char field000[4];
    unsigned long flags;
};

void UISAddThreadAction(UISInfo_t* info, UISThreadActionT action, UISThreadGroupInfoT* group, long count,
                        UISParam_t* params);
int _UISInternalLoad(UISInfo_t* info, unsigned short screen, unsigned short resolution, unsigned char,
                     unsigned char flag, UISParam_t* params);

extern "C" int UISLoadScreen(UISInfo_t* info, unsigned short screen, unsigned short resolution, unsigned char flag,
                             UISParam_t* params) {
    if (info->flags & 2) {
        UISThreadGroupInfoT group;

        group.h.h0 = screen;
        group.h.h2 = resolution;
        group.h.h4 = 0xffff;
        group.h.h6 = 0xffff;
        UISAddThreadAction(info, (UISThreadActionT)0, &group, flag, params);
    } else {
        return _UISInternalLoad(info, screen, resolution, 0, flag, params);
    }
    return 1;
}
