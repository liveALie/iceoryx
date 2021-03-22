// Copyright (c) 2019, 2021 by Robert Bosch GmbH. All rights reserved.
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
#ifndef IOX_POSH_ROUDI_PROCESS_MANAGER_HPP
#define IOX_POSH_ROUDI_PROCESS_MANAGER_HPP

#include "iceoryx_posh/internal/mepoo/segment_manager.hpp"
#include "iceoryx_posh/internal/roudi/introspection/process_introspection.hpp"
#include "iceoryx_posh/internal/roudi/port_manager.hpp"
#include "iceoryx_posh/internal/roudi/process.hpp"
#include "iceoryx_posh/internal/runtime/ipc_interface_user.hpp"
#include "iceoryx_posh/mepoo/chunk_header.hpp"
#include "iceoryx_posh/version/compatibility_check_level.hpp"
#include "iceoryx_posh/version/version_info.hpp"
#include "iceoryx_utils/cxx/list.hpp"
#include "iceoryx_utils/error_handling/error_handling.hpp"
#include "iceoryx_utils/posix_wrapper/posix_access_rights.hpp"

#include <cstdint>
#include <ctime>

namespace iox
{
namespace roudi
{
class ProcessManagerInterface
{
  public:
    virtual void sendServiceRegistryChangeCounterToProcess(const ProcessName_t& process_name) noexcept = 0;
    virtual void discoveryUpdate() noexcept = 0;

    virtual ~ProcessManagerInterface() noexcept = default;
};

class ProcessManager : public ProcessManagerInterface
{
  public:
    using ProcessList_t = cxx::list<Process, MAX_PROCESS_NUMBER>;
    using PortConfigInfo = iox::runtime::PortConfigInfo;

    ProcessManager(RouDiMemoryInterface& roudiMemoryInterface,
                   PortManager& portManager,
                   const version::CompatibilityCheckLevel compatibilityCheckLevel) noexcept;
    virtual ~ProcessManager() noexcept override = default;

    ProcessManager(const ProcessManager& other) = delete;
    ProcessManager& operator=(const ProcessManager& other) = delete;

    /// @brief Registers a process at the ProcessManager
    /// @param [in] name of the process which wants to register
    /// @param [in] pid is the host system process id
    /// @param [in] user is the posix user id to which the process belongs
    /// @param [in] isMonitored indicates if the process should be monitored for being alive
    /// @param [in] transmissionTimestamp is an ID for the application to check for the expected response
    /// @param [in] sessionId is an ID generated by RouDi to prevent sending outdated IPC channel transmission
    /// @param [in] versionInfo Version of iceoryx used
    /// @return false if process was already registered, true otherwise
    bool registerProcess(const ProcessName_t& name,
                         const uint32_t pid,
                         const posix::PosixUser user,
                         const bool isMonitored,
                         const int64_t transmissionTimestamp,
                         const uint64_t sessionId,
                         const version::VersionInfo& versionInfo) noexcept;

    /// @brief Unregisters a process at the ProcessManager
    /// @param [in] name of the process which wants to unregister
    /// @return true if known process was unregistered, false if process is unknown
    bool unregisterProcess(const ProcessName_t& name) noexcept;

    /// @brief Kills all registered processes. If RouDi doesn't have sufficient rights to kill the process, the
    /// process is considered killed.
    void killAllProcesses() noexcept;

    /// @brief Informs the user about the processes which are registered and then clears the process list
    void printWarningForRegisteredProcessesAndClearProcessList() noexcept;

    /// @brief Is one or more of the registered processes running?
    /// @return true if one or more of the registered processes is running, false otherwise
    bool isAnyRegisteredProcessStillRunning() noexcept;

    /// @brief Tries to gracefully terminate all registered processes
    void requestShutdownOfAllProcesses() noexcept;

    void updateLivelinessOfProcess(const ProcessName_t& name) noexcept;

    void findServiceForProcess(const ProcessName_t& name, const capro::ServiceDescription& service) noexcept;

    void
    addInterfaceForProcess(const ProcessName_t& name, capro::Interfaces interface, const NodeName_t& node) noexcept;

    void addApplicationForProcess(const ProcessName_t& name) noexcept;

    void addNodeForProcess(const ProcessName_t& process, const NodeName_t& node) noexcept;

    void addSubscriberForProcess(const ProcessName_t& name,
                                 const capro::ServiceDescription& service,
                                 const popo::SubscriberOptions& subscriberOptions,
                                 const PortConfigInfo& portConfigInfo = PortConfigInfo()) noexcept;

    void addPublisherForProcess(const ProcessName_t& name,
                                const capro::ServiceDescription& service,
                                const popo::PublisherOptions& publisherOptions,
                                const PortConfigInfo& portConfigInfo = PortConfigInfo()) noexcept;

