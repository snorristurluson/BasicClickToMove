// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SplineComponent.h"

#include "NavigatorComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BASICCLICKTOMOVE_API UNavigatorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UNavigatorComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
    TArray<FVector> FindPathToLocation(FVector Location);		

	UFUNCTION(BlueprintCallable)
	void Navigate(const TArray<FVector>& Points);

	UFUNCTION(BlueprintCallable)
	void Stop();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector Goal;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector NextPoint;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float DistanceToGoal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Length;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CurrentTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GoalDistanceThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DistanceThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DistanceBetweenPoints;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USplineComponent* Spline;
};
