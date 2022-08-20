#pragma once

#define DELEGATE_NAME                Delegate
#define DELEGATE_INVOKE_BY_REFERENCE 1
#define DELEGATE_SUPPORT_STATIC      0
#define DELEGATE_SUPPORT_NONSTATIC   1
#include "delegatebase.h"

#define DELEGATE_NAME                StaticDelegate
#define DELEGATE_INVOKE_BY_REFERENCE 1
#define DELEGATE_SUPPORT_STATIC      1
#define DELEGATE_SUPPORT_NONSTATIC   0
#include "delegatebase.h"

#define DELEGATE_NAME                UniversalDelegate
#define DELEGATE_INVOKE_BY_REFERENCE 1
#define DELEGATE_SUPPORT_STATIC      1
#define DELEGATE_SUPPORT_NONSTATIC   1
#include "delegatebase.h"