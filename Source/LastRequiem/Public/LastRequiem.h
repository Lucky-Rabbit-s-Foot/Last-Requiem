// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN ( LogLR , Log , All );

#if UE_BUILD_SHIPPING
	#define LOG_CALLINFO			
	#define LOG_SIMPLE(Verbosity)	
	#define LOG_MESSAGE(Verbosity, Format, ...) 
	#define LOG_MESSAGE_SCREEN(Verbosity, Format, ...)
#else
	#define LOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(") "))
	#define LOG_SIMPLE(Verbosity) UE_LOG(LogLR, Verbosity, TEXT("%s"), *LOG_CALLINFO)
	#define LOG_MESSAGE(Verbosity, Format, ...) UE_LOG(LogLR, Verbosity, TEXT("%s %s"), *LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
	#define LOG_MESSAGE_SCREEN(Verbosity, Format, ...) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::Printf(Format, ##__VA_ARGS__));
#endif