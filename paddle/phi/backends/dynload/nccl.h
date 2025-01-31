/* Copyright (c) 2016 PaddlePaddle Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */
#pragma once

#include <nccl.h>

#include <mutex>  // NOLINT

#include "paddle/phi/backends/dynload/dynamic_loader.h"
#include "paddle/phi/backends/dynload/port.h"

namespace phi {
namespace dynload {

extern std::once_flag nccl_dso_flag;
extern void* nccl_dso_handle;

#define DECLARE_DYNAMIC_LOAD_NCCL_WRAP(__name)                   \
  struct DynLoad__##__name {                                     \
    template <typename... Args>                                  \
    auto operator()(Args... args) -> decltype(__name(args...)) { \
      using nccl_func = decltype(&::__name);                     \
      std::call_once(nccl_dso_flag, []() {                       \
        nccl_dso_handle = phi::dynload::GetNCCLDsoHandle();      \
      });                                                        \
      static void* p_##__name = dlsym(nccl_dso_handle, #__name); \
      return reinterpret_cast<nccl_func>(p_##__name)(args...);   \
    }                                                            \
  };                                                             \
  extern DynLoad__##__name __name

#define NCCL_RAND_ROUTINE_EACH(__macro) \
  __macro(ncclCommInitAll);             \
  __macro(ncclGetUniqueId);             \
  __macro(ncclCommInitRank);            \
  __macro(ncclCommAbort);               \
  __macro(ncclCommDestroy);             \
  __macro(ncclCommCount);               \
  __macro(ncclCommCuDevice);            \
  __macro(ncclCommUserRank);            \
  __macro(ncclAllReduce);               \
  __macro(ncclBcast);                   \
  __macro(ncclAllGather);               \
  __macro(ncclGroupStart);              \
  __macro(ncclGroupEnd);                \
  __macro(ncclReduce);                  \
  __macro(ncclReduceScatter);           \
  __macro(ncclCommGetAsyncError);       \
  __macro(ncclGetErrorString);

NCCL_RAND_ROUTINE_EACH(DECLARE_DYNAMIC_LOAD_NCCL_WRAP)

#if NCCL_VERSION_CODE >= 2212
#define NCCL_RAND_ROUTINE_EACH_AFTER_2212(__macro) __macro(ncclBroadcast);
NCCL_RAND_ROUTINE_EACH_AFTER_2212(DECLARE_DYNAMIC_LOAD_NCCL_WRAP)
#endif

#if NCCL_VERSION_CODE >= 2304
#define NCCL_RAND_ROUTINE_EACH_AFTER_2304(__macro) __macro(ncclGetVersion);
NCCL_RAND_ROUTINE_EACH_AFTER_2304(DECLARE_DYNAMIC_LOAD_NCCL_WRAP)
#endif

#if NCCL_VERSION_CODE >= 2703
#define NCCL_RAND_ROUTINE_EACH_AFTER_2703(__macro) \
  __macro(ncclSend);                               \
  __macro(ncclRecv);
NCCL_RAND_ROUTINE_EACH_AFTER_2703(DECLARE_DYNAMIC_LOAD_NCCL_WRAP)
#endif

#if NCCL_VERSION_CODE >= 21100
#define NCCL_RAND_ROUTINE_EACH_AFTER_21100(__macro) \
  __macro(ncclRedOpCreatePreMulSum);                \
  __macro(ncclRedOpDestroy);
NCCL_RAND_ROUTINE_EACH_AFTER_21100(DECLARE_DYNAMIC_LOAD_NCCL_WRAP)
#endif

}  // namespace dynload
}  // namespace phi
