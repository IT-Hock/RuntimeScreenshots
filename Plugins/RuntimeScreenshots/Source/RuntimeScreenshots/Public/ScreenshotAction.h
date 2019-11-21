// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "IImageWrapper.h"
#include "ScreenshotAction.generated.h"

/**
 *
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScreenshotJpeg, const TArray<uint8>&, Jpeg);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScreenshotFail);

UCLASS()
class USERFEEDBACKDIALOGUE_API UScreenshotAction : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(BlueprintAssignable, Category = Screenshot)
	FOnScreenshotJpeg OnScreenshotSuccess;
	UPROPERTY(BlueprintAssignable, Category = Screenshot)
	FOnScreenshotFail OnScreenshotFail;

	/**
	 * Request to make a screenshot
	 * @param withUI Should the UI be included in the screenshot?
	 * @param compress Should the resulting image be compressed?
	 * @param quality Quality of the resulting compressed image (1-100) 100 and 0 meaning best, 1 meaning worst (Has no effect when compress = false)
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = Screenshot)
	static UScreenshotAction* RequestScreenshot(const bool withUI, const bool compress, const int quality);

	// UBlueprintAsyncActionBase interface
	virtual void Activate() override;
	//~UBlueprintAsyncActionBase interface	
private:
	bool withUI;
	bool compress;
	int quality;

	bool bIsScreenshotRequested;
protected:
	UFUNCTION()
	virtual void AcceptScreenshot(int32 InSizeX, int32 InSizeY, const TArray<FColor>& InImageData);
};
