#include <pthread.h>
#include <jni.h>
#include <memory.h>
#include <dlfcn.h>
#include <cstdio>
#include <cstdlib>
#include <Includes/Vector3.hpp>
#include <Includes/Unity.h>
#include <Includes/Utils.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <thread>
#include <bits/sysconf.h>
#include <sys/mman.h>

#include "Includes/Logger.h"
#include "Patching/Patch.h"
#import "Includes/Utils.h"

bool bankcontrollerhookinitialized = false;
bool playermovecupdatehookintitialzed = false;
//bool FirstPersonControlSharpHookInitialized = false;
void* bankControllerInstance;

void(*old_BankControllerUpdate)(void *instance);
void BankControllerUpdate(void *instance) {
    if(instance != NULL) {
        bankControllerInstance = instance;
        if(!bankcontrollerhookinitialized) {
            bankcontrollerhookinitialized = true;
            LOGI("RaycastBulletScriptHookInitialized hooked");
            
        }
        if (isAddCoins) {
            {
                void (*AddCoins)(void* _this, int, bool, int) =
                (void (*)(void* _this, int, bool, int))getRealOffset(0x26032F4); // BankController$$AddCoins

                AddCoins(instance, 100, true, 1);
                isAddCoins = !AddCoins;
            }
        }
        if (isAddGems) {
            {
                void (*AddGems)(void* _this, int, bool, int) =
                (void (*)(void* _this, int, bool, int))getRealOffset(0x2603538); // BankController$$AddGems

                AddGems(instance, 100, true, 1);
                isAddGems = !AddGems;
            }
        }
        if (isAddCoupons) {
            {
                void (*AddCoupons)(void* _this, int, bool, int) =
                (void (*)(void* _this, int, bool, int))getRealOffset(0x2606138); // BankController$$AddCoupons

                AddCoupons(instance, 100, true, 1);
                isAddGems = !AddCoupons;
            }
             
        }
    }
    old_BankControllerUpdate(instance);
}

void (*ActivateXRay)(void* _this, bool activate);
void (*SendChat)(void* _this, monoString* text, bool ClanLogo, monoString* iconName);

void(*old_player_move_c_update)(void *instance);
void player_move_c_update(void *instance) {
    if(instance != NULL) {
        if(!playermovecupdatehookintitialzed) {
            playermovecupdatehookintitialzed = true;
            LOGI("RaycastBulletScriptHookInitialized hooked");
        }
        if (isXray) {
            *(bool *) ((uint64_t) instance + 0x3A8) = true;
            ActivateXRay(instance, true);
        }
        if (isChatSpam) {
            SendChat(instance, CreateMonoString("Slice Cast is the best!"), false, CreateMonoString("Slice Cast is the best!"));
        }
    }
    old_player_move_c_update(instance);
}

bool (*old_HealthRegen)(void *instance);
bool HealthRegen(void *instance) {
    if (instance != NULL) {
        if (isHealthRegen) {
            return true;
        }
    }
    return old_HealthRegen(instance);
}

bool (*old_ArmorRegen)(void *instance);
bool ArmorRegen(void *instance) {
    if (instance != NULL) {
        if (isArmorRegen) {
            return true;
        }
    }
    return old_ArmorRegen(instance);
}

bool (*old_NinjaJump)(void *instance);
bool NinjaJump(void *instance) {
    if (instance != NULL) {
        if (isFly) {
            return true;
        }
    }
    return old_NinjaJump(instance);
}

bool (*old_ScatterReduction)(void *instance);
bool ScatterReduction(void *instance) {
    if (instance != NULL) {
        if (isNoSpread) {
            return false;
        }
    }
    return old_ScatterReduction(instance);
}

bool (*old_CriticalHit)(void *instance);
bool CriticalHit(void *instance) {
    if (instance != NULL) {
        if (isCritHit) {
            return true;
        }
    }
    return old_CriticalHit(instance);
}

bool (*old_DisabledRecoil)(void *instance);
bool DisabledRecoil(void *instance) {
    if (instance != NULL) {
        if (isNoRecoil) {
            return true;
        }
    }
    return old_DisabledRecoil(instance);
}

float (*old_GadgetCooldown)(void *instance);
float GadgetCooldown(void *instance) {
    if (instance != NULL) {
        if (isGadgetCooldown) {
            return 0.0f;
        }
    }
    return old_GadgetCooldown(instance);
}

float (*old_Speed)(void *instance);
float Speed(void *instance) {
    if (instance != NULL) {
        if (speedModifier) {
            return (float) speedModifier;
        }
        else {
            return 0.0f;
        }
    }
    return old_Speed(instance);
}

float (*old_Jump)(void *instance);
float Jump(void *instance) {
    if (instance != NULL) {
        if (jumpModifier) {
            return (float) jumpModifier;
        }
        else {
            return 0.0f;
        }
    }
    return old_Jump(instance);
}

float (*old_Level)(void *instance);
float Level(void *instance) {
    if (instance != NULL) {
        if (jumpModifier) {
            return (int) levelModifier;
        }
        else {
            return 1;
        }
    }
    return old_Level(instance);
}



    ActivateXRay = (void (*)(void*, bool))getRealOffset(0x184E480);
    SendChat = (void (*)(void*, monoString*, bool, monoString*))getRealOffset(Offsets::RSendChatOffset);

    hook((void*)getRealOffset(Offsets::RBankControllerUpdate), (void *)BankControllerUpdate, (void **) &old_BankControllerUpdate);
    hook((void*)getRealOffset(Offsets::RPlayer_move_c_Update), (void *)player_move_c_update, (void **) &old_player_move_c_update);
    hook((void*)getRealOffset(Offsets::RFirstPersonControlSharpUpdate), (void *)FirstPersonControlSharp, (void **) &old_FirstPersonControlSharp);
    hook((void*)getRealOffset(Offsets::Rget_healthregen), (void *)HealthRegen, (void **) &old_HealthRegen);
    hook((void*)getRealOffset(Offsets::Rget_armorregen), (void *)ArmorRegen, (void **) &old_ArmorRegen);
    hook((void*)getRealOffset(Offsets::Rget_ninjajump), (void *)NinjaJump, (void **) &old_NinjaJump);
    hook((void*)getRealOffset(Offsets::Rget_ScatterReduction), (void *)ScatterReduction, (void **) &old_ScatterReduction);
    hook((void*)getRealOffset(Offsets::Rget_AlwaysCriticalHit), (void *)CriticalHit, (void **) &old_CriticalHit);
    hook((void*)getRealOffset(Offsets::Rget_DisabledRecoil), (void *)DisabledRecoil, (void **) &old_DisabledRecoil);
    hook((void*)getRealOffset(Offsets::RGadgetCooldown), (void *)GadgetCooldown, (void **) &old_GadgetCooldown);
    hook((void*)getRealOffset(Offsets::RSpeedModifier), (void *)Speed, (void **) &old_Speed);
    hook((void*)getRealOffset(Offsets::RSpeedModifier), (void *)Jump, (void **) &old_Jump);
    hook((void*)getRealOffset(Offsets::Rget_CurrentLevel), (void *)Level, (void **) &old_Level);
