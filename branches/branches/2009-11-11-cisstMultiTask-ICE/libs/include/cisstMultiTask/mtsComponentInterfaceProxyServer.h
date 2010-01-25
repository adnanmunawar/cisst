/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsComponentInterfaceProxyServer.h 142 2009-03-11 23:02:34Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2010-01-12

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsComponentInterfaceProxyServer_h
#define _mtsComponentInterfaceProxyServer_h

#include <cisstMultiTask/mtsComponentProxy.h>
#include <cisstMultiTask/mtsComponentInterfaceProxy.h>
#include <cisstMultiTask/mtsProxyBaseServer.h>

#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsComponentInterfaceProxyServer : 
    public mtsProxyBaseServer<mtsComponentProxy, 
                              mtsComponentInterfaceProxy::ComponentInterfaceClientPrx,
                              unsigned int>
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

    /*! Typedef for client proxy type */
    typedef mtsComponentInterfaceProxy::ComponentInterfaceClientPrx ComponentInterfaceClientProxyType;

    /*! Typedef for base type */
    typedef mtsProxyBaseServer<mtsComponentProxy, ComponentInterfaceClientProxyType, unsigned int> BaseServerType;

public:
    mtsComponentInterfaceProxyServer(
        const std::string & adapterName, const std::string & endpointInfo, const std::string & communicatorID)
        : BaseServerType(adapterName, endpointInfo, communicatorID)
    {}

    ~mtsComponentInterfaceProxyServer();

    /*! Entry point to run a proxy. */
    bool Start(mtsComponentProxy * owner);

    /*! Stop the proxy (clean up thread-related resources) */
    void Stop();

protected:
    /*! Definitions for send thread */
    class ComponentInterfaceServerI;
    typedef IceUtil::Handle<ComponentInterfaceServerI> ComponentInterfaceServerIPtr;
    ComponentInterfaceServerIPtr Sender;

    //-------------------------------------------------------------------------
    //  Proxy Implementation
    //-------------------------------------------------------------------------
    /*! Create a servant */
    Ice::ObjectPtr CreateServant() {
        Sender = new ComponentInterfaceServerI(IceCommunicator, IceLogger, this);
        return Sender;
    }
    
    /*! Start a send thread and wait for shutdown (this is a blocking method). */
    void StartServer();

    /*! Resource clean-up when a client disconnects or is disconnected.
        TODO: add session
        TODO: add resource clean up
        TODconnectionID,O: review/add safe termination  */
    void OnClose();

    /*! Thread runner */
    static void Runner(ThreadArguments<mtsComponentProxy> * arguments);

    /*! Get a network proxy client object using clientID. If no network proxy 
        client with the clientID didn't connect or the proxy is not active,
        this method returns NULL. */
    ComponentInterfaceClientProxyType * GetNetworkProxyClient(const ClientIDType clientID);

    //-------------------------------------------------------------------------
    //  Event Handlers (Client -> Server)
    //-------------------------------------------------------------------------
    void ReceiveTestMessageFromClientToServer(const ConnectionIDType &connectionID, const std::string & str);

    /*! When a new client connects, add it to the client management list. */
    bool ReceiveAddClient(const ConnectionIDType & connectionID, 
                          const std::string & connectingProxyName, 
                          const unsigned int providedInterfaceProxyInstanceId, 
                          ComponentInterfaceClientProxyType & clientProxy);

    bool ReceiveFetchEventGeneratorProxyPointers(const ConnectionIDType & connectionID, 
                                                 const std::string & clientComponentName, 
                                                 const std::string & requiredInterfaceName,
                                                 mtsComponentInterfaceProxy::ListsOfEventGeneratorsRegistered & eventGeneratorProxyPointers);

    //-------------------------------------------------------------------------
    //  Event Generators (Event Sender) : Server -> Client
    //-------------------------------------------------------------------------
public:
    /*! Communicator (proxy) ID for communication between component interface
        server and component interface client */
    static std::string InterfaceCommunicatorID;

    /*! Test method: broadcast string to all clients connected */
    void SendTestMessageFromServerToClient(const std::string & str);

    /*! Fetch function proxy pointers from the connected required interface 
        proxy at server process. */
    bool SendFetchFunctionProxyPointers(
        const ClientIDType clientID, const std::string & requiredInterfaceName, 
        mtsComponentInterfaceProxy::FunctionProxyPointerSet & functionProxyPointers);

    /*! Execute commands. This will call function proxies in the required 
        interface proxy at server process. 
        clientID designates which network proxy client should execute a command 
        and commandID represents which function proxy object should be called. */
    bool SendExecuteCommandVoid(const ClientIDType clientID, const CommandIDType commandID);

    void SendExecuteCommandWriteSerialized(const ClientIDType clientID, const CommandIDType commandID, const mtsGenericObject & argument);

    void SendExecuteCommandReadSerialized(const ClientIDType clientID, const CommandIDType commandID, mtsGenericObject & argument);

    void SendExecuteCommandQualifiedReadSerialized(const ClientIDType clientID, const CommandIDType commandID, const mtsGenericObject & argument1, mtsGenericObject & argument2);

    //-------------------------------------------------------------------------
    //  Definition by mtsComponentInterfaceProxy.ice
    //-------------------------------------------------------------------------
protected:
    class ComponentInterfaceServerI : 
        public mtsComponentInterfaceProxy::ComponentInterfaceServer,
        public IceUtil::Monitor<IceUtil::Mutex>
    {
    private:
        /*! Ice objects */
        Ice::CommunicatorPtr Communicator;
        IceUtil::ThreadPtr SenderThreadPtr;
        Ice::LoggerPtr IceLogger;

        // TODO: Do I really need this flag??? what about mtsProxyBaseCommon::Runnable???
        /*! True if ICE proxy is running */
        bool Runnable;

        /*! Network event handler */
        mtsComponentInterfaceProxyServer * ComponentInterfaceProxyServer;
        
    public:
        ComponentInterfaceServerI(
            const Ice::CommunicatorPtr& communicator, 
            const Ice::LoggerPtr& logger,
            mtsComponentInterfaceProxyServer * componentInterfaceProxyServer);

        void Start();
        void Run();
        void Stop();

        //---------------------------------------
        //  Event Handlers (Client -> Server)
        //---------------------------------------
        /*! Add a client proxy. Called when a proxy client connects to server proxy. */
        bool AddClient(const std::string&, ::Ice::Int, const Ice::Identity&, const Ice::Current&);

        /*! Shutdown this session; prepare shutdown for safe and clean termination. */
        void Shutdown(const ::Ice::Current&);

        bool FetchEventGeneratorProxyPointers(
            const std::string & clientComponentName, const std::string & requiredInterfaceName,
            mtsComponentInterfaceProxy::ListsOfEventGeneratorsRegistered & eventGeneratorProxyPointers,
            const ::Ice::Current & current) const;

        void TestMessageFromClientToServer(const std::string & str, const ::Ice::Current & current);
    };
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsComponentInterfaceProxyServer)

#endif // _mtsComponentInterfaceProxyServer_h

