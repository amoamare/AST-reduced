#include <jni.h>
//We must include these for our JVM otherwise the JVM will fail. best to export from jni.h instead of recreating.
//They must not be renamed or mangled by C++ so we use extern c
#ifdef __cplusplus
extern "C" {
#endif
  JNIEXPORT void InitializeSignalChain() {}
  JNIEXPORT void ClaimSignalChain() {}
  JNIEXPORT void UnclaimSignalChain() {}
  JNIEXPORT void InvokeUserSignalHandler() {}
  JNIEXPORT void EnsureFrontOfChain() {}
  JNIEXPORT void AddSpecialSignalHandlerFn() {}
  JNIEXPORT void RemoveSpecialSignalHandlerFn() {}
  JNIEXPORT void SetSpecialSignalHandlerFn() {}
#ifdef __cplusplus
}
#endif
