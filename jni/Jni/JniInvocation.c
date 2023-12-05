/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "JniInvocation.h"
#include "DlHelp.h"
#include "JNIHelp.h"
#include <sys/system_properties.h>
#include <errno.h>
#include <jni.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "DlHelp.h"

// Name the default library providing the JNI Invocation API.
static const char* kDefaultJniInvocationLibrary = "libart.so";
static const char* kDebugJniInvocationLibrary = "libartd.so";
#if defined(__LP64__)
#define LIB_DIR "lib64"
#else
#define LIB_DIR "lib"
#endif
static const char* kDebugJniInvocationLibraryPath = "/apex/com.android.art/" LIB_DIR "/libartd.so";
struct JniInvocationImpl {
  // Name of library providing JNI_ method implementations.
  const char* jni_provider_library_name;
  // Opaque pointer to shared library from dlopen / LoadLibrary.
  void* jni_provider_library;


  void* jni_provider_library_libartpalette;       
  void* jni_provider_library_libartbase;
  void* jni_provider_library_libdexfile;
  void* jni_provider_library_libprofile;
  void* jni_provider_library_libsigchain;
  void* jni_provider_library_libvixl;
  void* jni_provider_library_libartcompiler;
  void* jni_provider_library_libandroidruntime;



  // Function pointers to methods in JNI provider.
  jint (*JNI_GetDefaultJavaVMInitArgs)(void*);
  jint (*JNI_CreateJavaVM)(JavaVM**, JNIEnv**, void*);
  jint (*JNI_GetCreatedJavaVMs)(JavaVM**, jsize, jsize*);
  // Function pointer to register natives
  jint (*JNI_RegisterNatives)(JNIEnv *env, jclass clazz);
};
static struct JniInvocationImpl g_impl;
//
// Internal helpers.
//
static bool IsDebuggable() {

  char debuggable[PROP_VALUE_MAX] = {0};
  __system_property_get("ro.debuggable", debuggable);
  return strcmp(debuggable, "1") == 0;

}
static int GetLibrarySystemProperty(char* buffer) {

  return __system_property_get("persist.sys.dalvik.vm.lib.2", buffer);


}
static DlSymbol FindSymbol(DlLibrary library, const char* symbol) {
  DlSymbol s = DlGetSymbol(library, symbol);
  if (s == NULL) {
    //todo: error logging maybe
  }
  return s;
}
//
// Exported functions for JNI based VM management from JNI spec.
//
jint JNI_GetDefaultJavaVMInitArgs(void* vmargs) {
    //todo: error logging maybe
  return g_impl.JNI_GetDefaultJavaVMInitArgs(vmargs);
}
jint JNI_CreateJavaVM(JavaVM** p_vm, JNIEnv** p_env, void* vm_args) {
    //todo: error logging maybe
  return g_impl.JNI_CreateJavaVM(p_vm, p_env, vm_args);
}
jint JNI_GetCreatedJavaVMs(JavaVM** vms, jsize size, jsize* vm_count) {
  if (NULL == g_impl.JNI_GetCreatedJavaVMs) {
    *vm_count = 0;
    return JNI_OK;
  }
  return g_impl.JNI_GetCreatedJavaVMs(vms, size, vm_count);
}

jint JNI_RegisterNatives(JNIEnv *env, jclass clazz) {
    //todo: error logging maybe
  return g_impl.JNI_RegisterNatives(env, clazz);
}


