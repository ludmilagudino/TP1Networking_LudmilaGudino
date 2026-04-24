#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Net/UnrealNetwork.h"
#include "ShineHeal_LudmilaTPCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UUserWidget;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(abstract)
class AShineHeal_LudmilaTPCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

private:

	UPROPERTY(ReplicatedUsing = OnRep_VidaActual, EditAnywhere, Category = "Vida", meta = (AllowPrivateAccess = "true"))
	float VidaActual;

	UPROPERTY(Replicated, EditAnywhere, Category = "Vida", meta = (AllowPrivateAccess = "true"))
	float VidaMaxima = 100.0f;

	UPROPERTY()
	UUserWidget* WidgetVida;

	UPROPERTY()
	UUserWidget* WidgetMensajeCurado;

protected:

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MouseLookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* InteractAction;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> WidgetVidaClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> WidgetMensajeCuradoClass;

	virtual void BeginPlay() override;

	void ActualizarUIVida();

public:

	AShineHeal_LudmilaTPCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_VidaActual();

	UFUNCTION(BlueprintCallable, Category = "Vida")
	float GetVidaActual() const { return VidaActual; }

	UFUNCTION(BlueprintCallable, Category = "Vida")
	float GetVidaMaxima() const { return VidaMaxima; }

	UFUNCTION(BlueprintCallable, Category = "Vida")
	void AgregarVida(float Cantidad);

	UFUNCTION(Server, Reliable)
	void Server_InteractuarSantuario();

	UFUNCTION(Client, Reliable)
	void Client_MostrarMensajeCurado();

protected:

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	void Interactuar();

public:

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoMove(float Right, float Forward);

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoLook(float Yaw, float Pitch);

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoJumpStart();

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoJumpEnd();

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};