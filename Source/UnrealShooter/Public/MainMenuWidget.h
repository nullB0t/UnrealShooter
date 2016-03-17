// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class UNREALSHOOTER_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

		APlayerController* PController;
	
public:

	virtual void NativeConstruct() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ResolutionScale")
		FString WINDOW_BIG = "r.SetRes 1920x1080";

	//Resolution
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ResolutionScale")
		FString RESOLUTION_LOW = "sg.ResolutionQuality 25";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ResolutionScale")
		FString RESOLUTION_MID = "sg.ResolutionQuality 50";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ResolutionScale")
		FString RESOLUTION_HIGH = "sg.ResolutionQuality 75";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ResolutionScale")
		FString RESOLUTION_EPIC = "sg.ResolutionQuality 100";

	//ViewDistance
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ViewDistanceScale")
		FString VIEWDISTANCE_LOW = "sg.ViewDistanceQuality 0";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ViewDistanceScale")
		FString VIEWDISTANCE_MID = "sg.ViewDistanceQuality 0.4f";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ViewDistanceScale")
		FString VIEWDISTANCE_HIGH = "sg.ViewDistanceQuality 0.7f";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ViewDistanceScale")
		FString VIEWDISTANCE_EPIC = "sg.ViewDistanceQuality 1.0f";

	//AntiAliasing
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AntiAliasingQuality")
		FString ANTIALIASING_LOW = "sg.AntiAliasingQuality 0";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AntiAliasingQuality")
		FString ANTIALIASING_MID = "sg.AntiAliasingQuality 2";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AntiAliasingQuality")
		FString ANTIALIASING_HIGH = "sg.AntiAliasingQuality 4";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AntiAliasingQuality")
		FString ANTIALIASING_EPIC = "sg.AntiAliasingQuality 6";


	//PostProcessing
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PostProcessQuality")
		FString POSTPROCESSING_LOW = "sg.PostProcessQuality 0";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PostProcessQuality")
		FString POSTPROCESSING_MID = "sg.PostProcessQuality 1";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PostProcessQuality")
		FString POSTPROCESSING_HIGH = "sg.PostProcessQuality 2";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PostProcessQuality")
		FString POSTPROCESSING_EPIC = "sg.PostProcessQuality 3";

	//Shadows
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ShadowQuality")
		FString SHADOWS_LOW = "sg.ShadowQuality 0";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ShadowQuality")
		FString SHADOWS_MID = "sg.ShadowQuality 1";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ShadowQuality")
		FString SHADOWS_HIGH = "sg.ShadowQuality 2";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ShadowQuality")
		FString SHADOWS_EPIC = "sg.ShadowQuality 3";


	//Textures
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TextureQuality")
		FString TEXTURES_LOW = "sg.TextureQuality 0";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TextureQuality")
		FString TEXTURES_MID = "sg.TextureQuality 1";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TextureQuality")
		FString TEXTURES_HIGH = "sg.TextureQuality 2";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TextureQuality")
		FString TEXTURES_EPIC = "sg.TextureQuality 3";

	//Effects
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EffectsQuality")
		FString EFFECTS_LOW = "sg.EffectsQuality 0";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EffectsQuality")
		FString EFFECTS_MID = "sg.EffectsQuality 1";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EffectsQuality")
		FString EFFECTS_HIGH = "sg.EffectsQuality 2";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EffectsQuality")
		FString EFFECTS_EPIC = "sg.EffectsQuality 3";


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "World")
		UWorld* WorldReference;

	UFUNCTION(BlueprintCallable, Category = "Bindings")
		void ExecuteConsoleCommand(FString cmd);

	UFUNCTION(BlueprintCallable, Category = "Bindings")
		void SetQuality(int32 value);

	UFUNCTION(BlueprintCallable, Category = "Bindings")
		void OnSliderValueChanged(float value);

	UFUNCTION(BlueprintCallable, Category = "Bindings")
		void ShowMouseCursor(bool value);
};