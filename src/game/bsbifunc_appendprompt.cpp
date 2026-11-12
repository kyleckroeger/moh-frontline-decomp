// A fragment of bsbifunc.cpp (0x8002ad98): appendMessageToPrompt, which appends
// text to the prompt buffer, word-wrapping at spaces: while the text does not
// fit the rest of the current line, the longest leading run of words within the
// line width is copied and terminated, and the cursor moves to the next of the
// 20 80-character lines; the rest is then copied and the cursor and column
// advanced. The file name is this project's; the original record is
// bsbifunc.cpp and the functions around it are not reconstructed. The function
// and globals are named by the symbols; the prompt globals are file-local
// statics in the original (declared extern here, without static, so this
// fragment can refer to them), and the second parameter is unused.
extern "C" {
unsigned long strlen(const char*);
char* strchr(const char*, int);
char* strncpy(char*, const char*, unsigned long);
char* strcpy(char*, const char*);
}

extern char g_promptBuffer[20][80];
extern char* g_promptCursor;
extern int g_promptColumn;
extern int g_prompt;
extern int g_promptMaxSize;
extern int g_promptLineWidth;

void appendMessageToPrompt(const char* text, int) {
    int length = strlen(text);
    while (length > g_promptMaxSize - g_promptColumn) {
        const char* end = text;
        while (end - text <= g_promptLineWidth - g_promptColumn) {
            const char* space = strchr(end, ' ');
            if (!space || space + 1 - text > g_promptMaxSize - g_promptColumn)
                break;
            end = space + 1;
        }
        strncpy(g_promptCursor, text, end - text);
        g_promptCursor[end - text] = 0;
        text = end;
        g_promptColumn = 0;
        g_prompt = (g_prompt + 1) % 20;
        g_promptCursor = g_promptBuffer[g_prompt];
        length = strlen(end);
    }
    strcpy(g_promptCursor, text);
    g_promptCursor += length;
    g_promptColumn += length;
}
