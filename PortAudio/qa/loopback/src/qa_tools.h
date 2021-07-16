
#ifndef _QA_TOOLS_H
#define _QA_TOOLS_H
extern int g_testsPassed;
extern int g_testsFailed;
#define QA_ASSERT_TRUE( message, flag ) \
if( !(flag) ) \
{ \
printf( "%s:%d - ERROR - %s\n", __FILE__, __LINE__, message ); \
g_testsFailed++; \
goto error; \
} \
else g_testsPassed++;
#define QA_ASSERT_EQUALS( message, expected, actual ) \
if( ((expected) != (actual)) ) \
{ \
printf( "%s:%d - ERROR - %s, expected %d, got %d\n", __FILE__, __LINE__, message, expected, actual ); \
g_testsFailed++; \
goto error; \
} \
else g_testsPassed++;
#define QA_ASSERT_CLOSE( message, expected, actual, tolerance ) \
if (fabs((expected)-(actual))>(tolerance)) \
{ \
printf( "%s:%d - ERROR - %s, expected %f, got %f, tol=%f\n", __FILE__, __LINE__, message, ((double)(expected)), ((double)(actual)), ((double)(tolerance)) ); \
g_testsFailed++; \
goto error; \
} \
else g_testsPassed++;
#define QA_ASSERT_CLOSE_INT( message, expected, actual, tolerance ) \
if (abs((expected)-(actual))>(tolerance)) \
{ \
printf( "%s:%d - ERROR - %s, expected %d, got %d, tol=%d\n", __FILE__, __LINE__, message, ((int)(expected)), ((int)(actual)), ((int)(tolerance)) ); \
g_testsFailed++; \
goto error; \
} \
else g_testsPassed++;
#endif
