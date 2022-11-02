// Define which prototype to run, and few other options

#if defined(__unix__)
#define SUDO_NODOCKER false
#define PROTO_IMGUI false
#define PROTO_ADJTIMEX false
#define PROTO_ARDUINOREAD false
#define PROTO_LIBSERIAL false
#define PROTO_TIMEFROMSTR false
#define PROTO_EXPERIMENT false
#define PROTO_COMBINED false
#elif defined(_WIN64) && !defined(__CYGWIN__)
#define PROTO_BOOSTPROCESS true
#endif