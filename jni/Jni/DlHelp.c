
/*
 * Copyright (C) 2020 The Android Open Source Project
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
#include "DlHelp.h"
#include <stdbool.h>
#include <dlfcn.h>

DlLibrary DlOpenLibrary(const char* filename) {

  // Load with RTLD_NODELETE in order to ensure that libart.so is not unmapped when it is closed.
  // This is due to the fact that it is possible that some threads might have yet to finish
  // exiting even after JNI_DeleteJavaVM returns, which can lead to segfaults if the library is
  // unloaded.
  return dlopen(filename, RTLD_NOW);
}
bool DlCloseLibrary(DlLibrary library) {

  return (dlclose(library) == 0);
}
DlSymbol DlGetSymbol(DlLibrary handle, const char* symbol) {

  return dlsym(handle, symbol);
}
const char* DlGetError() {
  return dlerror();
}
