#pragma once

#include <cstdint>
#include <Windows.h>
#include <initializer_list>
#include "cheat.h"

namespace Memory {

    template<typename T, typename... O>
    [[nodiscard]] inline T Read(std::uintptr_t base_address, T fallback, O... offsets)
    {
        std::initializer_list<std::uintptr_t> offset_chain = { static_cast<std::uintptr_t>(offsets)... };
        std::uintptr_t current_address = base_address;

        for (std::uintptr_t offset : offset_chain)
        {
            __try {
                current_address = *reinterpret_cast<std::uintptr_t*>(current_address + offset);
            }
            __except (EXCEPTION_EXECUTE_HANDLER) {
                return fallback;
            }

            if (!current_address)
                return fallback;
        }

        __try {
            return *reinterpret_cast<T*>(current_address);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            return fallback;
        }
    }

    template<typename T>
    [[nodiscard]] inline T Read(uintptr_t address)
    {
        __try {
            return *reinterpret_cast<T*>(address);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            return T{};
        }
    }

    template<typename T, typename... O>
    inline bool Write(std::uintptr_t base_address, T value, O... offsets)
    {
        std::initializer_list<std::uintptr_t> offset_chain = { static_cast<std::uintptr_t>(offsets)... };
        std::uintptr_t current_address = base_address;

        if (!offset_chain.empty())
        {
            auto it = offset_chain.begin();
            for (; it != offset_chain.end() - 1; ++it)
            {
                __try {
                    current_address = *reinterpret_cast<std::uintptr_t*>(current_address + *it);
                }
                __except (EXCEPTION_EXECUTE_HANDLER) {
                    return false;
                }

                if (!current_address)
                    return false;
            }

            current_address += *(offset_chain.end() - 1);
        }

        __try {
            *reinterpret_cast<T*>(current_address) = value;
            return true;
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            return false;
        }
    }

} 