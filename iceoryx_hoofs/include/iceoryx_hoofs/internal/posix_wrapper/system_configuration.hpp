// Copyright (c) 2019 by Robert Bosch GmbH. All rights reserved.
// Copyright (c) 2021 by Apex.AI Inc. All rights reserved.
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
#ifndef IOX_HOOFS_POSIX_WRAPPER_SYSTEM_CONFIGURATION_HPP
#define IOX_HOOFS_POSIX_WRAPPER_SYSTEM_CONFIGURATION_HPP

#include <cstdint>

namespace iox
{
namespace posix
{
/// @brief returns the page size of the system
uint64_t pageSize() noexcept;

} // namespace posix
} // namespace iox

#endif // IOX_HOOFS_POSIX_WRAPPER_SYSTEM_CONFIGURATION_HPP
