import nre

module_name = "LCMG"

dlt_app_id = "LCMG"

## Signal list: when the application receives one of these signals from the
## system, the C_APP_IMP::SignalHandler(siginfo) method is called if it
## overrides C_APP_BASE::SignalHandler(siginfo) 
signal_list = [
        ]


## PSM subscription allows your application to receive the power state change
## requests from the PSM. You can override the methods in the C_APP_IMP class,
## which correspond to the state changes that you want to be warned about
## (for example for flushing a file or database on the filesystem, releasing a
## driver...)
psm_subscription = False


#======================== VMOST clients ======================
## Define here the list of VMOST FBlocks/InstanceIds that you want to use as clients.
## NRE will manage the generation of the proxy source code (using most-parser)
## and the integration of the proxy into the application mainloop 
#vmost_proxy_list = [
#		{'FBlock':'AudioMaster','InstanceId':0x00},
#		{'FBlock':'AmFmTuner',	'InstanceId':0x01},			
#]


#======================== CommonAPI Server ======================
## Enter here the filename of the Franca IDL (fidl) file defining the CommonAPI interface
## that your application implements
#CommonAPI_server_fidl = 'Lifecycle.fidl'

CommonAPI_server_fidl_list = ['Lifecycle.fidl']


## And define here the interface names (as defined in your fidl file), and the pair (as defined by CommonAPI)
## <ServiceID>:<InstanceID>
## For DBus:
## <ServiceID> stands for
## <InstanceID> stands for the node address on DBus
CommonAPI_server_interface_list = {
    nre.ipc_type_dbus:[
        {   nre.ipc_interface:'LifecycleAPI',
            nre.ipc_address:'Lifecycle.Interface.LifecycleAPI:Lifecycle.Interface.LifecycleAPI',
            nre.ipc_version:'1.0'},
    ]
}

## Allow to not use the default implementation of the stubs by CommonAPI generator (empty methods doing nothing)
## useful during development, by setting the following parameter to False.
## For example in a release, this helps to check that ALL the interface methods are implemented.
## Optional parameter: True by default
# CommonAPI_server_stub_default = False


#======================== CommonAPI Client ======================
## Same as for CommonAPI server definition,
## but you can define many CommonAPI clients, so enter a list of fidl files.
#TBM20H :CommonAPI_client_fidl_list = ['imicro_interface.fidl', 'ModemAPI.fidl', 'secmwd_certificateManager.fidl']
#~
#~
#CommonAPI_client_interface_list = {
   # nre.ipc_type_dbus:[
   #    {nre.ipc_interface:'Modem_LifeCycle', nre.ipc_address:'capi.nre_imicro.Modem_LifeCycle:capi.nre_imicro.Modem_LifeCycle', nre.ipc_version:'0.8'},
   #   {nre.ipc_interface:'Modem_Interface', nre.ipc_address:'mm.tbm2.tlmapp.ModemAPI.Modem_Interface:mm.tbm2.tlmapp.ModemAPI.Modem_Interface', nre.ipc_version:'1.5'},
   #{nre.ipc_interface:'CertificateManager', nre.ipc_address:'com.magnetimarelli.secmwd.CertificateManager:com.magnetimarelli.secmwd.CertificateManager', nre.ipc_version:'0.2'},
   #     {nre.ipc_interface:'IMICRO_ADMIN', nre.ipc_address:'capi.nre_imicro.IMICRO_ADMIN:capi.nre_imicro.IMICRO_ADMIN', nre.ipc_version:'0.36'}
   # ]
#}

CommonAPI_client_fidl_list = ['Network.fidl',['imicro_interface.fidl','imicro_interface.fdepl','someip']]

# CommonAPI_client_interface_list = {
#     nre.ipc_type_dbus:[
#         {   nre.ipc_interface:'NreExample1',
#             nre.ipc_address:'com.marelli.NreExample1:com.marelli.NreExample1',
#             nre.ipc_version:'1.0'},
#     ]
# }

CommonAPI_client_interface_list = {
    nre.ipc_type_dbus:[
        {   nre.ipc_interface:'SOC_LIFECYCLE',
            nre.ipc_address:'iMicro.SOC_LIFECYCLE:iMicro.SOC_LIFECYCLE',
            nre.ipc_version:'1.0'},
        {   nre.ipc_interface:'NetworkManagerAPI',
            nre.ipc_address:'NetworkManager.NetworkManagerAPI:NetworkManager.NetworkManagerAPI',
            nre.ipc_version:'1.0'},
        {   nre.ipc_interface:'SOC_NTW_API', 
            nre.ipc_address:'iMicro.SOC_NTW_API:iMicro.SOC_NTW_API',
            nre.ipc_version:'1.0'},            
    ]
}

#====== Using Legato IPC =======================================================
## Define here the list of LEGATO Server/Client apis that you want to use.
## NRE will manage the generation of the proxy/server source code

legato = True

##---LEGATO SERVER API LIST----
legato_server_list = [
                         {nre.legato_api:'lifecycle'}
                     ]

##---LEGATO CLIENT API LIST----
legato_client_list = [
                         {nre.legato_api_defs: 'le_limit'},
                         {nre.legato_api:'le_appCtrl'}
                     ]

legato_events_subscription = True

legato_events_from_api = [
                                'le_appCtrl'
                         ]
