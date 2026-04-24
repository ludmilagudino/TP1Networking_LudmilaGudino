// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShineHeal_LudmilaTPCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "ShineHeal_LudmilaTP.h"
#include "Net/UnrealNetwork.h"
#include "ShineHeal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

AShineHeal_LudmilaTPCharacter::AShineHeal_LudmilaTPCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	VidaActual = 100.0f;
	VidaMaxima = 100.0f;

	SetReplicates(true);
}

void AShineHeal_LudmilaTPCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocallyControlled())
	{
		if (WidgetVidaClass)
		{
			WidgetVida = CreateWidget<UUserWidget>(GetWorld(), WidgetVidaClass);
			if (WidgetVida)
			{
				WidgetVida->AddToViewport();
				ActualizarUIVida();
			}
		}
	}
}

void AShineHeal_LudmilaTPCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AShineHeal_LudmilaTPCharacter, VidaActual);
	DOREPLIFETIME(AShineHeal_LudmilaTPCharacter, VidaMaxima);
}

void AShineHeal_LudmilaTPCharacter::OnRep_VidaActual()
{
	ActualizarUIVida();
}

void AShineHeal_LudmilaTPCharacter::ActualizarUIVida()
{
	if (WidgetVida)
	{
		UTextBlock* TextVida = Cast<UTextBlock>(WidgetVida->GetWidgetFromName(TEXT("TextVida")));
		if (TextVida)
		{
			FString TextoVida = FString::Printf(TEXT("Vida: %.0f / %.0f"), VidaActual, VidaMaxima);
			TextVida->SetText(FText::FromString(TextoVida));
		}

		UProgressBar* BarraVida = Cast<UProgressBar>(WidgetVida->GetWidgetFromName(TEXT("BarraVida")));
		if (BarraVida)
		{
			float Porcentaje = VidaActual / VidaMaxima;
			BarraVida->SetPercent(Porcentaje);
		}
	}
}

void AShineHeal_LudmilaTPCharacter::AgregarVida(float Cantidad)
{
	VidaActual = FMath::Clamp(VidaActual + Cantidad, 0.0f, VidaMaxima);

	if (IsLocallyControlled())
	{
		ActualizarUIVida();
	}
}

void AShineHeal_LudmilaTPCharacter::Server_InteractuarSantuario_Implementation()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, AShineHeal::StaticClass());

	if (OverlappingActors.Num() > 0)
	{
		AShineHeal* Santuario = Cast<AShineHeal>(OverlappingActors[0]);
		if (Santuario)
		{
			Santuario->Server_IntentarUsar(this);
		}
	}
}

void AShineHeal_LudmilaTPCharacter::Client_MostrarMensajeCurado_Implementation()
{
	if (WidgetMensajeCuradoClass)
	{
		WidgetMensajeCurado = CreateWidget<UUserWidget>(GetWorld(), WidgetMensajeCuradoClass);
		if (WidgetMensajeCurado)
		{
			WidgetMensajeCurado->AddToViewport();

			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
				{
					if (WidgetMensajeCurado)
					{
						WidgetMensajeCurado->RemoveFromParent();
					}
				}, 3.0f, false);
		}
	}
}

void AShineHeal_LudmilaTPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AShineHeal_LudmilaTPCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AShineHeal_LudmilaTPCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AShineHeal_LudmilaTPCharacter::Look);

		// Interactuar
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AShineHeal_LudmilaTPCharacter::Interactuar);
	}
	else
	{
		UE_LOG(LogShineHeal_LudmilaTP, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AShineHeal_LudmilaTPCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AShineHeal_LudmilaTPCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AShineHeal_LudmilaTPCharacter::Interactuar()
{
	Server_InteractuarSantuario();
}

void AShineHeal_LudmilaTPCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AShineHeal_LudmilaTPCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AShineHeal_LudmilaTPCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void AShineHeal_LudmilaTPCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}