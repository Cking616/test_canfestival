
/* File generated by gen_cfile.py. Should not be modified. */

#ifndef TESTMASTER_H
#define TESTMASTER_H

#include "data.h"

/* Prototypes of function provided by object dictionnary */
UNS32 TestMaster_valueRangeTest (UNS8 typeValue, void * value);
const indextable * TestMaster_scanIndexOD (UNS16 wIndex, UNS32 * errorCode, ODCallback_t **callbacks);

/* Master node data struct */
extern CO_Data TestMaster_Data;
extern UNS16 Contolword2;		/* Mapped at index 0x2020, subindex 0x00*/
extern UNS16 Statusword2;		/* Mapped at index 0x2021, subindex 0x00*/
extern UNS32 SubMode2;		/* Mapped at index 0x2032, subindex 0x00*/
extern INTEGER8 Mode2;		/* Mapped at index 0x2060, subindex 0x00*/
extern INTEGER32 Position2;		/* Mapped at index 0x2064, subindex 0x00*/
extern INTEGER32 tPosition2;		/* Mapped at index 0x207A, subindex 0x00*/
extern UNS32 Speed2;		/* Mapped at index 0x2081, subindex 0x00*/

#endif // TESTMASTER_H
