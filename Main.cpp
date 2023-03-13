#include <list>
#include <vector>
#include <string.h>
#include <pthread.h>
#include <thread>
#include <cstring>
#include <jni.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <dlfcn.h>
#include "Includes/Logger.h"
#include "Includes/obfuscate.h"
#include "Includes/Utils.h"
#include "KittyMemory/MemoryPatch.h"
#include "Menu/Setup.h"

//Target lib here
#define targetLibName OBFUSCATE("libil2cpp.so")

#include "Includes/Macros.h"

bool feature1, feature2, featureHookToggle, Health, UnlockSkins = false;
float LightRadius = 0, PlayerSpeed = 0;
void *instanceBtn;

// Hooking examples. Assuming you know how to write hook
bool (*old_unlockskins)(void *instance);
bool unlockskins(void *instance) {
    if (instance != NULL && UnlockSkins) {
        return true;
    }
    return old_unlockskins(instance);
}

float (*old_lightradius)(void *instance);
float lightradius(void *instance) {
    if (instance != NULL && LightRadius > 0) {
        return (float) LightRadius;
    }
    return old_lightradius(instance);
}

void (*old_playerspeed)(void *instance);
void playerspeed(void *instance) {
    if (instance != NULL) {
        if (PlayerSpeed >= 1) {
    //if PlayerSpeed is equal to or greater than 1 then it will modify the offset else it will return original value
            *(float *) ((uint32_t) instance + 0x2C) = PlayerSpeed;
        }
    }
    return old_playerspeed(instance);
}

/*
void (*AddMoneyExample)(void *instance, int amount);

bool (*old_get_BoolExample)(void *instance);
bool get_BoolExample(void *instance) {
    if (instance != NULL && featureHookToggle) {
        return true;
    }
    return old_get_BoolExample(instance);
}

float (*old_get_FloatExample)(void *instance);
float get_FloatExample(void *instance) {
    if (instance != NULL && sliderValue > 1) {
        return (float) sliderValue;
    }
    return old_get_FloatExample(instance);
}

int (*old_Level)(void *instance);
int Level(void *instance) {
    if (instance != NULL && level) {
        return (int) level;
    }
    return old_Level(instance);
}

void (*old_FunctionExample)(void *instance);
void FunctionExample(void *instance) {
    instanceBtn = instance;
    if (instance != NULL) {
        if (Health) {
            *(int *) ((uint64_t) instance + 0x48) = 999;
        }
    }
    return old_FunctionExample(instance);
}*/

// we will run our hacks in a new thread so our while loop doesn't block process main thread
void *hack_thread(void *) {
    LOGI(OBFUSCATE("pthread created"));

    //Check if target lib is loaded
    do {
        sleep(1);
    } while (!isLibraryLoaded(targetLibName));

    //Anti-lib rename
    /*
    do {
        sleep(1);
    } while (!isLibraryLoaded("libYOURNAME.so"));*/

    LOGI(OBFUSCATE("%s has been loaded"), (const char *) targetLibName);

#if defined(__aarch64__) //To compile this code for arm64 lib only. Do not worry about greyed out highlighting code, it still works
    


#else //To compile this code for armv7 lib only.

HOOK_LIB("libil2cpp.so", "0x9D3BAC", unlockskins, old_unlockskins);
HOOK_LIB("libil2cpp.so", "0x8FA3A0", lightradius, old_lightradius);
HOOK_LIB("libil2cpp.so", "0xA0C874", playerspeed, old_playerspeed);

/*
    // Hook example. Comment out if you don't use hook
    // Strings in macros are automatically obfuscated. No need to obfuscate!
    HOOK("str", FunctionExample, old_FunctionExample);
    HOOK_LIB("libFileB.so", "0x123456", FunctionExample, old_FunctionExample);
    HOOK_NO_ORIG("0x123456", FunctionExample);
    HOOK_LIB_NO_ORIG("libFileC.so", "0x123456", FunctionExample);
    HOOKSYM("__SymbolNameExample", FunctionExample, old_FunctionExample);
    HOOKSYM_LIB("libFileB.so", "__SymbolNameExample", FunctionExample, old_FunctionExample);
    HOOKSYM_NO_ORIG("__SymbolNameExample", FunctionExample);
    HOOKSYM_LIB_NO_ORIG("libFileB.so", "__SymbolNameExample", FunctionExample);

    // Patching offsets directly. Strings are automatically obfuscated too!
    PATCH("0x20D3A8", "00 00 A0 E3 1E FF 2F E1");
    PATCH_LIB("libFileB.so", "0x20D3A8", "00 00 A0 E3 1E FF 2F E1");

    //Restore changes to original
    RESTORE("0x20D3A8");
    RESTORE_LIB("libFileB.so", "0x20D3A8");

    AddMoneyExample = (void (*)(void *, int)) getAbsoluteAddress(targetLibName, 0x123456);
*/
    LOGI(OBFUSCATE("Done"));
#endif

    //Anti-leech
    /*if (!iconValid || !initValid || !settingsValid) {
        //Bad function to make it crash
        sleep(5);
        int *p = 0;
        *p = 0;
    }*/

    return NULL;
}

