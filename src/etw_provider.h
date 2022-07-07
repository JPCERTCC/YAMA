#ifndef SRC_ETW_PROVIDER_H
#define SRC_ETW_PROVIDER_H

#include "common.h"
namespace yama {

bool IsGuidInEtwProviders(const GUID& guid);

}  // namespace yama
#endif  // SRC_ETW_PROVIDER_H