//
// JniInvocation functions for setting up JNI functions.
//
const char* JniInvocationGetLibraryWith(const char* library,
                                        bool is_debuggable,
                                        const char* system_preferred_library) {
  if (is_debuggable) {
    // Debuggable property is set. Allow library providing JNI Invocation API to be overridden.
    // Choose the library parameter (if provided).
    if (library != NULL) {
      return library;
    }
    // If the debug library is installed, use it.
    // TODO(b/216099383): Do this in the test harness instead.
    struct stat st;
    if (stat(kDebugJniInvocationLibraryPath, &st) == 0) {
      return kDebugJniInvocationLibrary;
    } else if (errno != ENOENT) {
      //ALOGW("Failed to stat %s: %s", kDebugJniInvocationLibraryPath, strerror(errno));
      //todo: error logging maybe
    }
    // Choose the system_preferred_library (if provided).
    if (system_preferred_library != NULL) {
      return system_preferred_library;
    }
  }
  return kDefaultJniInvocationLibrary;
}
const char* JniInvocationGetLibrary(const char* library, char* buffer) {
  bool debuggable = IsDebuggable();
  const char* system_preferred_library = NULL;
  if (buffer != NULL && (GetLibrarySystemProperty(buffer) > 0)) {
    system_preferred_library = buffer;
  }
  return JniInvocationGetLibraryWith(library, debuggable, system_preferred_library);
}
struct JniInvocationImpl* JniInvocationCreate() {
  // Android only supports a single JniInvocation instance and only a single JavaVM.
  if (g_impl.jni_provider_library != NULL) {
    return NULL;
  }
  return &g_impl;
}


void OpenSoResources(struct JniInvocationImpl* instance)
{
  DlLibrary libb00 = DlOpenLibrary("libartpalette.so");
  DlLibrary libb01 = DlOpenLibrary("libartbase.so");
  DlLibrary libb02 = DlOpenLibrary("libdexfile.so");
  DlLibrary libb03 = DlOpenLibrary("libprofile.so");
  DlLibrary libb04 = DlOpenLibrary("libsigchain.so");
  DlLibrary libb06 = DlOpenLibrary("libvixl.so");
  instance->jni_provider_library_libartpalette = libb00;
  instance->jni_provider_library_libartbase = libb01;
  instance->jni_provider_library_libdexfile = libb02;
  instance->jni_provider_library_libprofile = libb03;
  instance->jni_provider_library_libsigchain = libb04;
  instance->jni_provider_library_libvixl = libb06;
}

bool JniInvocationInit(struct JniInvocationImpl* instance, const char* library_name) 
{
  char buffer[PROP_VALUE_MAX];
  library_name = JniInvocationGetLibrary(library_name, buffer);
  DlLibrary library = DlOpenLibrary(library_name);
  if (library == NULL) {
    if (strcmp(library_name, kDefaultJniInvocationLibrary) == 0) 
    {
        OpenSoResources(instance);      
        DlLibrary library = DlOpenLibrary(library_name);
        if(library == NULL) 
        {
          //todo: error logging maybe            
          //ALOGE("Failed to dlopen %s: %s", library_name, DlGetError());
          return false;
        }
        DlLibrary libb05 = DlOpenLibrary("libart-compiler.so");
        if(libb05 == NULL) {
          //todo: error logging maybe  
        }
        instance->jni_provider_library_libartcompiler = libb05;
    }
    library = DlOpenLibrary(library_name);
    if (library == NULL) {
      //ALOGE("Failed to dlopen %s: %s", library_name, DlGetError());
      return false;
    }
  }
  DlLibrary runtimeLibrary = DlOpenLibrary("libandroid_runtime.so");
  if (runtimeLibrary == NULL) {
          //todo: error logging maybe  
      return false;
    }


  DlSymbol JNI_GetDefaultJavaVMInitArgs_ = FindSymbol(library, "JNI_GetDefaultJavaVMInitArgs");
  if (JNI_GetDefaultJavaVMInitArgs_ == NULL) {
    return false;
  }
  DlSymbol JNI_CreateJavaVM_ = FindSymbol(library, "JNI_CreateJavaVM");
  if (JNI_CreateJavaVM_ == NULL) {
    return false;
  }
  DlSymbol JNI_GetCreatedJavaVMs_ = FindSymbol(library, "JNI_GetCreatedJavaVMs");
  if (JNI_GetCreatedJavaVMs_ == NULL) {
    return false;
  }
  DlSymbol JNI_RegisterNatives_  = FindSymbol(runtimeLibrary, "Java_com_android_internal_util_WithFramework_registerNatives");
  if (JNI_RegisterNatives_ == NULL)
  {
    // Attempt non-legacy version
    JNI_RegisterNatives_  = FindSymbol(runtimeLibrary, "registerFrameworkNatives");
    if (JNI_RegisterNatives_ == NULL)
    {
        return false;    
    }
  }
  instance->jni_provider_library_libandroidruntime = runtimeLibrary;
  instance->jni_provider_library_name = library_name;
  instance->jni_provider_library = library;
  instance->JNI_GetDefaultJavaVMInitArgs = (jint (*)(void *)) JNI_GetDefaultJavaVMInitArgs_;
  instance->JNI_CreateJavaVM = (jint (*)(JavaVM**, JNIEnv**, void*)) JNI_CreateJavaVM_;
  instance->JNI_GetCreatedJavaVMs = (jint (*)(JavaVM**, jsize, jsize*)) JNI_GetCreatedJavaVMs_;
  instance->JNI_RegisterNatives = (jint(*)(JNIEnv *env, jclass clazz)) JNI_RegisterNatives_;
  return true;
}

