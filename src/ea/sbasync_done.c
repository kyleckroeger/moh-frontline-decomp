/* SNDBANK_asyncdone (0x8015a5f8; the code map gives no file record for it,
   though it follows sbasync.c's functions): whether the asynchronous bank
   load has finished - 0 while a load is in progress, else 1 or the load's
   error code. sndbas is named by its symbol; its view follows sbasync.c and
   is inferred (the error word at +8). */
struct SndBankAsyncView {
    void* load;
    int priority;
    int error;
};

extern SndBankAsyncView sndbas;

extern "C" int SNDBANK_asyncdone(void) {
    int result;
    if (!sndbas.load) {
        result = 1;
        if (sndbas.error)
            result = sndbas.error;
        return result;
    }
    return 0;
}
