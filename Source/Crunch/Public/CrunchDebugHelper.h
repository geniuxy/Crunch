#pragma once

namespace Debug
{
	static void Print(
		const FString& Msg, int32 InKey = -1, float Duration = 15.f, const FColor& InColor = FColor::MakeRandomColor())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(InKey, Duration, InColor, Msg);

			UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);
		}
	}


	static void Print(const FString& Msg, float InValue)
	{
		Print(FString::Printf(TEXT("%s: %f"), *Msg, InValue));
	}
}
