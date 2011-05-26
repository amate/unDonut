/**
*	@file APIHook.h
*/

#pragma once


void	APIHook(PCSTR pszModuleName, PCSTR pszFuncName, PROC pfnReplace, PROC* ppfnOrig);
