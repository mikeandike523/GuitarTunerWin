 
#ifndef _INC_PROPIDL_PA
#define _INC_PROPIDL_PA
#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif
typedef const PROPVARIANT *REFPROPVARIANT;
#define PropVariantInit(VAR) memset((VAR), 0, sizeof(PROPVARIANT))
WINOLEAPI PropVariantClear(PROPVARIANT *pvar);
#endif  