// Do not change or translate the first text unless you know what you are doing
// Assigning feature numbers is optional. Without it, it will automatically count for you, starting from 0
// Assigned feature numbers can be like any numbers 1,3,200,10... instead in order 0,1,2,3,4,5...
// ButtonLink, Category, RichTextView and RichWebView is not counted. They can't have feature number assigned
// Toggle, ButtonOnOff and Checkbox can be switched on by default, if you add True_. Example: CheckBox_True_The Check Box
// To learn HTML, go to this page: https://www.w3schools.com/

jobjectArray GetFeatureList(JNIEnv *env, jobject context) {
    jobjectArray ret;

    const char *features[] = {
        OBFUSCATE("Category_Mods"), //It will not counted
        OBFUSCATE("10_Toggle_Unlock Pets"), //It will assigned as Case 10
        OBFUSCATE("15_Toggle_Unlock Skins"), //It will assigned as Case 15
        OBFUSCATE("20_SeekBar_Player Level_0_4"),
        OBFUSCATE("25_SeekBar_Light Radius_0_20"),
        OBFUSCATE("30_SeekBar_Player Speed_0_100"),
        /*
        "40_Spinner_Spinner Name_Option 1,Option 2,Option 3",
        "50_ButtonOnOff_Its Name",
        "60_CheckBox_Checkbox",*/
    };

    //Now you dont have to manually update the number everytime;
    int Total_Feature = (sizeof features / sizeof features[0]);
    ret = (jobjectArray)
            env->NewObjectArray(Total_Feature, env->FindClass(OBFUSCATE("java/lang/String")),
                                env->NewStringUTF(""));

    for (int i = 0; i < Total_Feature; i++)
        env->SetObjectArrayElement(ret, i, env->NewStringUTF(features[i]));

    return (ret);
}

