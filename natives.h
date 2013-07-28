#pragma once



#include "mail.h"





class amxNatives
{

public:

	static const AMX_NATIVE_INFO mailNatives[];

	static cell AMX_NATIVE_CALL Init(AMX *amx, cell *params);
	static cell AMX_NATIVE_CALL Send(AMX *amx, cell *params);
	static cell AMX_NATIVE_CALL IsValid(AMX *amx, cell *params);
};