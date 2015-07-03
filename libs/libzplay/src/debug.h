#include <assert.h>

#if _DEBUG
	#define ASSERT_W(f)	assert(f)
	#define PRINT_INT(a) printf("%i\n", a)
	#define PRINT_UINT(a) printf("%u\n", a)
	#define PRINT_FLOAT(a) printf("%f\n", a)
	#define PRINT_STRING(a) printf("%s\n", a)
#else
	#define ASSERT_W(f) NULL
	#define PRINT_INT(a) NULL
	#define PRINT_UINT(a) NULL
	#define PRINT_FLOAT(a) NULL
	#define PRINT_STRING(a) NULL
#endif