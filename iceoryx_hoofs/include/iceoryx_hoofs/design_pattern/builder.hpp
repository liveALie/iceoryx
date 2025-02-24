// Copyright (c) 2022 by Apex.AI Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0

#ifndef IOX_HOOFS_DESIGN_PATTERN_BUILDER_HPP
#define IOX_HOOFS_DESIGN_PATTERN_BUILDER_HPP

/// @brief Macro which generates a setter method useful for a builder pattern.
/// @param[in] type the data type of the parameter
/// @param[in] name the name of the parameter
/// @param[in] defaultValue the default value of the parameter
/// @code
///   class MyBuilder {
///     IOX_BUILDER_PARAMETER(TypeA, NameB, ValueC)
///     // START generates the following code
///     public:
///       decltype(auto) NameB(TypeA const& value) &&
///       {
///           m_NameB = value;
///           return std::move(*this);
///       }
///
///       decltype(auto) NameB(TypeA&& value) &&
///       {
///           m_NameB = std::move(value);
///           return std::move(*this);
///       }
///
///     private:
///       TypeA m_NameB = ValueC;
///     // END
///   };
/// @endcode
// NOLINTJUSTIFICATION brackets around macro parameter would lead in this case to compile time failures
// NOLINTBEGIN(bugprone-macro-parentheses)
// NOLINTJUSTIFICATION cannot be realized with templates or constexpr functions
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define IOX_BUILDER_PARAMETER(type, name, defaultValue)                                                                \
  public:                                                                                                              \
    decltype(auto) name(type const& value)&&                                                                           \
    {                                                                                                                  \
        m_##name = value;                                                                                              \
        return std::move(*this);                                                                                       \
    }                                                                                                                  \
                                                                                                                       \
    decltype(auto) name(type&& value)&&                                                                                \
    {                                                                                                                  \
        m_##name = std::move(value);                                                                                   \
        return std::move(*this);                                                                                       \
    }                                                                                                                  \
                                                                                                                       \
  private:                                                                                                             \
    type m_##name{defaultValue};
// NOLINTEND(bugprone-macro-parentheses)

#endif
