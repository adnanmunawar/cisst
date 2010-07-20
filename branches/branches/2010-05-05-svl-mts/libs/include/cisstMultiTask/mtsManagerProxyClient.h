/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Min Yang Jung
  Created on: 2010-01-20

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsManagerProxyClient_h
#define _mtsManagerProxyClient_h

#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsManagerProxy.h>
#include <cisstMultiTask/mtsProxyBaseClient.h>
#include <cisstMultiTask/mtsManagerGlobalInterface.h>

#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsManagerProxyClient :
    public mtsProxyBaseClient<mtsManagerLocal>, public mtsManagerGlobalInterface
{

    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

    /*! Typedef for base type. */
    typedef mtsProxyBaseClient<mtsManagerLocal> BaseClientType;

    /*! Typedef for connected server proxy. */
    typedef mtsManagerProxy::ManagerServerPrx ManagerServerProxyType;
    ManagerServerProxyType ManagerServerProxy;

protected:
    /*! Definitions for send thread */
    class ManagerClientI;
    typedef IceUtil::Handle<ManagerClientI> ManagerClientIPtr;
    ManagerClientIPtr Sender;

    /*! Instance counter used to set a short name of this thread */
    static unsigned int InstanceCounter;

    //-------------------------------------------------------------------------
    //  Proxy Implementation
    //-------------------------------------------------------------------------
    /*! Create a proxy object and a send thread. */
    void CreateProxy() {
        ManagerServerProxy =
            mtsManagerProxy::ManagerServerPrx::checkedCast(ProxyObject);
        if (!ManagerServerProxy) {
            throw "mtsManagerProxyClient: Invalid proxy";
        }

        Sender = new ManagerClientI(IceCommunicator, IceLogger, ManagerServerProxy, this);
    }

    /*! Create a proxy object and a send thread. */
    void RemoveProxy() {
        Sender->Stop();
    }

    /*! Start a send thread and wait for shutdown (blocking call). */
    void StartClient();

    /*! Called when server disconnection is detected */
    bool OnServerDisconnect();

    /*! Thread runner */
    static void Runner(ThreadArguments<mtsManagerLocal> * arguments);

    /*! Miscellaneous methods */
    void GetConnectionStringSet(mtsManagerProxy::ConnectionStringSet & connectionStringSet,
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceProvidedName,
        const std::string & requestProcessName = "");

    //-------------------------------------------------------------------------
    //  Event Handlers : Server -> Client
    //-------------------------------------------------------------------------
    void ReceiveTestMessageFromServerToClient(const std::string & str) const;

    // Proxy object control (creation and removal)
    bool ReceiveCreateComponentProxy(const std::string & componentProxyName);
    bool ReceiveRemoveComponentProxy(const std::string & componentProxyName);
    bool ReceiveCreateInterfaceProvidedProxy(const std::string & serverComponentProxyName, const ::mtsManagerProxy::InterfaceProvidedDescription & providedInterfaceDescription);
    bool ReceiveCreateInterfaceRequiredProxy(const std::string & clientComponentProxyName, const ::mtsManagerProxy::InterfaceRequiredDescription & requiredInterfaceDescription);
    bool ReceiveRemoveInterfaceProvidedProxy(const std::string & clientComponentProxyName, const std::string & providedInterfaceProxyName);
    bool ReceiveRemoveInterfaceRequiredProxy(const std::string & serverComponentProxyName, const std::string & requiredInterfaceProxyName);

    // Connection management
    bool ReceiveConnectServerSideInterface(const unsigned int connectionID, const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet);
    bool ReceiveConnectClientSideInterface(const unsigned int connectionID, const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet);

    // Getters
    bool ReceiveGetInterfaceProvidedDescription(const std::string & serverComponentName, const std::string & providedInterfaceName,
                                                ::mtsManagerProxy::InterfaceProvidedDescription & providedInterfaceDescription);
    bool ReceiveGetInterfaceRequiredDescription(const std::string & componentName, const std::string & requiredInterfaceName,
                                                ::mtsManagerProxy::InterfaceRequiredDescription & requiredInterfaceDescription);

    // Getters for component inspector
    void ReceiveGetNamesOfCommands(const std::string & componentName, const std::string & providedInterfaceName,
                                   ::mtsManagerProxy::NamesOfCommandsSequence & names) const;
    void ReceiveGetNamesOfEventGenerators(const std::string & componentName, const std::string & providedInterfaceName,
                                          ::mtsManagerProxy::NamesOfEventGeneratorsSequence & names) const;
    void ReceiveGetNamesOfFunctions(const std::string & componentName, const std::string & requiredInterfaceName,
                                    ::mtsManagerProxy::NamesOfFunctionsSequence & names) const;
    void ReceiveGetNamesOfEventHandlers(const std::string & componentName, const std::string & requiredInterfaceName,
                                        ::mtsManagerProxy::NamesOfEventHandlersSequence & names) const;

    void ReceiveGetDescriptionOfCommand(const std::string & componentName, const std::string & providedInterfaceName,
                                        const std::string & commandName, std::string & description) const;
    void ReceiveGetDescriptionOfEventGenerator(const std::string & componentName, const std::string & providedInterfaceName,
                                               const std::string & eventGeneratorName, std::string & description) const;
    void ReceiveGetDescriptionOfFunction(const std::string & componentName, const std::string & requiredInterfaceName,
                                         const std::string & functionName, std::string & description) const;
    void ReceiveGetDescriptionOfEventHandler(const std::string & componentName, const std::string & requiredInterfaceName,
                                             const std::string & eventHandlerName, std::string & description) const;

    // Getters for data visualization
    void ReceiveGetArgumentInformation(const std::string & componentName, const std::string & providedInterfaceName,
                                       const std::string & commandName, std::string & argumentName, ::mtsManagerProxy::NamesOfSignals & signalNames) const;
    void ReceiveGetValuesOfCommand(const std::string & componentName, const std::string & providedInterfaceName,
                                   const std::string & commandName, const int scalarIndex, ::mtsManagerProxy::SetOfValues & values) const;

    std::string ReceiveGetProcessName();
    ::Ice::Int ReceiveGetCurrentInterfaceCount(const std::string & componentName);

public:
    /*! Constructor and destructor */
    mtsManagerProxyClient(const std::string & serverEndpointInfo);
    ~mtsManagerProxyClient();

    /*! Entry point to run a proxy. */
    bool Start(mtsManagerLocal * proxyOwner);

    /*! Stop the proxy (clean up thread-related resources) */
    void Stop(void);

    //-------------------------------------------------------------------------
    //  Implementation of mtsManagerGlobalInterface
    //  (See mtsManagerGlobalInterface.h for details)
    //-------------------------------------------------------------------------
    //  Process Management
    bool AddProcess(const std::string & processName);
    bool FindProcess(const std::string & processName) const;
    bool RemoveProcess(const std::string & processName);

    //  Component Management
    bool AddComponent(const std::string & processName, const std::string & componentName);
    bool FindComponent(const std::string & processName, const std::string & componentName) const;
    bool RemoveComponent(const std::string & processName, const std::string & componentName);

    //  Interface Management
    bool AddInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName,
                                      const std::string & interfaceName, const bool isProxyInterface);
    bool AddInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName,
                                     const std::string & interfaceName, const bool isProxyInterface);
    bool FindInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName,
                                       const std::string & interfaceName) const;
    bool FindInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName,
                                      const std::string & interfaceName) const;
    bool RemoveInterfaceProvided(const std::string & processName, const std::string & componentName,
                                 const std::string & interfaceName);
    bool RemoveInterfaceRequired(const std::string & processName, const std::string & componentName,
                                 const std::string & interfaceName);

    //  Connection Management
    int Connect(const std::string & requestProcessName,
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceProvidedName);
    bool ConnectConfirm(unsigned int connectionSessionID);
    bool Disconnect(
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceProvidedName);

    //  Networking
    bool SetInterfaceProvidedProxyAccessInfo(
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceProvidedName,
        const std::string & endpointInfo);
    bool GetInterfaceProvidedProxyAccessInfo(
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceProvidedName,
        std::string & endpointInfo);
    bool InitiateConnect(const unsigned int connectionID,
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceProvidedName);
    bool ConnectServerSideInterfaceRequest(const unsigned int connectionID,
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceProvidedName);
    void GetListOfConnections(std::vector<ConnectionStrings> & list) const;

    //-------------------------------------------------------------------------
    //  Event Generators (Event Sender) : Client -> Server
    //-------------------------------------------------------------------------
    void SendTestMessageFromClientToServer(const std::string & str) const;

    // Process Management
    bool SendAddProcess(const std::string & processName);
    bool SendFindProcess(const std::string & processName);
    bool SendRemoveProcess(const std::string & processName);

    // Component Management
    bool SendAddComponent(const std::string & processName, const std::string & componentName);
    bool SendFindComponent(const std::string & processName, const std::string & componentName);
    bool SendRemoveComponent(const std::string & processName, const std::string & componentName);

    bool SendAddInterfaceProvided(const std::string & processName, const std::string & componentName, const std::string & interfaceName, bool isProxyInterface);
    bool SendFindInterfaceProvided(const std::string & processName, const std::string & componentName, const std::string & interfaceName);
    bool SendRemoveInterfaceProvided(const std::string & processName, const std::string & componentName, const std::string & interfaceName);

    bool SendAddInterfaceRequired(const std::string & processName, const std::string & componentName, const std::string & interfaceName, bool isProxyInterface);
    bool SendFindInterfaceRequired(const std::string & processName, const std::string & componentName, const std::string & interfaceName);
    bool SendRemoveInterfaceRequired(const std::string & processName, const std::string & componentName, const std::string & interfaceName);

    // Connection Management
    ::Ice::Int SendConnect(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet);
    bool SendConnectConfirm(::Ice::Int connectionSessionID);
    bool SendDisconnect(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet);

    // Networking
    bool SendSetInterfaceProvidedProxyAccessInfo(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, const std::string & endpointInfo);
    bool SendGetInterfaceProvidedProxyAccessInfo(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, std::string & endpointInfo);
    bool SendInitiateConnect(::Ice::Int connectionID, const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet);
    bool SendConnectServerSideInterfaceRequest(const unsigned int connectionID, const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet);

    //-------------------------------------------------------------------------
    //  Definition by mtsInterfaceProvidedProxy.ice
    //-------------------------------------------------------------------------
