// animst.cpp: its zero-initialised globals (0xc2c bytes), written by research/tools/bss_decls.py
// and checked by check_data.py. Views marked "view" hold the size only; real layouts come with the shared headers.

struct AnimFileStateAnimList_t { unsigned char data[0xbbc]; }; // view: size from the original
struct AnimTranTransListArray_t { unsigned char data[0x68]; }; // view: size from the original
struct AnimStFinishCallback { unsigned char data[0x4]; }; // view: size from the original
struct AnimStCallbackTableView;

AnimFileStateAnimList_t _AnimSt_pStatAnimList; // file-local in the original // address: _AnimSt_pStatAnimList 0x801d3e38
AnimTranTransListArray_t AnimTran_TransListArray;
AnimStFinishCallback _AnimSt_FinishCb; // file-local in the original // address: _AnimSt_FinishCb 0x8034e968
AnimStCallbackTableView* _AnimSt_CallbackTable; // file-local in the original // address: _AnimSt_CallbackTable 0x8034e96c
