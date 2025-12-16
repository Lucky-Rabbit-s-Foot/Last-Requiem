// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/Paths.h"
#include "Engine/Engine.h"

LASTREQUIEM_API DECLARE_LOG_CATEGORY_EXTERN(LogKHS, Log, All);

enum class EKLogLevel : uint8
{
	Trace = 0, 
	Debug = 1,
	Info = 2, 
	Warn = 3,
	Error = 4,
	Fatal = 5
};

#if UE_BUILD_SHIPPING
#define KHS_COMPILE_TIME_MIN_LEVEL EKLogLevel::Warn
#else
#define KHS_COMPILE_TIME_MIN_LEVEL EKLogLevel::Trace
#endif

#define KHS_LEVEL_ENABLED(Level) (static_cast<uint8>(Level) >= static_cast<int32>(KHS_COMPILE_TIME_MIN_LEVEL))

#define KHS_LOG_INTERNAL(Category, Level, Format, ...)														  \
	do {																									  \
		if (KHS_LEVEL_ENABLED(Level))																		  \
		{																									  \
			switch(Level) {																					  \
                case EKLogLevel::Trace:																		  \
                    UE_LOG(Category, VeryVerbose, TEXT("[%s][%s:%d] " Format),								  \
                        TEXT(__FUNCTION__), *FPaths::GetCleanFilename(__FILE__), __LINE__, ##__VA_ARGS__);	  \
                    break;																					  \
                case EKLogLevel::Debug:																		  \
                    UE_LOG(Category, Verbose, TEXT("[%s][%s:%d] " Format),									  \
                        TEXT(__FUNCTION__), *FPaths::GetCleanFilename(__FILE__), __LINE__, ##__VA_ARGS__);    \
                    break;																					  \
                case EKLogLevel::Info:																		  \
                    UE_LOG(Category, Log, TEXT("[%s][%s:%d] " Format),										  \
                        TEXT(__FUNCTION__), *FPaths::GetCleanFilename(__FILE__), __LINE__, ##__VA_ARGS__);    \
                    break;																					  \
                case EKLogLevel::Warn:																		  \
                    UE_LOG(Category, Warning, TEXT("[%s][%s:%d] " Format),									  \
                        TEXT(__FUNCTION__), *FPaths::GetCleanFilename(__FILE__), __LINE__, ##__VA_ARGS__);    \
                    break;																					  \
                case EKLogLevel::Error:																		  \
                    UE_LOG(Category, Error, TEXT("[%s][%s:%d] " Format),									  \
                        TEXT(__FUNCTION__), *FPaths::GetCleanFilename(__FILE__), __LINE__, ##__VA_ARGS__);    \
                    break;																					  \
                case EKLogLevel::Fatal:																		  \
                    UE_LOG(Category, Fatal, TEXT("[%s][%s:%d] " Format),									  \
                        TEXT(__FUNCTION__), *FPaths::GetCleanFilename(__FILE__), __LINE__, ##__VA_ARGS__);    \
                    break;																					  \
				default:																					  \
					UE_LOG(Category, Log, TEXT("[%s][%s:%d] " Format),										  \
						TEXT(__FUNCTION__), *FPaths::GetCleanFilename(__FILE__), __LINE__, ##__VA_ARGS__);    \
				break;																						  \
            }																								  \
		} \
	} while(0)


#define KHS_TRACE(Format, ...) KHS_LOG_INTERNAL(LogKHS, EKLogLevel::Trace, Format, ##__VA_ARGS__)
#define KHS_DEBUG(Format, ...) KHS_LOG_INTERNAL(LogKHS, EKLogLevel::Debug, Format, ##__VA_ARGS__)
#define KHS_INFO(Format, ...)  KHS_LOG_INTERNAL(LogKHS, EKLogLevel::Info,  Format, ##__VA_ARGS__)
#define KHS_WARN(Format, ...)  KHS_LOG_INTERNAL(LogKHS, EKLogLevel::Warn,  Format, ##__VA_ARGS__)
#define KHS_ERROR(Format, ...) KHS_LOG_INTERNAL(LogKHS, EKLogLevel::Error, Format, ##__VA_ARGS__)
#define KHS_FATAL(Format, ...) KHS_LOG_INTERNAL(LogKHS, EKLogLevel::Fatal, Format, ##__VA_ARGS__)


#if WITH_EDITOR

#define KHS_SCREEN_INFO(Format, ...) \
do \
{ \
const FString DebugMessage = FString::Printf(Format, ##__VA_ARGS__); \
KHS_INFO(TEXT("SCREEN: %s"), *DebugMessage); \
if (GEngine) \
{ \
GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, DebugMessage); \
} \
} while(0)

#else

#define KHS_SCREEN_INFO(Format, ...) do {   } while(0)

#endif



class LASTREQUIEM_API K_LoggingSystem
{
public:
	K_LoggingSystem();
	~K_LoggingSystem();
};
