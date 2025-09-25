#pragma once

namespace Debug
{
	static void Print(const FString& Message, const FColor Color = FColor::MakeRandomColor(), float Duration = 2.f, int32 InKey = -1)	
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(InKey, Duration, Color, Message);
		}
	}
}