protected:
    class ManagerClientI :
        public mtsManagerProxy::ManagerClient, public IceUtil::Monitor<IceUtil::Mutex>
    {
    private:
        /*! Ice objects */
        Ice::CommunicatorPtr Communicator;
        IceUtil::ThreadPtr SenderThreadPtr;
        Ice::LoggerPtr IceLogger;

        /*! Network event processor */
        mtsManagerProxyClient * ManagerProxyClient;

        /*! Connected server proxy */
        mtsManagerProxy::ManagerServerPrx Server;

    public:
        /*! Constructor and destructor */
        ManagerClientI(
            const Ice::CommunicatorPtr& communicator,
            const Ice::LoggerPtr& logger,
            const mtsManagerProxy::ManagerServerPrx& server,
            mtsManagerProxyClient * ManagerClient);
        ~ManagerClientI();

        /*! Proxy management */
        void Start();
        void Run();
        void Stop();

        /*! Getter */
        bool IsActiveProxy() const {
            if (ManagerProxyClient) {
                return ManagerProxyClient->IsActiveProxy();
            } else {
                return false;
            }
        }

        //-------------------------------------------------
        //  Event handlers (Server -> Client)
        //-------------------------------------------------
        /*! Test method */
        void TestMessageFromServerToClient(const std::string & str, const ::Ice::Current & current);

        /*! Proxy object control (creation and removal) */
        bool CreateComponentProxy(const std::string & componentProxyName, const ::Ice::Current & current);
        bool RemoveComponentProxy(const std::string & componentProxyName, const ::Ice::Current & current);
        bool CreateInterfaceProvidedProxy(const std::string & serverComponentProxyName, const ::mtsManagerProxy::InterfaceProvidedDescription & providedInterfaceDescription, const ::Ice::Current & current);
        bool CreateInterfaceRequiredProxy(const std::string & clientComponentProxyName, const ::mtsManagerProxy::InterfaceRequiredDescription & requiredInterfaceDescription, const ::Ice::Current & current);
        bool RemoveInterfaceProvidedProxy(const std::string & clientComponentProxyName, const std::string & providedInterfaceProxyName, const ::Ice::Current & current);
        bool RemoveInterfaceRequiredProxy(const std::string & serverComponentProxyName, const std::string & requiredInterfaceProxyName, const ::Ice::Current & current);

        /*! Connection management */
        bool ConnectServerSideInterface(::Ice::Int connectionID, const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, const ::Ice::Current & current);
        bool ConnectClientSideInterface(::Ice::Int connectionID, const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, const ::Ice::Current & current);

        /*! Getters */
        bool GetInterfaceProvidedDescription(const std::string & serverComponentName, const std::string & providedInterfaceName, ::mtsManagerProxy::InterfaceProvidedDescription & providedInterfaceDescription, const ::Ice::Current &) const;
        bool GetInterfaceRequiredDescription(const std::string & componentName, const std::string & requiredInterfaceName, ::mtsManagerProxy::InterfaceRequiredDescription & requiredInterfaceDescription, const ::Ice::Current &) const;

        /*! Getters for component inspector */
        void GetNamesOfCommands(const std::string & componentName, const std::string & providedInterfaceName, ::mtsManagerProxy::NamesOfCommandsSequence & names, const ::Ice::Current & current) const;
        void GetNamesOfEventGenerators(const std::string & componentName, const std::string & providedInterfaceName, ::mtsManagerProxy::NamesOfEventGeneratorsSequence  & names, const ::Ice::Current & current) const;
        void GetNamesOfFunctions(const std::string & componentName, const std::string & requiredInterfaceName, ::mtsManagerProxy::NamesOfFunctionsSequence & names, const ::Ice::Current & current) const;
        void GetNamesOfEventHandlers(const std::string & componentName, const std::string & requiredInterfaceName, ::mtsManagerProxy::NamesOfEventHandlersSequence & names, const ::Ice::Current & current) const;

        void GetDescriptionOfCommand(const std::string & componentName, const std::string & providedInterfaceName, const std::string & commandName, std::string & description, const ::Ice::Current & current) const;
        void GetDescriptionOfEventGenerator(const std::string & componentName, const std::string & providedInterfaceName, const std::string & eventGeneratorName, std::string & description, const ::Ice::Current & current) const;
        void GetDescriptionOfFunction(const std::string & componentName, const std::string & requiredInterfaceName, const std::string & eventGeneratorName, std::string & description, const ::Ice::Current & current) const;
        void GetDescriptionOfEventHandler(const std::string & componentName, const std::string & requiredInterfaceName, const std::string & eventHandlerName, std::string & description, const ::Ice::Current & current) const;

        /*! Getters for data visualiation */
        void GetArgumentInformation(const std::string & componentName, const std::string & providedInterfaceName, const std::string & commandName, std::string & argumentName, ::mtsManagerProxy::NamesOfSignals & signalNames, const ::Ice::Current & current) const;
        void GetValuesOfCommand(const std::string & componentName, const std::string & providedInterfaceName, const std::string & commandName, int scalarIndex, ::mtsManagerProxy::SetOfValues & values, const ::Ice::Current & current) const;

        std::string GetProcessName(const ::Ice::Current &) const;
        ::Ice::Int GetTotalNumberOfInterfaces(const std::string & componentName, const ::Ice::Current &) const;
    };
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsManagerProxyClient)

#endif // _mtsManagerProxyClient_h
