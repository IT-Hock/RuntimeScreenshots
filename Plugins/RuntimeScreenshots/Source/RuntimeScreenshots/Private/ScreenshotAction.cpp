// Fill out your copyright notice in the Description page of Project Settings.

#include "ScreenshotAction.h"
#include "FunctionalUIScreenshotTest.h"
#include "Engine/Engine.h"
#include "IImageWrapperModule.h"
#include "ModuleManager.h"

UScreenshotAction::UScreenshotAction(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer),
                                                                                    withUI(false), compress(true),
                                                                                    quality(90),
                                                                                    bIsScreenshotRequested(false)
{
}

UScreenshotAction* UScreenshotAction::RequestScreenshot(const bool withUI,
                                                        const bool compress, const int quality)
{
	UScreenshotAction* BlueprintNode = NewObject<UScreenshotAction>();
	BlueprintNode->withUI = withUI;
	BlueprintNode->compress = compress;
	BlueprintNode->quality = quality;
	return BlueprintNode;
}

void UScreenshotAction::Activate()
{
	if (GEngine == nullptr || GEngine->GameViewport == nullptr || bIsScreenshotRequested)
	{
		UE_LOG(LogTemp, Warning, TEXT("Screenshot already requested."));
		return;
	}

	bIsScreenshotRequested = true;
	GEngine->GameViewport->OnScreenshotCaptured().AddUObject(this, &UScreenshotAction::AcceptScreenshot);

	FScreenshotRequest::RequestScreenshot(withUI);
}

void UScreenshotAction::AcceptScreenshot(int32 InSizeX, int32 InSizeY, const TArray<FColor>& InImageData)
{
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(
		FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);

	GEngine->GameViewport->OnScreenshotCaptured().RemoveAll(this);

	if (!ImageWrapper.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("ImageWrapper was invalid"));
		
		OnScreenshotFail.Broadcast();
		bIsScreenshotRequested = false;
		return;
	}

	if (!ImageWrapper->SetRaw(&InImageData[0], InImageData.Num() * sizeof(FColor), InSizeX, InSizeY, ERGBFormat::BGRA,
	                          8))
	{
		UE_LOG(LogTemp, Error, TEXT("Could not set ImageWrapper data"));
		
		OnScreenshotFail.Broadcast();
		bIsScreenshotRequested = false;
		return;
	}

	const TArray<uint8>& CompressedImage = ImageWrapper->GetCompressed(compress ? quality : 100);

	OnScreenshotSuccess.Broadcast(CompressedImage);

	bIsScreenshotRequested = false;
}
