/*
 * configuration routines
 */

#include <string>
#include "solvespace.h"

void CnfFreezeString(const std::string& str, const char *name)
{ 
}

void CnfFreezeDWORD(DWORD v, const char *name)
{
}

void CnfFreezeFloat(float v, const char *name)
{
}

std::string CnfThawString(const char *name)
{
	//FIXME
	return std::string();
}

DWORD CnfThawDWORD(DWORD v, const char *name)
{
	//FIMXE
	return v;
}

float CnfThawFloat(float v, const char *name) 
{
	//FIXME
	return v;
}