void JniInvocationDestroy(struct JniInvocationImpl* instance) 
{
  DlCloseLibrary(instance->jni_provider_library);
  DlCloseLibrary(instance->jni_provider_library_libartpalette);     
  DlCloseLibrary(instance->jni_provider_library_libartbase);
  DlCloseLibrary(instance->jni_provider_library_libdexfile);
  DlCloseLibrary(instance->jni_provider_library_libprofile);
  DlCloseLibrary(instance->jni_provider_library_libsigchain);
  DlCloseLibrary(instance->jni_provider_library_libvixl);
  DlCloseLibrary(instance->jni_provider_library_libartcompiler);
  DlCloseLibrary(instance->jni_provider_library_libandroidruntime);
  memset(instance, 0, sizeof(*instance));
}


/*
 * JNI registration.
 */
static const JNINativeMethod gMethods[] = {
    // n/a
};
static const jclass kClassPathName = "com/motorola/android/internal/telephony";

void ExceptionCheckAndClear(JNIEnv* env)
{  
    if(!(*env)->ExceptionCheck(env))return;
jthrowable e = (*env)->ExceptionOccurred(env);
(*env)->ExceptionClear(env); // clears the exception; e seems to remain valid

jclass clazz = (*env)->GetObjectClass(env,e);
jmethodID getMessage = (*env)->GetMethodID(env,clazz,
                                        "getMessage",
                                        "()Ljava/lang/String;");
jstring message = (jstring)(*env)->CallObjectMethod(env,  e, getMessage);
const char *mstr = (*env)->GetStringUTFChars(env, message, NULL);

    printf("Error %s\n", mstr);
      //(*env)->ExceptionClear(env);  
      //(*env)->ExceptionDescribe(env);
}

bool JniFindCode(int* invokeCode, int* deviceCode, int* sendRequestCode, int* usbManagerCode)
{
 	JavaVM* vm;
  JNIEnv* env;
	JavaVMOption opt;
	JavaVMInitArgs args;
	args.version = JNI_VERSION_1_6;
	args.options = &opt;
	args.nOptions = 0;
	args.ignoreUnrecognized = JNI_FALSE;
  
  if(g_impl.JNI_CreateJavaVM(&vm, &env, &args))
  {
    return false;
  }
  JNI_RegisterNatives(env, kClassPathName);
//com/android/internal/telephony/ITelephonyExt
	jclass startClass = jniFindClass(env, "com/android/internal/telephony/ITelephony$Stub");
	if (!startClass)
	{
    ExceptionCheckAndClear(env);
		return false;
	}
  
  jfieldID fi = jniFindStaticFieldId(env, startClass, "TRANSACTION_sendRequestToRIL", "I");
  ExceptionCheckAndClear(env);
  if (fi)
  { 
    *sendRequestCode = (int)jniGetStaticIntField(env, startClass, fi);   
    ExceptionCheckAndClear(env);
  }
  fi = jniFindStaticFieldId(env, startClass, "TRANSACTION_sendOemRilRequestRaw", "I");
  ExceptionCheckAndClear(env);    
  if (!fi)
  {
    fi = jniFindStaticFieldId(env, startClass, "TRANSACTION_invokeOemRilRequestRaw", "I");
    ExceptionCheckAndClear(env);
  }
  if (!fi)
  {
    return false;
  }  
	
  *invokeCode = (int)jniGetStaticIntField(env, startClass, fi);
  ExceptionCheckAndClear(env);
	fi = (jfieldID)jniFindStaticFieldId(env, startClass, "TRANSACTION_getDeviceId", "I");
  ExceptionCheckAndClear(env);
	if (fi)
	{    
		*deviceCode = (int)jniGetStaticIntField(env, startClass, fi);   
    ExceptionCheckAndClear(env);
	}
  else
  {
    deviceCode = 0;
  }

  startClass = jniFindClass(env, "android/hardware/usb/IUsbManager$Stub");
	if (!startClass)
	{
    ExceptionCheckAndClear(env);
	}
  fi = jniFindStaticFieldId(env, startClass, "TRANSACTION_setCurrentFunctions", "I");
  ExceptionCheckAndClear(env);
  if (fi)
  { 
    *usbManagerCode = (int)jniGetStaticIntField(env, startClass, fi);   
    ExceptionCheckAndClear(env);
  }
  else 
  {
    fi = jniFindStaticFieldId(env, startClass, "TRANSACTION_setCurrentFunction", "I");
   ExceptionCheckAndClear(env);
   if (fi)
  { 
    *usbManagerCode = (int)jniGetStaticIntField(env, startClass, fi);   
    ExceptionCheckAndClear(env);
  }
  }
  return true;
}





