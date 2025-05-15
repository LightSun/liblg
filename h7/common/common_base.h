#pragma once

#if defined(_WIN32) || defined(WIN32)
#define CMD_LINE "\r\n"
#else
#define CMD_LINE "\n"
#endif

#define HFMT_BUF_32(code, fmt,...)\
do{\
    char buf[32];\
    snprintf(buf, 32, fmt, ##__VA_ARGS__);\
    code;\
}while(0);

#define HFMT_BUF_64(code, fmt, ...)\
do{\
    char buf[64];\
    snprintf(buf, 64, fmt, ##__VA_ARGS__);\
    code;\
}while(0);

#define HFMT_BUF_128(code, fmt, ...)\
do{\
    char buf[128];\
    snprintf(buf, 128, fmt, ##__VA_ARGS__);\
    code;\
}while(0);

#define HFMT_BUF_256(code, fmt ,...)\
do{\
    char buf[256];\
    snprintf(buf, 256, fmt, ##__VA_ARGS__);\
    code;\
}while(0);

#define HFMT_BUF_512(code, fmt ,...)\
do{\
    char buf[512];\
    snprintf(buf, 512, fmt, ##__VA_ARGS__);\
    code;\
}while(0);

#ifndef MED_ASSERT
#define MED_ASSERT(condition)                                                   \
    do                                                                      \
    {                                                                       \
        if (!(condition))                                                   \
        {                                                                   \
            std::cout << "Assertion failure: " << __FILE__ << "::" << __FUNCTION__  \
                                     << __LINE__ \
                                     << " >> " << #condition << std::endl;  \
            abort();                                                        \
        }                                                                   \
    } while (0)
#endif

#ifndef MED_ASSERT_IF
#define MED_ASSERT_IF(pre, condition)                                       \
    do                                                                      \
    {                                                                       \
        if (pre && !(condition))                                            \
        {                                                                   \
            std::cout << "Assertion failure: " << __FILE__ << "::" << __FUNCTION__  \
                                     << __LINE__ \
                                     << " >> " << #condition << std::endl;  \
            abort();                                                        \
        }                                                                   \
    } while (0)
#endif

#ifndef MED_ASSERT_X
#define MED_ASSERT_X(condition, msg)                                                   \
    do                                                                      \
    {                                                                       \
        if (!(condition))                                                   \
        {                                                                   \
            std::cout << "Assertion failure: " << __FILE__ << "::" << __FUNCTION__  \
                                     << __LINE__ \
                                     << "\n" << msg << " >> " << #condition << std::endl;  \
            abort();                                                        \
        }                                                                   \
    } while (0)
#endif

#ifndef MED_ASSERT_F
#define MED_ASSERT_F(condition, fmt ,...)                                                   \
    do                                                                      \
    {                                                                       \
        if (!(condition))                                                   \
        {      \
            char buf[512];\
            snprintf(buf, 512, fmt, ##__VA_ARGS__);\
            std::cout << "Assertion failure: " << __FILE__ << "::" << __FUNCTION__  \
                                     << __LINE__ \
                                     << "\n" << buf << " >> " << #condition << std::endl;  \
            abort();                                                        \
        }                                                                   \
    } while (0)
#endif

