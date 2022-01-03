/* Copyright 2022 Axel Huebl, Benjamin Worpitz, Matthias Werner, Jan Stephan
 *
 * This file is part of alpaka.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <alpaka/core/Common.hpp>
#include <alpaka/core/Unused.hpp>

#include <cassert>
#include <type_traits>


#define ALPAKA_ASSERT(EXPRESSION) assert(EXPRESSION)

#ifdef ALPAKA_DEBUG_OFFLOAD_ASSUME_HOST
#    define ALPAKA_ASSERT_OFFLOAD(EXPRESSION) ALPAKA_ASSERT(EXPRESSION)
#elif defined __AMDGCN__ && (!defined NDEBUG)
#    define ALPAKA_ASSERT_OFFLOAD(EXPRESSION)                                                                         \
        do                                                                                                            \
        {                                                                                                             \
            if(!(EXPRESSION))                                                                                         \
                __builtin_trap();                                                                                     \
        } while(false)
#else
#    define ALPAKA_ASSERT_OFFLOAD(EXPRESSION)
#endif

namespace alpaka
{
    namespace core
    {
        namespace detail
        {
            template<typename TArg>
            struct AssertValueUnsigned
            {
                ALPAKA_NO_HOST_ACC_WARNING ALPAKA_FN_HOST_ACC static auto assertValueUnsigned(TArg const& arg)
                {
                    alpaka::ignore_unused(arg);
                    if constexpr(std::is_signed_v<TArg>)
                        ALPAKA_ASSERT_OFFLOAD(arg >= 0);

                    // Nothing to do for unsigned types.
                }
            };
        } // namespace detail

        //! This method checks integral values if they are greater or equal zero.
        //! The implementation prevents warnings for checking this for unsigned types.
        ALPAKA_NO_HOST_ACC_WARNING
        template<typename TArg>
        ALPAKA_FN_HOST_ACC auto assertValueUnsigned(TArg const& arg) -> void
        {
            detail::AssertValueUnsigned<TArg>::assertValueUnsigned(arg);
        }

        namespace detail
        {
            template<typename TLhs, typename TRhs>
            struct AssertGreaterThan
            {
                ALPAKA_NO_HOST_ACC_WARNING ALPAKA_FN_HOST_ACC static auto assertGreaterThan(TRhs const& rhs)
                {
                    alpaka::ignore_unused(rhs);
                    if constexpr(std::is_signed_v<TRhs> || (TLhs::value != 0u))
                        ALPAKA_ASSERT_OFFLOAD(TLhs::value > rhs);

                    // Nothing to do for unsigned types comparing to zero.
                }
            };
        } // namespace detail

        //! This function asserts that the integral value TLhs is greater than TRhs.
        ALPAKA_NO_HOST_ACC_WARNING
        template<typename TLhs, typename TRhs>
        ALPAKA_FN_HOST_ACC auto assertGreaterThan(TRhs const& rhs) -> void
        {
            detail::AssertGreaterThan<TLhs, TRhs>::assertGreaterThan(rhs);
        }
    } // namespace core
} // namespace alpaka