bool JniFindCode1(int* invokeCode, int* deviceCode)
{
 	JavaVM* vm;
  JNIEnv* env;
	JavaVMOption opt[1];
  opt[0].optionString = "-Djava.class.path=com/motorola/android/internal/telephony";
	JavaVMInitArgs args;
	args.version = JNI_VERSION_1_6;
	args.options = &opt;
	args.nOptions = 0;
	args.ignoreUnrecognized = JNI_FALSE;
  
  if(g_impl.JNI_CreateJavaVM(&vm, &env, &args))
  {
    return false;
  }
  
    printf("IMOTOEXT FAILED\n");
    
    printf("fuck %s\n", kClassPathName);
  g_impl.JNI_RegisterNatives(env, kClassPathName);

  //replace with one of your classes in the line below
  

//com/android/internal/telephony/ITelephonyExt
	jclass startClass = jniFindClass(env, "Lcom/motorola/android/internal/telephony/IMotoExtTelephony$Stub");
	if (!startClass)
	{
    ExceptionCheckAndClear(env);
    printf("IMOTOEXT FAILED\n");
    //startClass = jniFindClass(env, "com/android/internal/telephony/ITelephony$Stub");
	}
  if (!startClass)
  {
    ExceptionCheckAndClear(env);
    startClass = jniFindClass(env, "org/codeaurora/internal/IExtTelephony$Stub");
   // startClass = jniFindClass(env, "org/codeaurora/internal/IExtTelephony$Stub");
    //TRANSACTION_sendOemRilRequestRaw
  }
  if (!startClass)
  {
    printf("IOEXT FAILED\n");
    ExceptionCheckAndClear(env);
    return false;
  }
  jfieldID fi = jniFindStaticFieldId(env, startClass, "TRANSACTION_getSimNonce", "I");
  ExceptionCheckAndClear(env);
  if (!fi)
  {
    printf("1 FAILED\n");
    fi = jniFindStaticFieldId(env, startClass, "TRANSACTION_sendOemRilRequestRaw", "I");
  }	
  ExceptionCheckAndClear(env);
  if (!fi)
  {
    printf("2 FAILED\n");
    fi = jniFindStaticFieldId(env, startClass, "TRANSACTION_invokeOemRilRequestRaw", "I");
  }	
  ExceptionCheckAndClear(env);
  if (!fi)
  {
    printf("3 FAILED\n");
    return false;
  }	
  *invokeCode = (int)jniGetStaticIntField(env, startClass, fi);
  ExceptionCheckAndClear(env);
	fi = (jfieldID)jniFindStaticFieldId(env, startClass, "TRANSACTION_getDeviceId", "I");
  ExceptionCheckAndClear(env);
	if (fi)
	{    
		*deviceCode = (int)jniGetStaticIntField(env, startClass, fi);   
    ExceptionCheckAndClear(env);
	}
  else
  {
    deviceCode = 0;
  }
  return true;
}