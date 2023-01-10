#ifndef __SYDF_API_H__
#define __SYDF_API_H__
#include "typedef.h"
#include "sydfile.h"

//call by syd lib
void sydf_hdl_init(void);
SYDFILE *sydf_fhdl_malloc(void);
SYDFILE *sydf_fhdl_free(SYDFILE *pfile);
SYDFS *sydf_fshdl_malloc(void);
SYDFS *sydf_fshdl_free(SYDFS *pfs);

#endif
