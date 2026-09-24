#pragma once

#define _CRT_SECURE_NO_WARNINGS 1
// IDA Update: 9.4 - __IDP__ no longer used by the SDK headers, removed
#define __NT__                  1
#define __X64__                 1

#define plugin_name "IDA RPC"

#include <ida.hpp>
#include <idp.hpp>
#include <frame.hpp>
#include <expr.hpp>
#include <name.hpp>
#include <segment.hpp>
#include <bytes.hpp>
#include <loader.hpp>
#include <kernwin.hpp>
#include <auto.hpp>
#include <entry.hpp>
#include <typeinf.hpp>
#include <demangle.hpp>
#include <allins.hpp>
#include <search.hpp>
#include <ua.hpp>
#include <fpro.h>
#include <diskio.hpp>