    void addConditionVariableForProcess(const ProcessName_t& processName) noexcept;

    void initIntrospection(ProcessIntrospectionType* processIntrospection) noexcept;

    void run() noexcept;

    popo::PublisherPortData* addIntrospectionPublisherPort(const capro::ServiceDescription& service,
                                                           const ProcessName_t& process_name) noexcept;

    /// @brief Notify the application that it sent an unsupported message
    void sendMessageNotSupportedToRuntime(const ProcessName_t& name) noexcept;

    void sendServiceRegistryChangeCounterToProcess(const ProcessName_t& process_name) noexcept override;

  private:
    bool searchForProcessAndThen(const ProcessName_t& name,
                                 cxx::function_ref<void(Process&)> AndThenCallable,
                                 cxx::function_ref<void()> OrElseCallable) noexcept;

    void monitorProcesses() noexcept;
    void discoveryUpdate() noexcept override;

    /// @param [in] name of the process; this is equal to the IPC channel name, which is used for communication
    /// @param [in] pid is the host system process id
    /// @param [in] payloadMemoryManager is a pointer to the payload memory manager for this process
    /// @param [in] isMonitored indicates if the process should be monitored for being alive
    /// @param [in] transmissionTimestamp is an ID for the application to check for the expected response
    /// @param [in] payloadSegmentId is an identifier for the shm payload segment
    /// @param [in] sessionId is an ID generated by RouDi to prevent sending outdated IPC channel transmission
    /// @param [in] versionInfo Version of iceoryx used
    /// @return Returns if the process could be added successfully.
    bool addProcess(const ProcessName_t& name,
                    const uint32_t pid,
                    cxx::not_null<mepoo::MemoryManager* const> payloadMemoryManager,
                    const bool isMonitored,
                    const int64_t transmissionTimestamp,
                    const uint64_t payloadSegmentId,
                    const uint64_t sessionId,
                    const version::VersionInfo& versionInfo) noexcept;

    /// @brief Removes the process from the managed client process list, identified by its id.
    /// @param [in] name The process name which should be removed.
    /// @return Returns true if the process was found and removed from the internal list.
    bool searchForProcessAndRemoveIt(const ProcessName_t& name) noexcept;

    /// @brief Removes the given process from the managed client process list and the respective resources in shared
    /// memory
    /// @param [in] processIter The process which should be removed.
    /// @return Returns true if the process was found and removed from the internal list.
    bool removeProcessAndDeleteRespectiveSharedMemoryObjects(ProcessList_t::iterator& processIter) noexcept;

    enum class ShutdownPolicy
    {
        SIG_TERM,
        SIG_KILL
    };

    /// @brief Shuts down the given process in m_processList with the given signal.
    /// @param [in] process The process to shut down.
    /// @param [in] shutdownPolicy Signal passed to the system to shut down the process
    /// @param [in] shutdownLog Defines the logging detail.
    /// @return Returns true if the sent signal was successful.
    bool requestShutdownOfProcess(Process& process, ShutdownPolicy shutdownPolicy) noexcept;

    /// @brief Evaluates with a kill SIGTERM signal to a process if he is still alive.
    /// @param [in] process The process to check.
    /// @return Returns true if the process is still alive, otherwise false.
    bool isProcessAlive(const Process& process) noexcept;

    /// @brief Evaluates eventual upcoming errors from kill() command in requestShutdownOfProcess
    /// Calls the errorhandler.
    /// @param [in] process process where the kill command was run on
    /// @param [in] errnum errorcode of the killcommand
    /// @param [in] errorString errorstring of the killcommand
    /// @param [in] shutdownPolicy enum which tells what termination command was used (e.g. SIGTERM)
    void evaluateKillError(const Process& process,
                           const int32_t& errnum,
                           const char* errorString,
                           ShutdownPolicy shutdownPolicy) noexcept;

    RouDiMemoryInterface& m_roudiMemoryInterface;
    PortManager& m_portManager;
    mepoo::SegmentManager<>* m_segmentManager{nullptr};
    mepoo::MemoryManager* m_introspectionMemoryManager{nullptr};
    RelativePointer::id_t m_mgmtSegmentId{RelativePointer::NULL_POINTER_ID};
    ProcessList_t m_processList;
    ProcessIntrospectionType* m_processIntrospection{nullptr};
    /// @brief is currently used for the internal publisher/subscriber ports
    mepoo::MemoryManager* m_memoryManagerOfCurrentProcess{nullptr};
    version::CompatibilityCheckLevel m_compatibilityCheckLevel;
};

} // namespace roudi
} // namespace iox

#endif // IOX_POSH_ROUDI_PROCESS_MANAGER_HPP