void Changes(JNIEnv *env, jclass clazz, jobject obj,
                                        jint featNum, jstring featName, jint value,
                                        jboolean boolean, jstring str) {

    LOGD(OBFUSCATE("Feature name: %d - %s | Value: = %d | Bool: = %d | Text: = %s"), featNum,
         env->GetStringUTFChars(featName, 0), value,
         boolean, str != NULL ? env->GetStringUTFChars(str, 0) : "");

    //BE CAREFUL NOT TO ACCIDENTLY REMOVE break;

    switch (featNum) {
        case 10:
            PATCH_LIB_SWITCH("libil2cpp.so", "0x9D3A04", "01 00 A0 E3 1E FF 2F E1", boolean);
          /*  if (boolean) {
            Toast(env,obj,OBFUSCATE("Unlock Pets Enabled"),ToastLength::LENGTH_SHORT);
            }*/
            break;
        case 15:
            UnlockSkins = boolean;
         /*   if (boolean) {
            Toast(env,obj,OBFUSCATE("Unlock Skins Enabled"),ToastLength::LENGTH_SHORT);
            }*/
            break;
         case 20:
             switch (value) {
                 case 0:
                     MemoryPatch::createWithHex(targetLibName, string2Offset(OBFUSCATE("0xA1E8A0")), OBFUSCATE("10 4C 2D E9 08 B0 8D E2")).Modify();
                     break;
                 case 1:
                     MemoryPatch::createWithHex(targetLibName, string2Offset(OBFUSCATE("0xA1E8A0")), OBFUSCATE("05 00 A0 E3 1E FF 2F E1")).Modify();
                     break;
                 case 2:
                     MemoryPatch::createWithHex(targetLibName, string2Offset(OBFUSCATE("0xA1E8A0")), OBFUSCATE("64 00 A0 E3 1E FF 2F E1")).Modify();
                     break;
                 case 3:
                     MemoryPatch::createWithHex(targetLibName, string2Offset(OBFUSCATE("0xA1E8A0")), OBFUSCATE("FF 00 A0 E3 1E FF 2F E1")).Modify();
                     break;
                 case 4:
                     MemoryPatch::createWithHex(targetLibName, string2Offset(OBFUSCATE("0xA1E8A0")), OBFUSCATE("50 03 0C E3 1E FF 2F E1")).Modify();
                     break;
             }
             break;
          case 25:
              LightRadius = value;
              break;
          case 30:
              PlayerSpeed = value;
              break;
          
          /*    
              
          case 40:
              //Same like we did for hex slider
              switch (value) {
                 case 0:
                     MemoryPatch::createWithHex(targetLibName, string2Offset(OBFUSCATE("0xD69D50")), OBFUSCATE("10 4C 2D E9 08 B0 8D E2")).Modify();
                     break;
                 case 1:
                     MemoryPatch::createWithHex(targetLibName, string2Offset(OBFUSCATE("0xD69D50")), OBFUSCATE("05 00 A0 E3 1E FF 2F E1")).Modify();
                     break;
                 case 2:
                     MemoryPatch::createWithHex(targetLibName, string2Offset(OBFUSCATE("0xD69D50")), OBFUSCATE("64 00 A0 E3 1E FF 2F E1")).Modify();
                     break;
                 case 3:
                     MemoryPatch::createWithHex(targetLibName, string2Offset(OBFUSCATE("0xD69D50")), OBFUSCATE("FF 00 A0 E3 1E FF 2F E1")).Modify();
                     break;
                 case 4:
                     MemoryPatch::createWithHex(targetLibName, string2Offset(OBFUSCATE("0xD69D50")), OBFUSCATE("50 03 0C E3 1E FF 2F E1")).Modify();
                     break;
              }
              break;
        case 50:
            //Same like toggle
            PATCH_LIB_SWITCH("libil2cpp.so", "0xBC8D60", "01 00 A0 E3 1E FF 2F E1", boolean);
            UnlockSkins = boolean;
            break;
       case 60:
           //Same like toggle
           PATCH_LIB_SWITCH("libil2cpp.so", "0xBC8D60", "01 00 A0 E3 1E FF 2F E1", boolean);
           UnlockSkins = boolean;
           break;
        
        
       /* case 0:
            // A much simpler way to patch hex via KittyMemory without need to specify the struct and len. Spaces or without spaces are fine
            // ARMv7 assembly example
            // MOV R0, #0x0 = 00 00 A0 E3
            // BX LR = 1E FF 2F E1
            PATCH_LIB_SWITCH("libil2cpp.so", "0x100000", "00 00 A0 E3 1E FF 2F E1", boolean);
            break;
        case 100:
            //Reminder that the strings are auto obfuscated
            //Switchable patch
            PATCH_SWITCH("0x400000", "00 00 A0 E3 1E FF 2F E1", boolean);
            PATCH_LIB_SWITCH("libil2cpp.so", "0x200000", "00 00 A0 E3 1E FF 2F E1", boolean);
            PATCH_SYM_SWITCH("_SymbolExample", "00 00 A0 E3 1E FF 2F E1", boolean);
            PATCH_LIB_SYM_SWITCH("libNativeGame.so", "_SymbolExample", "00 00 A0 E3 1E FF 2F E1", boolean);

            //Restore patched offset to original
            RESTORE("0x400000");
            RESTORE_LIB("libil2cpp.so", "0x400000");
            RESTORE_SYM("_SymbolExample");
            RESTORE_LIB_SYM("libil2cpp.so", "_SymbolExample");
            break;
        case 110:
            break;
        case 1:
            if (value >= 1) {
                sliderValue = value;
            }
            break;
        case 2:
            switch (value) {
                //For noobies
                case 0:
                    RESTORE("0x0");
                    break;
                case 1:
                    PATCH("0x0", "01 00 A0 E3 1E FF 2F E1");
                    break;
                case 2:
                    PATCH("0x0", "02 00 A0 E3 1E FF 2F E1");
                    break;
            }
            break;
        case 3:
            switch (value) {
                case 0:
                    LOGD(OBFUSCATE("Selected item 1"));
                    break;
                case 1:
                    LOGD(OBFUSCATE("Selected item 2"));
                    break;
                case 2:
                    LOGD(OBFUSCATE("Selected item 3"));
                    break;
            }
            break;
        case 4:
            // Since we have instanceBtn as a field, we can call it out of Update hook function
            if (instanceBtn != NULL)
                AddMoneyExample(instanceBtn, 999999);
            // MakeToast(env, obj, OBFUSCATE("Button pressed"), Toast::LENGTH_SHORT);
            break;
        case 5:
            break;
        case 6:
            featureHookToggle = boolean;
            break;
        case 7:
            level = value;
            break;
        case 8:
            break;
        case 9:
            break;*/
    }
}

