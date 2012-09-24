/**
*	@file APIHook.h
*/

#pragma once

#include <Windows.h>


void	APIHook(PCSTR pszModuleName, PCSTR pszFuncName, PROC pfnReplace, PROC* ppfnOrig);