__attribute__((constructor))
void lib_main() {
    // Create a new thread so it does not block the main thread, means the game would not freeze
    pthread_t ptid;
    pthread_create(&ptid, NULL, hack_thread, NULL);
}

int RegisterMenu(JNIEnv *env) {
    JNINativeMethod methods[] = {
            {OBFUSCATE("Icon"), OBFUSCATE("()Ljava/lang/String;"), reinterpret_cast<void *>(Icon)},
            {OBFUSCATE("IconWebViewData"),  OBFUSCATE("()Ljava/lang/String;"), reinterpret_cast<void *>(IconWebViewData)},
            {OBFUSCATE("IsGameLibLoaded"),  OBFUSCATE("()Z"), reinterpret_cast<void *>(isGameLibLoaded)},
            {OBFUSCATE("Init"),  OBFUSCATE("(Landroid/content/Context;Landroid/widget/TextView;Landroid/widget/TextView;)V"), reinterpret_cast<void *>(Init)},
            {OBFUSCATE("SettingsList"),  OBFUSCATE("()[Ljava/lang/String;"), reinterpret_cast<void *>(SettingsList)},
            {OBFUSCATE("GetFeatureList"),  OBFUSCATE("()[Ljava/lang/String;"), reinterpret_cast<void *>(GetFeatureList)},
    };

    jclass clazz = env->FindClass(OBFUSCATE("com/android/support/Menu"));
    if (!clazz)
        return JNI_ERR;
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return JNI_ERR;
    return JNI_OK;
}

int RegisterPreferences(JNIEnv *env) {
    JNINativeMethod methods[] = {
            {OBFUSCATE("Changes"), OBFUSCATE("(Landroid/content/Context;ILjava/lang/String;IZLjava/lang/String;)V"), reinterpret_cast<void *>(Changes)},
    };
    jclass clazz = env->FindClass(OBFUSCATE("com/android/support/Preferences"));
    if (!clazz)
        return JNI_ERR;
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return JNI_ERR;
    return JNI_OK;
}

int RegisterMain(JNIEnv *env) {
    JNINativeMethod methods[] = {
            {OBFUSCATE("CheckOverlayPermission"), OBFUSCATE("(Landroid/content/Context;)V"), reinterpret_cast<void *>(CheckOverlayPermission)},
    };
    jclass clazz = env->FindClass(OBFUSCATE("com/android/support/Main"));
    if (!clazz)
        return JNI_ERR;
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return JNI_ERR;

    return JNI_OK;
}

extern "C"
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);
    if (RegisterMenu(env) != 0)
        return JNI_ERR;
    if (RegisterPreferences(env) != 0)
        return JNI_ERR;
    if (RegisterMain(env) != 0)
        return JNI_ERR;
    return JNI_VERSION_1_6;
}
