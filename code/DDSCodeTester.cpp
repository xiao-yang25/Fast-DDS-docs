
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/qos/DomainParticipantQos.hpp>
#include <fastdds/dds/domain/DomainParticipantListener.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/qos/PublisherQos.hpp>
#include <fastdds/dds/publisher/PublisherListener.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/SubscriberListener.hpp>
#include <fastdds/dds/subscriber/qos/SubscriberQos.hpp>
#include <fastdds/dds/subscriber/SubscriberListener.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/topic/qos/TopicQos.hpp>
#include <fastdds/dds/topic/TopicListener.hpp>
#include <fastrtps/xmlparser/XMLProfileManager.h>
#include <fastrtps/types/DynamicTypePtr.h>
#include <fastdds/dds/log/Log.hpp>
#include <fastdds/dds/log/StdoutConsumer.hpp>
#include <fastdds/dds/log/FileConsumer.hpp>

#include <fastdds/rtps/transport/UDPv4TransportDescriptor.h>
#include <fastrtps/utils/IPLocator.h>

using namespace eprosima::fastdds::dds;

//DDS_DOMAINPARTICIPANT_LISTENER_SPECIALIZATION
class CustomDomainParticipantListener : public DomainParticipantListener
{

public:

    CustomDomainParticipantListener()
    : DomainParticipantListener()
    {
    }

    virtual ~CustomDomainParticipantListener()
    {
    }

    virtual void on_participant_discovery(
            DomainParticipant* /*participant*/,
            eprosima::fastrtps::rtps::ParticipantDiscoveryInfo&& info)
    {
        if (info.status == eprosima::fastrtps::rtps::ParticipantDiscoveryInfo::DISCOVERED_PARTICIPANT)
        {
            std::cout << "New participant discovered" << std::endl;
        }
        else if (info.status == eprosima::fastrtps::rtps::ParticipantDiscoveryInfo::REMOVED_PARTICIPANT ||
                info.status == eprosima::fastrtps::rtps::ParticipantDiscoveryInfo::DROPPED_PARTICIPANT)
        {
            std::cout << "New participant lost" << std::endl;
        }
    }

#if HAVE_SECURITY
    virtual void onParticipantAuthentication(
            DomainParticipant* /*participant*/,
            eprosima::fastrtps::rtps::ParticipantAuthenticationInfo&& info)
    {
        if (info.status == eprosima::fastrtps::rtps::ParticipantAuthenticationInfo::AUTHORIZED_PARTICIPANT)
        {
            std::cout << "A participant was authorized" << std::endl;
        }
        else if (info.status == eprosima::fastrtps::rtps::ParticipantAuthenticationInfo::UNAUTHORIZED_PARTICIPANT)
        {
            std::cout << "A participant failed authorization" << std::endl;
        }
    }

#endif

    virtual void on_subscriber_discovery(
            DomainParticipant* /*participant*/,
            eprosima::fastrtps::rtps::ReaderDiscoveryInfo&& info)
    {
        if (info.status == eprosima::fastrtps::rtps::ReaderDiscoveryInfo::DISCOVERED_READER)
        {
            std::cout << "New subscriber discovered" << std::endl;
        }
        else if (info.status == eprosima::fastrtps::rtps::ReaderDiscoveryInfo::REMOVED_READER)
        {
            std::cout << "New subscriber lost" << std::endl;
        }
    }

    virtual void on_publisher_discovery(
            DomainParticipant* /*participant*/,
            eprosima::fastrtps::rtps::WriterDiscoveryInfo&& info)
    {
        if (info.status == eprosima::fastrtps::rtps::WriterDiscoveryInfo::DISCOVERED_WRITER)
        {
            std::cout << "New publisher discovered" << std::endl;
        }
        else if (info.status == eprosima::fastrtps::rtps::WriterDiscoveryInfo::REMOVED_WRITER)
        {
            std::cout << "New publisher lost" << std::endl;
        }
    }

    virtual void on_type_discovery(
            DomainParticipant* participant,
            const eprosima::fastrtps::rtps::SampleIdentity& request_sample_id,
            const eprosima::fastrtps::string_255& topic,
            const eprosima::fastrtps::types::TypeIdentifier* identifier,
            const eprosima::fastrtps::types::TypeObject* object,
            eprosima::fastrtps::types::DynamicType_ptr dyn_type)
    {
        (void)participant, (void)request_sample_id, (void)topic, (void)identifier, (void)object, (void)dyn_type;
        std::cout << "New data type discovered" << std::endl;

    }

    virtual void on_type_dependencies_reply(
            DomainParticipant* participant,
            const eprosima::fastrtps::rtps::SampleIdentity& request_sample_id,
            const eprosima::fastrtps::types::TypeIdentifierWithSizeSeq& dependencies)
    {
        (void)participant, (void)request_sample_id, (void)dependencies;
        std::cout << "Answer to a request for type dependencies was received" << std::endl;
    }

    virtual void on_type_information_received(
            DomainParticipant* participant,
            const eprosima::fastrtps::string_255 topic_name,
            const eprosima::fastrtps::string_255 type_name,
            const eprosima::fastrtps::types::TypeInformation& type_information)
    {
        (void)participant, (void)topic_name, (void)type_name, (void)type_information;
        std::cout << "New data type information received" << std::endl;
    }
};
//!--

void dds_domain_examples()
{
    {
        //DDS_LOAD_XML_PROFILE
        // Load the XML with the profiles
        DomainParticipantFactory::get_instance()->load_XML_profiles_file("profiles.xml");

        // Profiles can now be used to create Entities
        DomainParticipant* participant_with_profile =
                DomainParticipantFactory::get_instance()->create_participant_with_profile(0, "participant_profile");
        if (nullptr != participant_with_profile)
        {
            // Error
            return;
        }
        //!--
    }

    {
        //DDS_CREATE_DOMAINPARTICIPANT
        // Create a DomainParticipant with default DomainParticipantQos and no Listener
        // The value PARTICIPANT_QOS_DEFAULT is used to denote the default QoS.
        DomainParticipant* participant_with_default_attributes =
                DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);
        if (nullptr != participant_with_default_attributes)
        {
            // Error
            return;
        }

        // A custom DomainParticipantQos can be provided to the creation method
        DomainParticipantQos custom_qos;

        // Modify QoS attributes
        // (...)

        DomainParticipant* participant_with_custom_qos =
                DomainParticipantFactory::get_instance()->create_participant(0, custom_qos);
        if (nullptr != participant_with_custom_qos)
        {
            // Error
            return;
        }

        // Create a DomainParticipant with default QoS and a custom Listener.
        // CustomDomainParticipantListener inherits from DomainParticipantListener.
        // The value PARTICIPANT_QOS_DEFAULT is used to denote the default QoS.
        CustomDomainParticipantListener custom_listener;
        DomainParticipant* participant_with_default_qos_and_custom_listener =
                DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT, &custom_listener);
        if (nullptr != participant_with_default_qos_and_custom_listener)
        {
            // Error
            return;
        }
        //!--
    }

    {
        //DDS_CREATE_PROFILE_DOMAINPARTICIPANT
        // First load the XML with the profiles
        DomainParticipantFactory::get_instance()->load_XML_profiles_file("profiles.xml");

        // Create a DomainParticipant using a profile and no Listener
        DomainParticipant* participant_with_profile =
                DomainParticipantFactory::get_instance()->create_participant_with_profile(0, "participant_profile");
        if (nullptr != participant_with_profile)
        {
            // Error
            return;
        }

        // Create a DomainParticipant using a profile and a custom Listener.
        // CustomDomainParticipantListener inherits from DomainParticipantListener.
        CustomDomainParticipantListener custom_listener;
        DomainParticipant* participant_with_profile_and_custom_listener =
                DomainParticipantFactory::get_instance()->create_participant_with_profile(0, "participant_profile", &custom_listener);
        if (nullptr != participant_with_profile_and_custom_listener)
        {
            // Error
            return;
        }
        //!--
    }

    {
        //DDS_CHANGE_DOMAINPARTICIPANTQOS
        // Create a DomainParticipant with default DomainParticipantQos
        DomainParticipant* participant =
                DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);
        if (nullptr != participant)
        {
            // Error
            return;
        }

        // Get the current QoS or create a new one from scratch
        DomainParticipantQos qos = participant->get_qos();

        // Modify QoS attributes
        qos.entity_factory().autoenable_created_entities = false;

        // Assign the new Qos to the object
        participant->set_qos(qos);
        //!--
    }

    {
        //DDS_CHANGE_DOMAINPARTICIPANTQOS_TO_DEFAULT
        // Create a custom DomainParticipantQos
        DomainParticipantQos custom_qos;

        // Modify QoS attributes
        // (...)

        // Create a DomainParticipant with a custom DomainParticipantQos

        DomainParticipant* participant = DomainParticipantFactory::get_instance()->create_participant(0, custom_qos);
        if (nullptr != participant)
        {
            // Error
            return;
        }

        // Set the QoS on the participant to the default
        if (participant->set_qos(PARTICIPANT_QOS_DEFAULT) != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }

        // The previous instruction is equivalent to the following:
        if(participant->set_qos(DomainParticipantFactory::get_instance()->get_default_participant_qos())
                != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }
        //!--
    }

    {
        //DDS_DELETE_DOMAINPARTICIPANT
        // Create a DomainParticipant
        DomainParticipant* participant =
                DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);
        if (nullptr != participant)
        {
            // Error
            return;
        }

        // Use the DomainParticipant to communicate
        // (...)

        // Delete the DomainParticipant
        if (DomainParticipantFactory::get_instance()->delete_participant(participant) != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }
        //!--
    }

    {
        //DDS_CHANGE_DEFAULT_DOMAINPARTICIPANTQOS
        // Get the current QoS or create a new one from scratch
        DomainParticipantQos qos_type1 = DomainParticipantFactory::get_instance()->get_default_participant_qos();

        // Modify QoS attributes
        // (...)

        // Set as the new default TopicQos
        if(DomainParticipantFactory::get_instance()->set_default_participant_qos(qos_type1) != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }

        // Create a DomainParticipant with the new default DomainParticipantQos.
        DomainParticipant* participant_with_qos_type1 =
                DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);
        if (nullptr != participant_with_qos_type1)
        {
            // Error
            return;
        }

        // Get the current QoS or create a new one from scratch
        DomainParticipantQos qos_type2;

        // Modify QoS attributes
        // (...)

        // Set as the new default TopicQos
        if(DomainParticipantFactory::get_instance()->set_default_participant_qos(qos_type2) != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }

        // Create a Topic with the new default TopicQos.
        DomainParticipant* participant_with_qos_type2 =
                DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);
        if (nullptr != participant_with_qos_type2)
        {
            // Error
            return;
        }

        // Resetting the default DomainParticipantQos to the original default constructed values
        if(DomainParticipantFactory::get_instance()->set_default_participant_qos(PARTICIPANT_QOS_DEFAULT)
                != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }

        // The previous instruction is equivalent to the following
        if(DomainParticipantFactory::get_instance()->set_default_participant_qos(DomainParticipantQos())
                != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }
        //!--
    }

    {
        //DDS_CHANGE_DOMAINPARTICIPANTFACTORYQOS
        DomainParticipantFactoryQos qos;

        // Setting autoenable_created_entities to true makes the created DomainParticipants
        // to be enabled upon creation
        qos.entity_factory().autoenable_created_entities = true;
        if(DomainParticipantFactory::get_instance()->set_qos(qos) != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }

        // Create a DomainParticipant with the new DomainParticipantFactoryQos.
        // The returned DomainParticipant is already enabled
        DomainParticipant* enabled_participant =
                DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);
        if (nullptr != enabled_participant)
        {
            // Error
            return;
        }

        // Setting autoenable_created_entities to false makes the created DomainParticipants
        // to be disabled upon creation
        qos.entity_factory().autoenable_created_entities = false;
        if(DomainParticipantFactory::get_instance()->set_qos(qos) != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }

        // Create a DomainParticipant with the new DomainParticipantFactoryQos.
        // The returned DomainParticipant is disabled and will need to be enabled explicitly
        DomainParticipant* disabled_participant =
                DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);
        if (nullptr != disabled_participant)
        {
            // Error
            return;
        }
        //!--
    }
    {
        // DDS_SECURITY_AUTH_PLUGIN
        eprosima::fastdds::dds::DomainParticipantQos pqos;

        // Activate DDS:Auth:PKI-DH plugin
        pqos.properties().properties().emplace_back("dds.sec.auth.plugin",
            "builtin.PKI-DH");

        // Configure DDS:Auth:PKI-DH plugin
        pqos.properties().properties().emplace_back(
            "dds.sec.auth.builtin.PKI-DH.identity_ca",
            "file://maincacert.pem");
        pqos.properties().properties().emplace_back(
            "dds.sec.auth.builtin.PKI-DH.identity_certificate",
            "file://partcert.pem");
        pqos.properties().properties().emplace_back(
            "dds.sec.auth.builtin.PKI-DH.identity_crl",
            "file://crl.pem");
        pqos.properties().properties().emplace_back(
            "dds.sec.auth.builtin.PKI-DH.private_key",
            "file://partkey.pem");
        pqos.properties().properties().emplace_back(
            "dds.sec.auth.builtin.PKI-DH.password",
            "domainParticipantPassword");
        //!--
    }
    {
        // DDS_SECURITY_ACCESS_CONTROL_PLUGIN
        eprosima::fastdds::dds::DomainParticipantQos pqos;

        // Activate DDS:Access:Permissions plugin
        pqos.properties().properties().emplace_back("dds.sec.access.plugin",
            "builtin.Access-Permissions");

        // Configure DDS:Access:Permissions plugin
        pqos.properties().properties().emplace_back(
            "dds.sec.access.builtin.Access-Permissions.permissions_ca",
            "file://certs/maincacert.pem");
        pqos.properties().properties().emplace_back(
            "dds.sec.access.builtin.Access-Permissions.governance",
            "file://certs/governance.smime");
        pqos.properties().properties().emplace_back(
            "dds.sec.access.builtin.Access-Permissions.permissions",
            "file://certs/permissions.smime");
        //!--
    }
    {
        // DDS_SECURITY_CRYPTO_PLUGIN_DOMAINPARTICIPANT
        eprosima::fastdds::dds::DomainParticipantQos pqos;

        // Activate DDS:Crypto:AES-GCM-GMAC plugin
        pqos.properties().properties().emplace_back("dds.sec.crypto.plugin",
            "builtin.AES-GCM-GMAC");

        // Only if DDS:Access:Permissions plugin is not enabled
        // Configure DDS:Crypto:AES-GCM-GMAC plugin
        pqos.properties().properties().emplace_back(
            "rtps.participant.rtps_protection_kind",
            "ENCRYPT");
        //!--

        // DDS_SECURITY_CRYPTO_PLUGIN_DATAWRITER
        eprosima::fastdds::dds::DataWriterQos wqos;

        // Only if DDS:Access:Permissions plugin is not enabled
        // Configure DDS:Crypto:AES-GCM-GMAC plugin
        wqos.properties().properties().emplace_back(
            "rtps.endpoint.submessage_protection_kind",
            "ENCRYPT");
        wqos.properties().properties().emplace_back(
            "rtps.endpoint.payload_protection_kind",
            "ENCRYPT");
        //!--

        // DDS_SECURITY_CRYPTO_PLUGIN_DATAREADER
        eprosima::fastdds::dds::DataWriterQos rqos;

        // Only if DDS:Access:Permissions plugin is not enabled
        // Configure DDS:Crypto:AES-GCM-GMAC plugin
        rqos.properties().properties().emplace_back(
            "rtps.endpoint.submessage_protection_kind",
            "ENCRYPT");
        //!--
    }
    {
        // DDS_SECURITY_LOGGING_PLUGIN
        eprosima::fastdds::dds::DomainParticipantQos pqos;

        // Activate DDS:Logging:DDS_LogTopic plugin
        pqos.properties().properties().emplace_back("dds.sec.log.plugin",
            "builtin.DDS_LogTopic");

        // Configure DDS:Logging:DDS_LogTopic plugin
        pqos.properties().properties().emplace_back(
            "dds.sec.log.builtin.DDS_LogTopic.logging_level",
            "EMERGENCY_LEVEL");
        pqos.properties().properties().emplace_back(
            "dds.sec.log.builtin.DDS_LogTopic.log_file",
            "myLogFile.log");
        //!--
    }
}

//DOMAINPARTICIPANTLISTENER-DISCOVERY-CALLBACKS
class DiscoveryDomainParticipantListener : public DomainParticipantListener
{
    /* Custom Callback on_participant_discovery */
    virtual void on_participant_discovery(
            DomainParticipant* participant,
            eprosima::fastrtps::rtps::ParticipantDiscoveryInfo&& info)
    {
        (void)participant;
        switch (info.status){
            case eprosima::fastrtps::rtps::ParticipantDiscoveryInfo::DISCOVERED_PARTICIPANT:
                /* Process the case when a new DomainParticipant was found in the domain */
                std::cout << "New DomainParticipant '" << info.info.m_participantName <<
                    "' with ID '" << info.info.m_guid.entityId << "' and GuidPrefix '" <<
                    info.info.m_guid.guidPrefix << "' discovered." << std::endl;
                break;
            case eprosima::fastrtps::rtps::ParticipantDiscoveryInfo::CHANGED_QOS_PARTICIPANT:
                /* Process the case when a DomainParticipant changed its QOS */
                break;
            case eprosima::fastrtps::rtps::ParticipantDiscoveryInfo::REMOVED_PARTICIPANT:
                /* Process the case when a DomainParticipant was removed from the domain */
                std::cout << "New DomainParticipant '" << info.info.m_participantName <<
                    "' with ID '" << info.info.m_guid.entityId << "' and GuidPrefix '" <<
                    info.info.m_guid.guidPrefix << "' left the domain." << std::endl;
                break;
        }
    }

    /* Custom Callback on_subscriber_discovery */
    virtual void on_subscriber_discovery(
            DomainParticipant* participant,
            eprosima::fastrtps::rtps::ReaderDiscoveryInfo&& info)
    {
        (void)participant;
        switch (info.status){
            case eprosima::fastrtps::rtps::ReaderDiscoveryInfo::DISCOVERED_READER:
                /* Process the case when a new subscriber was found in the domain */
                std::cout << "New DataReader subscribed to topic '" << info.info.topicName() <<
                "' of type '" << info.info.typeName() << "' discovered";
                break;
            case eprosima::fastrtps::rtps::ReaderDiscoveryInfo::CHANGED_QOS_READER:
                /* Process the case when a subscriber changed its QOS */
                break;
            case eprosima::fastrtps::rtps::ReaderDiscoveryInfo::REMOVED_READER:
                /* Process the case when a subscriber was removed from the domain */
                std::cout << "New DataReader subscribed to topic '" << info.info.topicName() <<
                    "' of type '" << info.info.typeName() << "' left the domain.";
                break;
        }
    }

    /* Custom Callback on_publisher_discovery */
    virtual void on_publisher_discovery(
            DomainParticipant* participant,
            eprosima::fastrtps::rtps::WriterDiscoveryInfo&& info)
    {
        (void)participant;
        switch (info.status){
            case eprosima::fastrtps::rtps::WriterDiscoveryInfo::DISCOVERED_WRITER:
                /* Process the case when a new publisher was found in the domain */
                std::cout << "New DataWriter publishing under topic '" << info.info.topicName() <<
                    "' of type '" << info.info.typeName() << "' discovered";
                break;
            case eprosima::fastrtps::rtps::WriterDiscoveryInfo::CHANGED_QOS_WRITER:
                /* Process the case when a publisher changed its QOS */
                break;
            case eprosima::fastrtps::rtps::WriterDiscoveryInfo::REMOVED_WRITER:
                /* Process the case when a publisher was removed from the domain */
                std::cout << "New DataWriter publishing under topic '" << info.info.topicName() <<
                    "' of type '" << info.info.typeName() << "' left the domain.";
                break;
        }
    }

    /* Custom Callback on_type_discovery */
    virtual void on_type_discovery(
            DomainParticipant* participant,
            const eprosima::fastrtps::rtps::SampleIdentity& request_sample_id,
            const eprosima::fastrtps::string_255& topic,
            const eprosima::fastrtps::types::TypeIdentifier* identifier,
            const eprosima::fastrtps::types::TypeObject* object,
            eprosima::fastrtps::types::DynamicType_ptr dyn_type)
    {
        (void)participant, (void)request_sample_id, (void)topic, (void)identifier, (void)object, (void)dyn_type;
        std::cout << "New data type of topic '" << topic << "' discovered." << std::endl;
    }
};
//!--

void dds_discovery_examples()
{
    {
        eprosima::fastdds::dds::DomainParticipantQos pqos;

        //SET-DISCOVERY-CALLBACKS
        // Create a custom user DomainParticipantListener
        DiscoveryDomainParticipantListener* plistener = new DiscoveryDomainParticipantListener();
        // Pass the listener on DomainParticipant creation.
        DomainParticipant* participant =
                DomainParticipantFactory::get_instance()->create_participant(
                        0, PARTICIPANT_QOS_DEFAULT, plistener);
        //!--

        //CONF-DISCOVERY-PROTOCOL
        pqos.wire_protocol().builtin.discovery_config.discoveryProtocol =
                eprosima::fastrtps::rtps::DiscoveryProtocol_t::SIMPLE;
        //!--

        //CONF-DISCOVERY-IGNORE-FLAGS
        pqos.wire_protocol().builtin.discovery_config.ignoreParticipantFlags =
                static_cast<eprosima::fastrtps::rtps::ParticipantFilteringFlags_t>(
            eprosima::fastrtps::rtps::ParticipantFilteringFlags_t::FILTER_DIFFERENT_PROCESS |
            eprosima::fastrtps::rtps::ParticipantFilteringFlags_t::FILTER_SAME_PROCESS);
        //!--

        //CONF-DISCOVERY-LEASE-DURATION
        pqos.wire_protocol().builtin.discovery_config.leaseDuration = Duration_t(10, 20);
        //!--

        //CONF-DISCOVERY-LEASE-ANNOUNCEMENT
        pqos.wire_protocol().builtin.discovery_config.leaseDuration_announcementperiod = Duration_t(1, 2);
        //!--

        //DISCOVERY-CONFIG-INITIAL-ANNOUNCEMENT
        pqos.wire_protocol().builtin.discovery_config.initial_announcements.count = 5;
        pqos.wire_protocol().builtin.discovery_config.initial_announcements.period = Duration_t(0, 100000000u);
        //!--

        //CONF-QOS-DISCOVERY-EDP-ATTRIBUTES
        pqos.wire_protocol().builtin.discovery_config.use_SIMPLE_EndpointDiscoveryProtocol = true;
        pqos.wire_protocol().builtin.discovery_config.m_simpleEDP.use_PublicationWriterANDSubscriptionReader = true;
        pqos.wire_protocol().builtin.discovery_config.m_simpleEDP.use_PublicationReaderANDSubscriptionWriter = false;
        //!--

        //CONF_INITIAL_PEERS_BASIC
        eprosima::fastrtps::rtps::Locator_t initial_peers_locator;
        eprosima::fastrtps::rtps::IPLocator::setIPv4(initial_peers_locator, "192.168.10.13");
        initial_peers_locator.port = 7412;

        pqos.wire_protocol().builtin.initialPeersList.push_back(initial_peers_locator);
        //!--

        //CONF_STATIC_DISCOVERY_CODE
        pqos.wire_protocol().builtin.discovery_config.use_SIMPLE_EndpointDiscoveryProtocol = false;
        pqos.wire_protocol().builtin.discovery_config.use_STATIC_EndpointDiscoveryProtocol = true;
        //!--

        //CONF_QOS_STATIC_DISCOVERY_USERID
        // Configure the DataWriter
        eprosima::fastdds::dds::DataWriterQos wqos;
        wqos.endpoint().user_defined_id = 1;

        // Configure the DataReader
        eprosima::fastdds::dds::DataReaderQos rqos;
        rqos.endpoint().user_defined_id = 3;
        //!--

        //CONF_STATIC_DISCOVERY_XML
        pqos.wire_protocol().builtin.discovery_config.setStaticEndpointXMLFilename("RemotePublisher.xml");
        pqos.wire_protocol().builtin.discovery_config.setStaticEndpointXMLFilename("RemoteSubscriber.xml");
        //!--
    }
    {
        //CONF_SERVER_DISCOVERY_PROTOCOL
        eprosima::fastdds::dds::DomainParticipantQos pqos;

        pqos.wire_protocol().builtin.discovery_config.discoveryProtocol =
                eprosima::fastrtps::rtps::DiscoveryProtocol_t::CLIENT;
        pqos.wire_protocol().builtin.discovery_config.discoveryProtocol =
                eprosima::fastrtps::rtps::DiscoveryProtocol_t::SERVER;
        pqos.wire_protocol().builtin.discovery_config.discoveryProtocol =
                eprosima::fastrtps::rtps::DiscoveryProtocol_t::BACKUP;
        //!--
    }
    {
        //CONF_SERVER_SERVER_GUIDPREFIX_OPTION_1
        using namespace eprosima::fastrtps::rtps;

        GuidPrefix_t serverGuidPrefix;
        serverGuidPrefix.value[0] = octet(0x77);
        serverGuidPrefix.value[1] = octet(0x73);
        serverGuidPrefix.value[2] = octet(0x71);
        serverGuidPrefix.value[3] = octet(0x85);
        serverGuidPrefix.value[4] = octet(0x69);
        serverGuidPrefix.value[5] = octet(0x76);
        serverGuidPrefix.value[6] = octet(0x95);
        serverGuidPrefix.value[7] = octet(0x66);
        serverGuidPrefix.value[8] = octet(0x65);
        serverGuidPrefix.value[9] = octet(0x82);
        serverGuidPrefix.value[10] = octet(0x82);
        serverGuidPrefix.value[11] = octet(0x79);

        eprosima::fastdds::dds::DomainParticipantQos serverQos;
        serverQos.wire_protocol().prefix = serverGuidPrefix;
        //!--
    }
    {
        //CONF_SERVER_SERVER_GUIDPREFIX_OPTION_2
        eprosima::fastdds::dds::DomainParticipantQos serverQos;
        std::istringstream("4d.49.47.55.45.4c.5f.42.41.52.52.4f") >> serverQos.wire_protocol().prefix;
        //!--
    }
    {
        //CONF_SERVER_METATRAFFICUNICAST
        eprosima::fastrtps::rtps::Locator_t locator;
        eprosima::fastrtps::rtps::IPLocator::setIPv4(locator, 192, 168, 1, 133);
        locator.port = 64863;

        eprosima::fastdds::dds::DomainParticipantQos serverQos;
        serverQos.wire_protocol().builtin.metatrafficUnicastLocatorList.push_back(locator);
        //!--
    }
    {
        //CONF_SERVER_CLIENT_GUIDPREFIX
        eprosima::fastrtps::rtps::RemoteServerAttributes server;
        server.ReadguidPrefix("4D.49.47.55.45.4c.5f.42.41.52.52.4f");

        eprosima::fastdds::dds::DomainParticipantQos clientQos;
        clientQos.wire_protocol().builtin.discovery_config.m_DiscoveryServers.push_back(server);
        //!--
    }
    {
        //CONF_SERVER_CLIENT_LOCATORS
        eprosima::fastrtps::rtps::Locator_t locator;
        eprosima::fastrtps::rtps::IPLocator::setIPv4(locator, 192, 168, 1, 133);
        locator.port = 64863;
        eprosima::fastrtps::rtps::RemoteServerAttributes server;
        server.metatrafficUnicastLocatorList.push_back(locator);

        eprosima::fastdds::dds::DomainParticipantQos clientQos;
        clientQos.wire_protocol().builtin.discovery_config.m_DiscoveryServers.push_back(server);
        //!--
    }

    {
        //CONF_SERVER_SERVER_LOCATORS
        eprosima::fastrtps::rtps::Locator_t locator;
        eprosima::fastrtps::rtps::IPLocator::setIPv4(locator, 192, 168, 1, 133);
        locator.port = 64863;

        eprosima::fastdds::dds::DomainParticipantQos serverQos;
        serverQos.wire_protocol().builtin.metatrafficUnicastLocatorList.push_back(locator);
        //!--
    }

    {
        //CONF_SERVER_CLIENT_PING
        eprosima::fastdds::dds::DomainParticipantQos clientQos;
        clientQos.wire_protocol().builtin.discovery_config.discoveryServer_client_syncperiod =
                Duration_t(0, 250000000);
        //!--
    }

    {
        //CONF_SERVER_SERVER_PING
        eprosima::fastdds::dds::DomainParticipantQos serverQos;
        serverQos.wire_protocol().builtin.discovery_config.discoveryServer_client_syncperiod =
                Duration_t(0, 250000000);
        //!--
    }

    {
        //CONF_SERVER_PING
        eprosima::fastrtps::rtps::RemoteServerAttributes server;
        server.ReadguidPrefix("4D.49.47.55.45.4c.5f.42.41.52.52.4f");

        eprosima::fastrtps::rtps::Locator_t locator;
        eprosima::fastrtps::rtps::IPLocator::setIPv4(locator, 192, 168, 1, 133);
        locator.port = 64863;
        server.metatrafficUnicastLocatorList.push_back(locator);

        eprosima::fastdds::dds::DomainParticipantQos clientQos;
        clientQos.wire_protocol().builtin.discovery_config.discoveryProtocol =
                eprosima::fastrtps::rtps::DiscoveryProtocol_t::CLIENT;
        clientQos.wire_protocol().builtin.discovery_config.m_DiscoveryServers.push_back(server);
        clientQos.wire_protocol().builtin.discovery_config.discoveryServer_client_syncperiod =
                Duration_t(0, 250000000);
        //!--
    }
}

//DDS_TOPIC_LISTENER_SPECIALIZATION
class CustomTopicListener : public TopicListener
{

public:

    CustomTopicListener()
    : TopicListener()
    {
    }

    virtual ~CustomTopicListener()
    {
    }

    virtual void on_inconsistent_topic(
            Topic* topic,
            InconsistentTopicStatus status)
    {
        (void)topic, (void)status;
        std::cout << "Inconsistent topic received discovered" << std::endl;
    }
};
//!--

class CustomDataType : public TopicDataType
{
public:

    CustomDataType()
        : TopicDataType()
    {
        setName("footype");
    }

    bool serialize(
            void* /*data*/,
            eprosima::fastrtps::rtps::SerializedPayload_t* /*payload*/) override
    {
        return true;
    }

    bool deserialize(
            eprosima::fastrtps::rtps::SerializedPayload_t* /*payload*/,
            void* /*data*/) override
    {
        return true;
    }

    std::function<uint32_t()> getSerializedSizeProvider(
            void* /*data*/) override
    {
        return std::function<uint32_t()>();
    }

    void* createData() override
    {
        return nullptr;
    }

    void deleteData(
            void* /*data*/) override
    {
    }

    bool getKey(
            void* /*data*/,
            eprosima::fastrtps::rtps::InstanceHandle_t* /*ihandle*/,
            bool /*force_md5*/) override
    {
        return true;
    }
};

void dds_topic_examples()
{
    {
        //DDS_CREATE_TOPIC
        // Create a DomainParticipant in the desired domain
        DomainParticipant* participant =
                DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);
        if (nullptr != participant)
        {
            // Error
            return;
        }

        // Create a Topic with default TopicQos and no Listener
        // The symbol TOPIC_QOS_DEFAULT is used to denote the default QoS.
        Topic* topic_with_default_qos =
                participant->create_topic("TopicName", "DataTypeName", TOPIC_QOS_DEFAULT);
        if (nullptr != topic_with_default_qos)
        {
            // Error
            return;
        }

        // A custom TopicQos can be provided to the creation method
        TopicQos custom_qos;

        // Modify QoS attributes
        // (...)

        Topic* topic_with_custom_qos =
                participant->create_topic("TopicName", "DataTypeName", custom_qos);
        if (nullptr != topic_with_custom_qos)
        {
            // Error
            return;
        }

        // Create a Topic with default QoS and a custom Listener.
        // CustomTopicListener inherits from TopicListener.
        // The symbol TOPIC_QOS_DEFAULT is used to denote the default QoS.
        CustomTopicListener custom_listener;
        Topic* topic_with_default_qos_and_custom_listener =
                participant->create_topic("TopicName", "DataTypeName", TOPIC_QOS_DEFAULT, &custom_listener);
        if (nullptr != topic_with_default_qos_and_custom_listener)
        {
            // Error
            return;
        }
        //!--
    }

    {
        //DDS_CREATE_PROFILE_TOPIC
        // First load the XML with the profiles
        DomainParticipantFactory::get_instance()->load_XML_profiles_file("profiles.xml");

        // Create a DomainParticipant in the desired domain
        DomainParticipant* participant =
                DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);
        if (nullptr != participant)
        {
            // Error
            return;
        }

        // Create a Topic using a profile and no Listener
        Topic* topic_with_profile =
                participant->create_topic_with_profile("TopicName", "DataTypeName", "topic_profile");
        if (nullptr != topic_with_profile)
        {
            // Error
            return;
        }

        // Create a Topic using a profile and a custom Listener.
        // CustomTopicListener inherits from TopicListener.
        CustomTopicListener custom_listener;
        Topic* topic_with_profile_and_custom_listener =
                participant->create_topic_with_profile("TopicName", "DataTypeName", "topic_profile", &custom_listener);
        if (nullptr != topic_with_profile_and_custom_listener)
        {
            // Error
            return;
        }
        //!--
    }

    {
        //DDS_CHANGE_TOPICQOS
        // Create a DomainParticipant in the desired domain
        DomainParticipant* participant =
                DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);
        if (nullptr != participant)
        {
            // Error
            return;
        }

        // Create a Topic with default TopicQos
        Topic* topic =
                participant->create_topic("TopicName", "DataTypeName", TOPIC_QOS_DEFAULT);
        if (nullptr != topic)
        {
            // Error
            return;
        }

        // Get the current QoS or create a new one from scratch
        TopicQos qos = topic->get_qos();

        // Modify QoS attributes
        // (...)

        // Assign the new Qos to the object
        topic->set_qos(qos);
        //!--
    }

    {
        //DDS_CHANGE_TOPICQOS_TO_DEFAULT
        // Create a DomainParticipant in the desired domain
        DomainParticipant* participant =
                DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);
        if (nullptr != participant)
        {
            // Error
            return;
        }

        // Create a custom TopicQos
        TopicQos custom_qos;

        // Modify QoS attributes
        // (...)

        // Create a topic with a custom TopicQos
        Topic* topic = participant->create_topic("TopicName", "DataTypeName", custom_qos);
        if (nullptr != topic)
        {
            // Error
            return;
        }

        // Set the QoS on the topic to the default
        if (topic->set_qos(TOPIC_QOS_DEFAULT) != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }

        // The previous instruction is equivalent to the following:
        if(topic->set_qos(participant->get_default_topic_qos())
                != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }
        //!--
    }

    {
        //DDS_DELETE_TOPIC
        // Create a DomainParticipant in the desired domain
        DomainParticipant* participant =
                DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);
        if (nullptr != participant)
        {
            // Error
            return;
        }

        // Create a Topic
        Topic* topic =
                participant->create_topic("TopicName", "DataTypeName", TOPIC_QOS_DEFAULT);
        if (nullptr != topic)
        {
            // Error
            return;
        }

        // Use the Topic to communicate
        // (...)

        // Delete the Topic
        if (participant->delete_topic(topic) != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }
        //!--
    }

    {
        //DDS_CHANGE_DEFAULT_TOPICQOS
        // Create a DomainParticipant in the desired domain
        DomainParticipant* participant =
                DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);
        if (nullptr != participant)
        {
            // Error
            return;
        }

        // Get the current QoS or create a new one from scratch
        TopicQos qos_type1 = participant->get_default_topic_qos();

        // Modify QoS attributes
        // (...)

        // Set as the new default TopicQos
        if(participant->set_default_topic_qos(qos_type1) != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }

        // Create a Topic with the new default TopicQos.
        Topic* topic_with_qos_type1 =
                participant->create_topic("TopicName", "DataTypeName", TOPIC_QOS_DEFAULT);
        if (nullptr != topic_with_qos_type1)
        {
            // Error
            return;
        }

        // Get the current QoS or create a new one from scratch
        TopicQos qos_type2;

        // Modify QoS attributes
        // (...)

        // Set as the new default TopicQos
        if(participant->set_default_topic_qos(qos_type2) != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }

        // Create a Topic with the new default TopicQos.
        Topic* topic_with_qos_type2 =
                participant->create_topic("TopicName", "DataTypeName", TOPIC_QOS_DEFAULT);
        if (nullptr != topic_with_qos_type2)
        {
            // Error
            return;
        }

        // Resetting the default TopicQos to the original default constructed values
        if(participant->set_default_topic_qos(TOPIC_QOS_DEFAULT)
                != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }

        // The previous instruction is equivalent to the following
        if(participant->set_default_topic_qos(TopicQos())
                != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }
        //!--
    }

    {
        //DDS_TYPE_REGISTER
        // Create a DomainParticipant in the desired domain
        DomainParticipant* participant =
                DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);
        if (nullptr != participant)
        {
            // Error
            return;
        }

        // Register the data type in the DomainParticipant.
        // If nullptr is used as name argument, the one returned by the type itself is used
        TypeSupport custom_type_support(new CustomDataType());
        custom_type_support.register_type(participant, nullptr);

        // The previous instruction is equivalent to the following one
        // Even if we are registering the same data type with the same name twice, no error will be issued
        custom_type_support.register_type(participant, custom_type_support.get_type_name());

        // Create a Topic with the registered type.
        Topic* topic =
                participant->create_topic("topic_name", custom_type_support.get_type_name(), TOPIC_QOS_DEFAULT);
        if (nullptr != topic)
        {
            // Error
            return;
        }

        // Create an alias for the same data type using a different name.
        custom_type_support.register_type(participant, "data_type_name");

        // We can now use the aliased name to If no name is given, it uses the name returned by the type itself
        Topic* another_topic =
                participant->create_topic("other_topic_name", "data_type_name", TOPIC_QOS_DEFAULT);
        if (nullptr != another_topic)
        {
            // Error
            return;
        }
        //!--
    }

    {
        //DDS_DYNAMIC_TYPES
        // Create a DomainParticipant in the desired domain
        DomainParticipant* participant =
                DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);
        if (nullptr != participant)
        {
            // Error
            return;
        }

        // Load the XML file with the type description
        eprosima::fastrtps::xmlparser::XMLProfileManager::loadXMLFile("example_type.xml");

        // Retrieve the an instance of the desired type and register it
        eprosima::fastrtps::types::DynamicType_ptr dyn_type =
                eprosima::fastrtps::xmlparser::XMLProfileManager::getDynamicTypeByName("DynamicType")->build();
        TypeSupport dyn_type_support(new eprosima::fastrtps::types::DynamicPubSubType(dyn_type));
        dyn_type_support.register_type(participant, nullptr);

        // Create a Topic with the registered type.
        Topic* topic =
                participant->create_topic("topic_name", dyn_type_support.get_type_name(), TOPIC_QOS_DEFAULT);
        if (nullptr != topic)
        {
            // Error
            return;
        }
        //!--
    }
}


class CustomPublisherListener : public PublisherListener
{
};

void dds_publisher_examples()
{
    {
        //DDS_CREATE_PUBLISHER
        // Create a DomainParticipant in the desired domain
        DomainParticipant* participant =
                DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);
        if (nullptr != participant)
        {
            // Error
            return;
        }

        // Create a Publisher with default PublisherQos and no Listener
        // The value PUBLISHER_QOS_DEFAULT is used to denote the default QoS.
        Publisher* publisher_with_default_qos =
                participant->create_publisher(PUBLISHER_QOS_DEFAULT);
        if (nullptr != publisher_with_default_qos)
        {
            // Error
            return;
        }

        // A custom PublisherQos can be provided to the creation method
        PublisherQos custom_qos;

        // Modify QoS attributes
        // (...)

        Publisher* publisher_with_custom_qos =
                participant->create_publisher(custom_qos);
        if (nullptr != publisher_with_custom_qos)
        {
            // Error
            return;
        }

        // Create a Publisher with default QoS and a custom Listener.
        // CustomPublisherListener inherits from PublisherListener.
        // The value PUBLISHER_QOS_DEFAULT is used to denote the default QoS.
        CustomPublisherListener custom_listener;
        Publisher* publisher_with_default_qos_and_custom_listener =
                participant->create_publisher(PUBLISHER_QOS_DEFAULT, &custom_listener);
        if (nullptr != publisher_with_default_qos_and_custom_listener)
        {
            // Error
            return;
        }
        //!--
    }

    {
        //DDS_CREATE_PROFILE_PUBLISHER
        // First load the XML with the profiles
        DomainParticipantFactory::get_instance()->load_XML_profiles_file("profiles.xml");

        // Create a DomainParticipant in the desired domain
        DomainParticipant* participant =
                DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);
        if (nullptr != participant)
        {
            // Error
            return;
        }

        // Create a Publisher using a profile and no Listener
        Publisher* publisher_with_profile =
                participant->create_publisher_with_profile("publisher_profile");
        if (nullptr != publisher_with_profile)
        {
            // Error
            return;
        }

        // Create a Publisher using a profile and a custom Listener.
        // CustomPublisherListener inherits from PublisherListener.
        CustomPublisherListener custom_listener;
        Publisher* publisher_with_profile_and_custom_listener =
                participant->create_publisher_with_profile("publisher_profile", &custom_listener);
        if (nullptr != publisher_with_profile_and_custom_listener)
        {
            // Error
            return;
        }
        //!--
    }

    {
        //DDS_CHANGE_PUBLISHERQOS
        // Create a DomainParticipant in the desired domain
        DomainParticipant* participant =
                DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);
        if (nullptr != participant)
        {
            // Error
            return;
        }

        // Create a Publisher with default PublisherQos
        Publisher* publisher =
                participant->create_publisher(PUBLISHER_QOS_DEFAULT);
        if (nullptr != publisher)
        {
            // Error
            return;
        }

        // Get the current QoS or create a new one from scratch
        PublisherQos qos = publisher->get_qos();

        // Modify QoS attributes
        // (...)

        // Assign the new Qos to the object
        publisher->set_qos(qos);
        //!--
    }

    {
        //DDS_CHANGE_PUBLISHERQOS_TO_DEFAULT
        // Create a DomainParticipant in the desired domain
        DomainParticipant* participant =
                DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);
        if (nullptr != participant)
        {
            // Error
            return;
        }

        // Create a custom PublisherQos
        PublisherQos custom_qos;

        // Modify QoS attributes
        // (...)

        // Create a publisher with a custom PublisherQos
        Publisher* publisher = participant->create_publisher(custom_qos);
        if (nullptr != publisher)
        {
            // Error
            return;
        }

        // Set the QoS on the publisher to the default
        if (publisher->set_qos(PUBLISHER_QOS_DEFAULT) != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }

        // The previous instruction is equivalent to the following:
        if(publisher->set_qos(participant->get_default_publisher_qos())
                != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }
        //!--
    }

    {
        //DDS_DELETE_PUBLISHER
        // Create a DomainParticipant in the desired domain
        DomainParticipant* participant =
                DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);
        if (nullptr != participant)
        {
            // Error
            return;
        }

        // Create a Publisher
        Publisher* publisher =
                participant->create_publisher(PUBLISHER_QOS_DEFAULT);
        if (nullptr != publisher)
        {
            // Error
            return;
        }

        // Use the Publisher to communicate
        // (...)

        // Delete the Publisher
        if (participant->delete_publisher(publisher) != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }
        //!--
    }

    {
        //DDS_CHANGE_DEFAULT_PUBLISHERQOS
        // Create a DomainParticipant in the desired domain
        DomainParticipant* participant =
                DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);
        if (nullptr != participant)
        {
            // Error
            return;
        }

        // Get the current QoS or create a new one from scratch
        PublisherQos qos_type1 = participant->get_default_publisher_qos();

        // Modify QoS attributes
        // (...)

        // Set as the new default PublisherQos
        if(participant->set_default_publisher_qos(qos_type1) != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }

        // Create a Publisher with the new default PublisherQos.
        Publisher* publisher_with_qos_type1 =
                participant->create_publisher(PUBLISHER_QOS_DEFAULT);
        if (nullptr != publisher_with_qos_type1)
        {
            // Error
            return;
        }

        // Get the current QoS or create a new one from scratch
        PublisherQos qos_type2;

        // Modify QoS attributes
        // (...)

        // Set as the new default PublisherQos
        if(participant->set_default_publisher_qos(qos_type2) != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }

        // Create a Publisher with the new default PublisherQos.
        Publisher* publisher_with_qos_type2 =
                participant->create_publisher(PUBLISHER_QOS_DEFAULT);
        if (nullptr != publisher_with_qos_type2)
        {
            // Error
            return;
        }

        // Resetting the default PublisherQos to the original default constructed values
        if(participant->set_default_publisher_qos(PUBLISHER_QOS_DEFAULT)
                != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }

        // The previous instruction is equivalent to the following
        if(participant->set_default_publisher_qos(PublisherQos())
                != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }
        //!--
    }
}

//DDS_DATAWRITER_LISTENER_SPECIALIZATION
class CustomDataWriterListener : public DataWriterListener
{

public:

    CustomDataWriterListener()
    : DataWriterListener()
    {
    }

    virtual ~CustomDataWriterListener()
    {
    }

    virtual void on_publication_matched(
            DataWriter* writer,
            const PublicationMatchedStatus& info)
    {
        (void)writer
        ;
        if (info.current_count_change == 1)
        {
            std::cout << "Matched a remote Subscriber for one of our Topics" << std::endl;
        }
        else if (info.current_count_change == -1)
        {
            std::cout << "Unmatched a remote Subscriber" << std::endl;
        }
    }

    virtual void on_offered_deadline_missed(
             DataWriter* writer,
             const OfferedDeadlineMissedStatus& status)
    {
         (void)writer, (void)status;
         std::cout << "Some data could not be delivered on time" << std::endl;
    }

    virtual void on_offered_incompatible_qos(
         DataWriter* writer,
         const OfferedIncompatibleQosStatus& status)
    {
        (void)writer, (void)status;
        std::cout << "Found a remote Topic with incompatible QoS" << std::endl;
    }

    virtual void on_liveliness_lost(
         DataWriter* writer,
         const LivelinessLostStatus& status)
    {
        (void)writer, (void)status;
        std::cout << "Liveliness lost. Matched Subscribers will consider us offline" << std::endl;
    }
};
//!--

void dds_dataWriter_examples()
{
    // Taken out of the examples to avoid bloating them
    DomainParticipant* participant =
            DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);
    Publisher* publisher =
            participant->create_publisher(PUBLISHER_QOS_DEFAULT);
    Topic* topic =
            participant->create_topic("TopicName", "DataTypeName", TOPIC_QOS_DEFAULT);

    {
        //DDS_CREATE_DATAWRITER
        // Create a DataWriter with default DataWriterQos and no Listener
        // The value DATAWRITER_QOS_DEFAULT is used to denote the default QoS.
        DataWriter* data_writer_with_default_qos =
                publisher->create_datawriter(topic, DATAWRITER_QOS_DEFAULT);
        if (nullptr != data_writer_with_default_qos)
        {
            // Error
            return;
        }

        // A custom DataWriterQos can be provided to the creation method
        DataWriterQos custom_qos;

        // Modify QoS attributes
        // (...)

        DataWriter* data_writer_with_custom_qos =
                publisher->create_datawriter(topic, custom_qos);
        if (nullptr != data_writer_with_custom_qos)
        {
            // Error
            return;
        }

        // Create a DataWriter with default QoS and a custom Listener.
        // CustomDataWriterListener inherits from DataWriterListener.
        // The value DATAWRITER_QOS_DEFAULT is used to denote the default QoS.
        CustomDataWriterListener custom_listener;
        DataWriter* data_writer_with_default_qos_and_custom_listener =
                publisher->create_datawriter(topic, DATAWRITER_QOS_DEFAULT, &custom_listener);
        if (nullptr != data_writer_with_default_qos_and_custom_listener)
        {
            // Error
            return;
        }
        //!--
    }

    {
        //DDS_CREATE_PROFILE_DATAWRITER
        // First load the XML with the profiles
        DomainParticipantFactory::get_instance()->load_XML_profiles_file("profiles.xml");

        // Create a DataWriter using a profile and no Listener
        DataWriter* data_writer_with_profile =
                publisher->create_datawriter_with_profile(topic, "data_writer_profile");
        if (nullptr != data_writer_with_profile)
        {
            // Error
            return;
        }

        // Create a DataWriter using a profile and a custom Listener.
        // CustomDataWriterListener inherits from DataWriterListener.
        CustomDataWriterListener custom_listener;
        DataWriter* data_writer_with_profile_and_custom_listener =
                publisher->create_datawriter_with_profile(topic, "data_writer_profile", &custom_listener);
        if (nullptr != data_writer_with_profile_and_custom_listener)
        {
            // Error
            return;
        }
        //!--
    }

    {
        //DDS_CHANGE_DATAWRITERQOS
        // Create a DataWriter with default DataWriterQos
        DataWriter* data_writer =
                publisher->create_datawriter(topic, DATAWRITER_QOS_DEFAULT);
        if (nullptr != data_writer)
        {
            // Error
            return;
        }

        // Get the current QoS or create a new one from scratch
        DataWriterQos qos = data_writer->get_qos();

        // Modify QoS attributes
        // (...)

        // Assign the new Qos to the object
        data_writer->set_qos(qos);
        //!--
    }

    {
        //DDS_CHANGE_DATAWRITERQOS_TO_DEFAULT
        // Create a custom DataWriterQos
        DataWriterQos custom_qos;

        // Modify QoS attributes
        // (...)

        // Create a DataWriter with a custom DataWriterQos
        DataWriter* data_writer = publisher->create_datawriter(topic, custom_qos);
        if (nullptr != data_writer)
        {
            // Error
            return;
        }

        // Set the QoS on the DataWriter to the default
        if (data_writer->set_qos(DATAWRITER_QOS_DEFAULT) != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }

        // The previous instruction is equivalent to the following:
        if(data_writer->set_qos(publisher->get_default_datawriter_qos())
                != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }
        //!--
    }

    {
        //DDS_DELETE_DATAWRITER
        // Create a DataWriter
        DataWriter* data_writer =
                publisher->create_datawriter(topic, DATAWRITER_QOS_DEFAULT);
        if (nullptr != data_writer)
        {
            // Error
            return;
        }

        // Use the DataWriter to communicate
        // (...)

        // Delete the DataWriter
        if (publisher->delete_datawriter(data_writer) != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }
        //!--
    }

    {
        //DDS_CHANGE_DEFAULT_DATAWRITERQOS
        // Get the current QoS or create a new one from scratch
        DataWriterQos qos_type1 = publisher->get_default_datawriter_qos();

        // Modify QoS attributes
        // (...)

        // Set as the new default DataWriterQos
        if(publisher->set_default_datawriter_qos(qos_type1) != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }

        // Create a DataWriter with the new default DataWriterQos.
        DataWriter* data_writer_with_qos_type1 =
                publisher->create_datawriter(topic, DATAWRITER_QOS_DEFAULT);
        if (nullptr != data_writer_with_qos_type1)
        {
            // Error
            return;
        }

        // Get the current QoS or create a new one from scratch
        DataWriterQos qos_type2;

        // Modify QoS attributes
        // (...)

        // Set as the new default DataWriterQos
        if(publisher->set_default_datawriter_qos(qos_type2) != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }

        // Create a DataWriter with the new default DataWriterQos.
        DataWriter* data_writer_with_qos_type2 =
                publisher->create_datawriter(topic, DATAWRITER_QOS_DEFAULT);
        if (nullptr != data_writer_with_qos_type2)
        {
            // Error
            return;
        }

        // Resetting the default DataWriterQos to the original default constructed values
        if(publisher->set_default_datawriter_qos(DATAWRITER_QOS_DEFAULT)
                != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }

        // The previous instruction is equivalent to the following
        if(publisher->set_default_datawriter_qos(DataWriterQos())
                != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }
        //!--
    }

    {
        //DDS_DATAWRITER_WRITE
        // Register the data type in the DomainParticipant.
        TypeSupport custom_type_support(new CustomDataType());
        custom_type_support.register_type(participant, custom_type_support.get_type_name());

        // Create a Topic with the registered type.
        Topic* custom_topic =
                participant->create_topic("topic_name", custom_type_support.get_type_name(), TOPIC_QOS_DEFAULT);
        if (nullptr != custom_topic)
        {
            // Error
            return;
        }

        // Create a DataWriter
        DataWriter* data_writer =
                publisher->create_datawriter(custom_topic, DATAWRITER_QOS_DEFAULT);
        if (nullptr != data_writer)
        {
            // Error
            return;
        }

        // Get a data instance
        void* data = custom_type_support->createData();

        // Fill the data values
        // (...)

        // Publish the new value, deduce the instance handle
        if (data_writer->write(data, eprosima::fastrtps::rtps::InstanceHandle_t()) != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }

        // The data instance can be reused to publish new values,
        // but delete it at the end to avoid leaks
        custom_type_support->deleteData(data);
        //!--
    }
}

//DDS_SUBSCRIBER_LISTENER_SPECIALIZATION
class CustomSubscriberListener : public SubscriberListener
{

public:

    CustomSubscriberListener()
    : SubscriberListener()
    {
    }

    virtual ~CustomSubscriberListener()
    {
    }

    virtual void on_data_on_readers(
            Subscriber* sub)
    {
        (void)sub;
        std::cout << "New data available" << std::endl;
    }
};
//!--

void dds_subscriber_examples()
{
    {
        //DDS_CREATE_SUBSCRIBER
        // Create a DomainParticipant in the desired domain
        DomainParticipant* participant =
                DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);
        if (nullptr != participant)
        {
            // Error
            return;
        }

        // Create a Subscriber with default SubscriberQos and no Listener
        // The value SUBSCRIBER_QOS_DEFAULT is used to denote the default QoS.
        Subscriber* subscriber_with_default_qos =
                participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT);
        if (nullptr != subscriber_with_default_qos)
        {
            // Error
            return;
        }

        // A custom SubscriberQos can be provided to the creation method
        SubscriberQos custom_qos;

        // Modify QoS attributes
        // (...)

        Subscriber* subscriber_with_custom_qos =
                participant->create_subscriber(custom_qos);
        if (nullptr != subscriber_with_custom_qos)
        {
            // Error
            return;
        }

        // Create a Subscriber with default QoS and a custom Listener.
        // CustomSubscriberListener inherits from SubscriberListener.
        // The value SUBSCRIBER_QOS_DEFAULT is used to denote the default QoS.
        CustomSubscriberListener custom_listener;
        Subscriber* subscriber_with_default_qos_and_custom_listener =
                participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT, &custom_listener);
        if (nullptr != subscriber_with_default_qos_and_custom_listener)
        {
            // Error
            return;
        }
        //!--
    }

    {
        //DDS_CREATE_PROFILE_SUBSCRIBER
        // First load the XML with the profiles
        DomainParticipantFactory::get_instance()->load_XML_profiles_file("profiles.xml");

        // Create a DomainParticipant in the desired domain
        DomainParticipant* participant =
                DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);
        if (nullptr != participant)
        {
            // Error
            return;
        }

        // Create a Subscriber using a profile and no Listener
        Subscriber* subscriber_with_profile =
                participant->create_subscriber_with_profile("subscriber_profile");
        if (nullptr != subscriber_with_profile)
        {
            // Error
            return;
        }

        // Create a Subscriber using a profile and a custom Listener.
        // CustomSubscriberListener inherits from SubscriberListener.
        CustomSubscriberListener custom_listener;
        Subscriber* subscriber_with_profile_and_custom_listener =
                participant->create_subscriber_with_profile("subscriber_profile", &custom_listener);
        if (nullptr != subscriber_with_profile_and_custom_listener)
        {
            // Error
            return;
        }
        //!--
    }

    {
        //DDS_CHANGE_SUBSCRIBERQOS
        // Create a DomainParticipant in the desired domain
        DomainParticipant* participant =
                DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);
        if (nullptr != participant)
        {
            // Error
            return;
        }

        // Create a Subscriber with default SubscriberQos
        Subscriber* subscriber =
                participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT);
        if (nullptr != subscriber)
        {
            // Error
            return;
        }

        // Get the current QoS or create a new one from scratch
        SubscriberQos qos = subscriber->get_qos();

        // Modify QoS attributes
        qos.entity_factory().autoenable_created_entities = false;

        // Assign the new Qos to the object
        subscriber->set_qos(qos);
        //!--
    }

    {
        //DDS_CHANGE_SUBSCRIBERQOS_TO_DEFAULT
        // Create a DomainParticipant in the desired domain
        DomainParticipant* participant =
                DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);
        if (nullptr != participant)
        {
            // Error
            return;
        }

        // Create a custom SubscriberQos
        SubscriberQos custom_qos;

        // Modify QoS attributes
        // (...)

        // Create a subscriber with a custom SubscriberQos
        Subscriber* subscriber = participant->create_subscriber(custom_qos);
        if (nullptr != subscriber)
        {
            // Error
            return;
        }

        // Set the QoS on the subscriber to the default
        if (subscriber->set_qos(SUBSCRIBER_QOS_DEFAULT) != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }

        // The previous instruction is equivalent to the following:
        if(subscriber->set_qos(participant->get_default_subscriber_qos())
                != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }
        //!--
    }

    {
        //DDS_DELETE_SUBSCRIBER
        // Create a DomainParticipant in the desired domain
        DomainParticipant* participant =
                DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);
        if (nullptr != participant)
        {
            // Error
            return;
        }

        // Create a Subscriber
        Subscriber* subscriber =
                participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT);
        if (nullptr != subscriber)
        {
            // Error
            return;
        }

        // Use the Subscriber to communicate
        // (...)

        // Delete the Subscriber
        if (participant->delete_subscriber(subscriber) != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }
        //!--
    }

    {
        //DDS_CHANGE_DEFAULT_SUBSCRIBERQOS
        // Create a DomainParticipant in the desired domain
        DomainParticipant* participant =
                DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);
        if (nullptr != participant)
        {
            // Error
            return;
        }

        // Get the current QoS or create a new one from scratch
        SubscriberQos qos_type1 = participant->get_default_subscriber_qos();

        // Modify QoS attributes
        // (...)

        // Set as the new default SubscriberQos
        if(participant->set_default_subscriber_qos(qos_type1) != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }

        // Create a Subscriber with the new default SubscriberQos.
        Subscriber* subscriber_with_qos_type1 =
                participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT);
        if (nullptr != subscriber_with_qos_type1)
        {
            // Error
            return;
        }

        // Get the current QoS or create a new one from scratch
        SubscriberQos qos_type2;

        // Modify QoS attributes
        // (...)

        // Set as the new default SubscriberQos
        if(participant->set_default_subscriber_qos(qos_type2) != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }

        // Create a Subscriber with the new default SubscriberQos.
        Subscriber* subscriber_with_qos_type2 =
                participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT);
        if (nullptr != subscriber_with_qos_type2)
        {
            // Error
            return;
        }

        // Resetting the default SubscriberQos to the original default constructed values
        if(participant->set_default_subscriber_qos(SUBSCRIBER_QOS_DEFAULT)
                != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }

        // The previous instruction is equivalent to the following
        if(participant->set_default_subscriber_qos(SubscriberQos())
                != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }
        //!--
    }
}

//DDS_DATAREADER_LISTENER_SPECIALIZATION
class CustomDataReaderListener : public DataReaderListener
{

public:

    CustomDataReaderListener()
    : DataReaderListener()
    {
    }

    virtual ~CustomDataReaderListener()
    {
    }

    virtual void on_data_available(
            DataReader* reader)
    {
        (void)reader;
        std::cout << "Received new data message" << std::endl;
    }

    virtual void on_subscription_matched(
            DataReader* reader,
            const SubscriptionMatchedStatus& info)
    {
        (void)reader;
        if (info.current_count_change == 1)
        {
            std::cout << "Matched a remote DataWriter" << std::endl;
        }
        else if (info.current_count_change == -1)
        {
            std::cout << "Unmatched a remote DataWriter" << std::endl;
        }
    }

    virtual void on_requested_deadline_missed(
            DataReader* reader,
            const eprosima::fastrtps::RequestedDeadlineMissedStatus& info)
    {
        (void)reader, (void)info;
        std::cout << "Some data was not received on time" << std::endl;
    }

    virtual void on_liveliness_changed(
            DataReader* reader,
            const eprosima::fastrtps::LivelinessChangedStatus& info)
    {
        (void)reader;
        if (info.alive_count_change == 1)
        {
            std::cout << "A matched DataWriter has become active" << std::endl;
        }
        else if (info.not_alive_count_change == 1)
        {
            std::cout << "A matched DataWriter has become inactive" << std::endl;
        }
    }

    virtual void on_sample_rejected(
            DataReader* reader,
            const eprosima::fastrtps::SampleRejectedStatus& info)
    {
        (void)reader, (void)info;
        std::cout << "A received data sample was rejected" << std::endl;
    }

    virtual void on_requested_incompatible_qos(
            DataReader* reader,
            const RequestedIncompatibleQosStatus& info)
    {
        (void)reader, (void)info;
        std::cout << "Found a remote Topic with incompatible QoS" << std::endl;
    }

    virtual void on_sample_lost(
            DataReader* reader,
            const SampleLostStatus& info)
    {
        (void)reader, (void)info;
        std::cout << "A data sample was lost and will not be received" << std::endl;
    }
};
//!--

void dds_dataReader_examples()
{
    // Taken out of the examples to avoid bloating them
    DomainParticipant* participant =
            DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);
    Subscriber* subscriber =
            participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT);
    Topic* topic =
            participant->create_topic("TopicName", "DataTypeName", TOPIC_QOS_DEFAULT);

    {
        //DDS_CREATE_DATAREADER
        // Create a DataReader with default DataReaderQos and no Listener
        // The value DATAREADER_QOS_DEFAULT is used to denote the default QoS.
        DataReader* data_reader_with_default_qos =
                subscriber->create_datareader(topic, DATAREADER_QOS_DEFAULT);
        if (nullptr != data_reader_with_default_qos)
        {
            // Error
            return;
        }

        // A custom DataReaderQos can be provided to the creation method
        DataReaderQos custom_qos;

        // Modify QoS attributes
        // (...)

        DataReader* data_reader_with_custom_qos =
                subscriber->create_datareader(topic, custom_qos);
        if (nullptr != data_reader_with_custom_qos)
        {
            // Error
            return;
        }

        // Create a DataReader with default QoS and a custom Listener.
        // CustomDataReaderListener inherits from DataReaderListener.
        // The value DATAREADER_QOS_DEFAULT is used to denote the default QoS.
        CustomDataReaderListener custom_listener;
        DataReader* data_reader_with_default_qos_and_custom_listener =
                subscriber->create_datareader(topic, DATAREADER_QOS_DEFAULT, &custom_listener);
        if (nullptr != data_reader_with_default_qos_and_custom_listener)
        {
            // Error
            return;
        }
        //!--
    }

    {
        //DDS_CREATE_PROFILE_DATAREADER
        // First load the XML with the profiles
        DomainParticipantFactory::get_instance()->load_XML_profiles_file("profiles.xml");

        // Create a DataReader using a profile and no Listener
        DataReader* data_reader_with_profile =
                subscriber->create_datareader_with_profile(topic, "data_reader_profile");
        if (nullptr != data_reader_with_profile)
        {
            // Error
            return;
        }

        // Create a DataReader using a profile and a custom Listener.
        // CustomDataReaderListener inherits from DataReaderListener.
        CustomDataReaderListener custom_listener;
        DataReader* data_reader_with_profile_and_custom_listener =
                subscriber->create_datareader_with_profile(topic, "data_reader_profile", &custom_listener);
        if (nullptr != data_reader_with_profile_and_custom_listener)
        {
            // Error
            return;
        }
        //!--
    }

    {
        //DDS_CHANGE_DATAREADERQOS
        // Create a DataReader with default DataReaderQos
        DataReader* data_reader =
                subscriber->create_datareader(topic, DATAREADER_QOS_DEFAULT);
        if (nullptr != data_reader)
        {
            // Error
            return;
        }

        // Get the current QoS or create a new one from scratch
        DataReaderQos qos = data_reader->get_qos();

        // Modify QoS attributes
        // (...)

        // Assign the new Qos to the object
        data_reader->set_qos(qos);
        //!--
    }

    {
        //DDS_CHANGE_DATAREADERQOS_TO_DEFAULT
        // Create a custom DataReaderQos
        DataReaderQos custom_qos;

        // Modify QoS attributes
        // (...)

        // Create a DataWriter with a custom DataReaderQos
        DataReader* data_reader = subscriber->create_datareader(topic, custom_qos);
        if (nullptr != data_reader)
        {
            // Error
            return;
        }

        // Set the QoS on the DataWriter to the default
        if (data_reader->set_qos(DATAREADER_QOS_DEFAULT) != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }

        // The previous instruction is equivalent to the following:
        if(data_reader->set_qos(subscriber->get_default_datareader_qos())
                != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }
        //!--
    }

    {
        //DDS_DELETE_DATAREADER
        // Create a DataReader
        DataReader* data_reader =
                subscriber->create_datareader(topic, DATAREADER_QOS_DEFAULT);
        if (nullptr != data_reader)
        {
            // Error
            return;
        }

        // Use the DataReader to communicate
        // (...)

        // Delete the DataReader
        if (subscriber->delete_datareader(data_reader) != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }
        //!--
    }

    {
        //DDS_CHANGE_DEFAULT_DATAREADERQOS
        // Get the current QoS or create a new one from scratch
        DataReaderQos qos_type1 = subscriber->get_default_datareader_qos();

        // Modify QoS attributes
        // (...)

        // Set as the new default DataReaderQos
        if(subscriber->set_default_datareader_qos(qos_type1) != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }

        // Create a DataReader with the new default DataReaderQos.
        DataReader* data_reader_with_qos_type1 =
                subscriber->create_datareader(topic, DATAREADER_QOS_DEFAULT);
        if (nullptr != data_reader_with_qos_type1)
        {
            // Error
            return;
        }

        // Get the current QoS or create a new one from scratch
        DataReaderQos qos_type2;

        // Modify QoS attributes
        // (...)

        // Set as the new default DataReaderQos
        if(subscriber->set_default_datareader_qos(qos_type2) != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }

        // Create a DataReader with the new default DataReaderQos.
        DataReader* data_reader_with_qos_type2 =
                subscriber->create_datareader(topic, DATAREADER_QOS_DEFAULT);
        if (nullptr != data_reader_with_qos_type2)
        {
            // Error
            return;
        }

        // Resetting the default DataReaderQos to the original default constructed values
        if(subscriber->set_default_datareader_qos(DATAREADER_QOS_DEFAULT)
                != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }

        // The previous instruction is equivalent to the following
        if(subscriber->set_default_datareader_qos(DataReaderQos())
                != ReturnCode_t::RETCODE_OK)
        {
            // Error
            return;
        }
        //!--
    }
    {
        //DDS_DATAREADER_READ_WAIT
        // Create a DataReader
        DataReader* data_reader =
                subscriber->create_datareader(topic, DATAREADER_QOS_DEFAULT);
        if (nullptr != data_reader)
        {
            // Error
            return;
        }

        // Create a data and SampleInfo instance
        void* data = data_reader->type().create_data();
        SampleInfo info;

        //Define a timeout of 5 seconds
        eprosima::fastrtps::Duration_t timeout (5,0);

        // Loop reading data as it arrives
        // This will make the current threat to be dedicated exclusively to
        // waiting and reading data until the remote DataWriter dies
        while (true)
        {
            if (data_reader->wait_for_unread_message(timeout))
            {
                if (data_reader->take_next_sample(&data, &info) == ReturnCode_t::RETCODE_OK)
                {
                    if (info.instance_state == ALIVE)
                    {
                        // Do something with the data
                        std::cout << "Received new data value for topic "
                                  << topic->get_name()
                                  << std::endl;
                    }
                    else
                    {
                        // If the remote writer is not alive, we exit the reading loop
                        std::cout << "Remote writer for topic "
                                  << topic->get_name()
                                  << " is dead" << std::endl;
                        break;
                    }
                }
            }
            else
            {
                std::cout << "No data this time" << std::endl;
            }
        }

        // The data instance can be reused to retrieve new values,
        // but delete it at the end to avoid leaks
        data_reader->type().delete_data(data);
        //!--
    }
}

//DDS_DATAREADER_READ_LISTENER
class CustomizedDataReaderListener : public DataReaderListener
{

public:

    CustomizedDataReaderListener()
    : DataReaderListener()
    {
    }

    virtual ~CustomizedDataReaderListener()
    {
    }

    virtual void on_data_available(
            DataReader* reader)
    {
        // Create a data and SampleInfo instance
        void* data = reader->type().create_data();
        SampleInfo info;

        // Keep taking data until there is nothing to take
        while (reader->take_next_sample(&data, &info) == ReturnCode_t::RETCODE_OK)
        {
            if (info.instance_state == ALIVE)
            {
                // Do something with the data
                std::cout << "Received new data value for topic "
                          << reader->get_topicdescription()->get_name()
                          << std::endl;
            }
            else
            {
                std::cout << "Remote writer for topic "
                          << reader->get_topicdescription()->get_name()
                          << " is dead" << std::endl;
            }
        }

        // The data instance can be reused to retrieve new values,
        // but delete it at the end to avoid leaks
        reader->type().delete_data(data);
    }
};
//!--

void dds_qos_examples()
{
    {
        //DDS_CHANGE_DEADLINE_QOS_POLICY
        DeadlineQosPolicy deadline;
        //The DeadlineQosPolicy is default constructed with an infinite period.
        //Change the period to 1 second
        deadline.period.seconds = 1;
        deadline.period.nanosec = 0;
        //!--
    }

    {
        //DDS_CHANGE_DURABILITY_QOS_POLICY
        DurabilityQosPolicy durability;
        //The DurabilityQosPolicy is default constructed with kind = VOLATILE_DURABILITY_QOS
        //Change the kind to TRANSIENT_LOCAL_DURABILITY_QOS
        durability.kind = TRANSIENT_LOCAL_DURABILITY_QOS;
        //!--
    }

    {
        //DDS_CHANGE_ENTITY_FACTORY_QOS_POLICY
        EntityFactoryQosPolicy entity_factory;
        //The EntityFactoryQosPolicy is default constructed with autoenable_created_entities = true
        //Change it to false
        entity_factory.autoenable_created_entities = false;
        //!--
    }

    {
        //DDS_CHANGE_GROUP_DATA_QOS_POLICY
        GroupDataQosPolicy group_data;
        //The GroupDataQosPolicy is default constructed with an empty collection
        //Collection is a private member so you need to use getters and setters to access
        //Add data to the collection
        std::vector<eprosima::fastrtps::rtps::octet> vec;
        vec = group_data.data_vec(); // Getter function

        eprosima::fastrtps::rtps::octet val = 3;
        vec.push_back(val);
        group_data.data_vec(vec); //Setter function
        //!--
    }

    {
        //DDS_CHANGE_HISTORY_QOS_POLICY
        HistoryQosPolicy history;
        //The HistoryQosPolicy is default constructed with kind = KEEP_LAST and depth = 1.
        //Change the depth to 20
        history.depth = 20;
        //You can also change the kind to KEEP_ALL but after that the depth will not have effect.
        history.kind = KEEP_ALL_HISTORY_QOS;
        //!--
    }

    {
        //DDS_CHANGE_LIFESPAN_QOS_POLICY
        LifespanQosPolicy lifespan;
        //The LifespanQosPolicy is default constructed with duration set to infinite.
        //Change the duration to 5 s
        lifespan.duration = {5, 0};
        //!--
    }

    {
        //DDS_CHANGE_LIVELINESS_QOS_POLICY
        LivelinessQosPolicy liveliness;
        //The LivelinessQosPolicy is default constructed with kind = AUTOMATIC
        //Change the kind to MANUAL_BY_PARTICIPANT
        liveliness.kind = MANUAL_BY_PARTICIPANT_LIVELINESS_QOS;
        //The LivelinessQosPolicy is default constructed with lease_duration set to infinite
        //Change the lease_duration to 1 second
        liveliness.lease_duration = {1, 0};
        //The LivelinessQosPolicy is default constructed with announcement_period set to infinite
        //Change the announcement_period to 1 ms
        liveliness.announcement_period = {0, 1000000};
        //!--
    }

    {
        //DDS_CHANGE_OWNERSHIP_QOS_POLICY
        OwnershipQosPolicy ownership;
        //The OwnershipQosPolicy is default constructed with kind = SHARED.
        //Change the kind to EXCLUSIVE
        ownership.kind = EXCLUSIVE_OWNERSHIP_QOS;
        //!--
    }

    {
        //DDS_CHANGE_OWNERSHIP_STRENGTH_QOS_POLICY
        OwnershipStrengthQosPolicy ownership_strength;
        //The OwnershipStrengthQosPolicy is default constructed with value 0
        //Change the strength to 10
        ownership_strength.value = 10;
        //!--
    }

    {
        //DDS_CHANGE_PARTITION_QOS_POLICY
        PartitionQosPolicy partitions;
        //The PartitionsQosPolicy is default constructed with max_size = 0.
        //Max_size is a private member so you need to use getters and setters to access
        //Change the max_size to 20
        partitions.set_max_size(20); //Setter function
        //The PartitionsQosPolicy is default constructed with an empty list of partitions
        //Partitions is a private member so you need to use getters and setters to access
        //Add new partitions
        std::vector<std::string> part = partitions.names(); //Getter function
        part.push_back("part1");
        part.push_back("part2");
        partitions.names(part); //Setter function
        //!--
    }

    {
        //DDS_CHANGE_RELIABILITY_QOS_POLICY
        ReliabilityQosPolicy reliability;
        //The ReliabilityQosPolicy is default constructed with kind = BEST_EFFORT
        //Change the kind to RELIABLE
        reliability.kind = RELIABLE_RELIABILITY_QOS;
        //The ReliabilityQosPolicy is default constructed with max_blocking_time = 100ms
        //Change the max_blocking_time to 1s
        reliability.max_blocking_time = {1, 0};
        //!--
    }

    {
        //DDS_CHANGE_RESOURCE_LIMITS_QOS_POLICY
        ResourceLimitsQosPolicy resource_limits;
        //The ResourceLimitsQosPolicy is default constructed with max_samples = 5000
        //Change max_samples to 200
        resource_limits.max_samples = 200;
        //The ResourceLimitsQosPolicy is default constructed with max_instances = 10
        //Change max_instances to 20
        resource_limits.max_instances = 20;
        //The ResourceLimitsQosPolicy is default constructed with max_samples_per_instance = 400
        //Change max_samples_per_instance to 100 as it must be lower than max_samples
        resource_limits.max_samples_per_instance = 100;
        //The ResourceLimitsQosPolicy is default constructed with allocated_samples = 100
        //Change allocated_samples to 50
        resource_limits.allocated_samples = 50;
        //!--
    }

    {
        //DDS_CHANGE_TOPIC_DATA_QOS_POLICY
        //The TopicDataQosPolicy is default constructed with an empty vector.
        TopicDataQosPolicy topic_data;
        std::vector<eprosima::fastrtps::rtps::octet> vec;
        vec = topic_data.data_vec(); // Getter Function

        //Add new octet to topic data vector
        eprosima::fastrtps::rtps::octet val = 3;
        vec.push_back(val);
        topic_data.data_vec(vec); //Setter Function
        //!--
    }


    {
        //DDS_CHANGE_USER_DATA_QOS_POLICY
        //The TopicDataQosPolicy is default constructed with an empty vector.
        UserDataQosPolicy user_data;
        std::vector<eprosima::fastrtps::rtps::octet> vec;
        vec = user_data.data_vec(); // Getter Function

        //Add new octet to topic data vector
        eprosima::fastrtps::rtps::octet val = 3;
        vec.push_back(val);
        user_data.data_vec(vec); //Setter Function
        //!--
    }

    {
        //DDS_CHANGE_DISABLE_POSITIVE_ACKS_QOS_POLICY
        DisablePositiveACKsQosPolicy disable_acks;
        //The DisablePositiveACKsQosPolicy is default constructed with enabled = false
        //Change enabled to true
        disable_acks.enabled = true;
        //The DisablePositiveACKsQosPolicy is default constructed with infinite duration
        //Change the duration to 1 second
        disable_acks.duration = {1, 0};

        //!--
    }

    {
        //DDS_CHANGE_PARTICIPANT_RESOURCE_LIMITS_QOS_POLICY
        ParticipantResourceLimitsQos participant_limits;
        //Set the maximum size of participant resource limits collection to 3 and it allocation configuration to fixed size
        participant_limits.participants = eprosima::fastrtps::ResourceLimitedContainerConfig::fixed_size_configuration(3u);
        //Set the maximum size of reader's resource limits collection to 2 and its allocation configuration to fixed size
        participant_limits.readers = eprosima::fastrtps::ResourceLimitedContainerConfig::fixed_size_configuration(2u);
        //Set the maximum size of writer's resource limits collection to 1 and its allocation configuration to fixed size
        participant_limits.writers = eprosima::fastrtps::ResourceLimitedContainerConfig::fixed_size_configuration(1u);
        //Set the maximum size of the partition data to 256
        participant_limits.data_limits.max_partitions = 256u;
        //Set the maximum size of the user data to 256
        participant_limits.data_limits.max_user_data = 256u;
        //Set the maximum size of the properties data to 512
        participant_limits.data_limits.max_properties = 512u;
        //!--
    }

    {
        //DDS_CHANGE_PROPERTY_POLICY_QOS
        PropertyPolicyQos property_policy;
        //Add new property for the Auth:PKI-DH plugin
        property_policy.properties().emplace_back("dds.sec.auth.plugin", "builtin.PKI-DH");
        //Add new property for the Access:Permissions plugin
        property_policy.properties().emplace_back(eprosima::fastrtps::rtps::Property("dds.sec.access.plugin", "builtin.Access-Permissions"));
        //!--
    }

    {
        //DDS_CHANGE_PUBLISH_MODE_QOS
        PublishModeQosPolicy publish_mode;
        //The PublishModeQosPolicy is default constructed with kind = SYNCHRONOUS
        //Change the kind to ASYNCHRONOUS
        publish_mode.kind = ASYNCHRONOUS_PUBLISH_MODE;
        //!--
    }

    {
        //DDS_CHANGE_READER_RESOURCE_LIMITS_QOS
        ReaderResourceLimitsQos reader_limits;
        //Set the maximum size for writer matched resource limits collection to 1 and its allocation configuration to fixed size
        reader_limits.matched_publisher_allocation = eprosima::fastrtps::ResourceLimitedContainerConfig::fixed_size_configuration(1u);
        //!--
    }

    {
        //DDS_CHANGE_WRITER_RESOURCE_LIMITS_QOS
        WriterResourceLimitsQos writer_limits;
        //Set the maximum size for reader matched resource limits collection to 3 and its allocation configuration to fixed size
        writer_limits.matched_subscriber_allocation = eprosima::fastrtps::ResourceLimitedContainerConfig::fixed_size_configuration(3u);
        //!--
    }

    {
        //DDS_CHANGE_RTPS_ENDPOINT_QOS
        RTPSEndpointQos endpoint;
        //Add new unicast locator with port 7800
        eprosima::fastrtps::rtps::Locator_t new_unicast_locator;
        new_unicast_locator.port = 7800;
        endpoint.unicast_locator_list.push_back(new_unicast_locator);
        //Add new multicast locator with IP 239.255.0.4 and port 7900
        eprosima::fastrtps::rtps::Locator_t new_multicast_locator;
        eprosima::fastrtps::rtps::IPLocator::setIPv4(new_multicast_locator, "239.255.0.4");
        new_multicast_locator.port = 7900;
        endpoint.multicast_locator_list.push_back(new_multicast_locator);
        //Set 3 as user defined id
        endpoint.user_defined_id = 3;
        //Set 4 as entity id
        endpoint.entity_id = 4;
        //The RTPSEndpointQos is default constructed with history_memory_policy = PREALLOCATED
        //Change the history_memory_policy to DYNAMIC_RESERVE
        endpoint.history_memory_policy = eprosima::fastrtps::rtps::DYNAMIC_RESERVE_MEMORY_MODE;
        //!--
    }

    {
        //DDS_CHANGE_RTPS_RELIABLE_WRITER_QOS
        RTPSReliableWriterQos reliable_writer_qos;
        //The RTPSReliableWriterQos is default constructed with initialHeartbeatDelay = 12 ms
        //Change the initialHeartbeatDelay to 20 nanoseconds
        reliable_writer_qos.times.initialHeartbeatDelay = {0, 20};
        //The RTPSReliableWriterQos is default constructed with heartbeatPeriod = 3 s
        //Change the heartbeatPeriod to 5 seconds
        reliable_writer_qos.times.heartbeatPeriod = {5, 0};
        //The RTPSReliableWriterQos is default constructed with nackResponseDelay = 5 ms
        //Change the nackResponseDelay to 10 nanoseconds
        reliable_writer_qos.times.nackResponseDelay = {0, 10};
        //The RTPSReliableWriterQos is default constructed with nackSupressionDuration = 0 s
        //Change the nackSupressionDuration to 20 nanoseconds
        reliable_writer_qos.times.nackSupressionDuration = {0, 20};
        //You can also change the DisablePositiveACKsQosPolicy. For further details see DisablePositiveACKsQosPolicy section.
        reliable_writer_qos.disable_positive_acks.enabled = true;
        //!--
    }

    {
        //DDS_CHANGE_RTPS_RELIABLE_READER_QOS
        RTPSReliableReaderQos reliable_reader_qos;
        //The RTPSReliableReaderQos is default constructed with initialAcknackDelay = 70 ms
        //Change the initialAcknackDelay to 70 nanoseconds
        reliable_reader_qos.times.initialAcknackDelay = {0, 70};
        //The RTPSReliableWriterQos is default constructed with heartbeatResponseDelay = 5 ms
        //Change the heartbeatResponseDelay to 5 nanoseconds
        reliable_reader_qos.times.heartbeatResponseDelay = {0, 5};
        //You can also change the DisablePositiveACKsQosPolicy. For further details see DisablePositiveACKsQosPolicy section.
        reliable_reader_qos.disable_positive_ACKs.enabled = true;
        //!--
    }

    {
        //DDS_CHANGE_TRANSPORT_CONFIG_QOS
        TransportConfigQos transport;
        //Add new transport to the list of user transports
        std::shared_ptr<eprosima::fastdds::rtps::UDPv4TransportDescriptor> descriptor =
                std::make_shared<eprosima::fastdds::rtps::UDPv4TransportDescriptor>();
        descriptor->sendBufferSize = 9126;
        descriptor->receiveBufferSize = 9126;
        transport.user_transports.push_back(descriptor);
        //Set use_builtin_transports to false
        transport.use_builtin_transports = false;
        //!--
    }

    {
        //DDS_CHANGE_WIRE_PROTOCOL_CONFIG_QOS
        eprosima::fastdds::dds::WireProtocolConfigQos wire_protocol;
        //Set the guid prefix
        std::istringstream("72.61.73.70.66.61.72.6d.74.65.73.74") >> wire_protocol.prefix;
        //Configure Builtin Attributes
        wire_protocol.builtin.discovery_config.discoveryProtocol = eprosima::fastrtps::rtps::DiscoveryProtocol_t::SERVER;
        //Add locator to unicast list
        eprosima::fastrtps::rtps::Locator_t server_locator;
        eprosima::fastrtps::rtps::IPLocator::setIPv4(server_locator, "192.168.10.57");
        server_locator.port = 56542;
        wire_protocol.builtin.metatrafficUnicastLocatorList.push_back(server_locator);
        // Limit to 300kb per second.
        eprosima::fastrtps::rtps::ThroughputControllerDescriptor slowPublisherThroughputController{300000, 1000};
        wire_protocol.throughput_controller = slowPublisherThroughputController;
        //Add locator to default unicast locator list
        eprosima::fastrtps::rtps::Locator_t unicast_locator;
        eprosima::fastrtps::rtps::IPLocator::setIPv4(unicast_locator, 192, 168, 1, 41);
        unicast_locator.port = 7400;
        wire_protocol.default_unicast_locator_list.push_back(unicast_locator);
        //Add locator to default multicast locator list
        eprosima::fastrtps::rtps::Locator_t multicast_locator;
        eprosima::fastrtps::rtps::IPLocator::setIPv4(multicast_locator, 192, 168, 1, 41);
        multicast_locator.port = 7400;
        wire_protocol.default_multicast_locator_list.push_back(multicast_locator);
        //!--
    }

    {
        //DDS_CHANGE_DATA_REPRESENTATION_QOS
        DataRepresentationQosPolicy data_representation;
        //Add XCDR v1 data representation to the list of valid representations
        data_representation.m_value.push_back(DataRepresentationId_t::XCDR_DATA_REPRESENTATION);
        //Add XML data representation to the list of valid representations
        data_representation.m_value.push_back(DataRepresentationId_t::XML_DATA_REPRESENTATION);
        //!--
    }

    {
        //DDS_CHANGE_TYPE_CONSISTENCY_ENFORCEMENT_QOS
        TypeConsistencyEnforcementQosPolicy type_enforcement;
        //The TypeConsistencyEnforcementQosPolicy is default constructed with kind = ALLOW_TYPE_COERCION
        //Change the kind to DISALLOW_TYPE_COERCION
        type_enforcement.m_kind = TypeConsistencyKind::DISALLOW_TYPE_COERCION;
        //Configures the system to ignore the sequence sizes in assignations
        type_enforcement.m_ignore_sequence_bounds = true;
        //Configures the system to ignore the string sizes in assignations
        type_enforcement.m_ignore_string_bounds = true;
        //Configures the system to ignore the member names. Members with same ID could have different names
        type_enforcement.m_ignore_member_names = true;
        //Configures the system to allow type widening
        type_enforcement.m_prevent_type_widening = false;
        //Configures the system to not use the complete Type Information in entities match process
        type_enforcement.m_force_type_validation = false;
        //!--
    }

    {
        //DDS_CHANGE_TYPE_CONSISTENCY_QOS
        TypeConsistencyQos consistency_qos;
        //You can change the DataRepresentationQosPolicy. For further details see DataRepresentationQosPolicySection section.
        consistency_qos.representation.m_value.push_back(DataRepresentationId_t::XCDR2_DATA_REPRESENTATION);
        //You can change the TypeConsistencyEnforcementQosPolicy. For further details see TypeConsistencyEnforcementQosPolicy section.
        consistency_qos.type_consistency.m_kind = TypeConsistencyKind::ALLOW_TYPE_COERCION;
        //!--
    }
}

void dds_dynamic_types_examples ()
{
    {
        //DDS_TYPELOOKUP_SERVICE_ENABLING
        DomainParticipantQos qos;
        qos.wire_protocol().builtin.typelookup_config.use_client = true;
        qos.wire_protocol().builtin.typelookup_config.use_server = true;
        //!--
    }
}

void log_examples()
{
    //LOG_MESSAGES
    logInfo(DOCUMENTATION_CATEGORY, "This is an info message");
    logWarning(DOCUMENTATION_CATEGORY, "This is an warning message");
    logError(DOCUMENTATION_CATEGORY, "This is an error message");
    //!--

    //LOG_SET_GET_VERBOSITY
    // Set log verbosity level to Log::Kind::Info
    Log::SetVerbosity(Log::Kind::Info);

    // Get log verbosity level
    Log::Kind verbosity_level = Log::GetVerbosity();
    //!--

    //LOG_REPORT_FILENAMES
    // Enable file name and line number reporting
    Log::ReportFilenames(true);

    // Disable file name and line number reporting
    Log::ReportFilenames(false);
    //!--

    //LOG_REPORT_FUNCTIONS
    // Enable function name reporting
    Log::ReportFunctions(true);

    // Disable function name reporting
    Log::ReportFunctions(false);
    //!--

    //LOG_CATEGORY_FILTER
    // Set filter using regular expression
    Log::SetCategoryFilter(std::regex("(CATEGORY_1)|(CATEGORY_2)"));

    // Would be consumed
    logError(CATEGORY_1, "First log entry");
    // Would be consumed
    logError(CATEGORY_2, "Second log entry");
    // Would NOT be consumed
    logError(CATEGORY_3, "Third log entry");
    //!--

    //LOG_FILENAME_FILTER
    // Filename: example.cpp

    // Enable file name and line number reporting
    Log::ReportFilenames(true);

    // Set filter using regular expression so filename must match "example"
    Log::SetFilenameFilter(std::regex("example"));
    // Would be consumed
    logError(CATEGORY, "First log entry");

    // Set filter using regular expression so filename must match "other"
    Log::SetFilenameFilter(std::regex("other"));
    // Would NOT be consumed
    logError(CATEGORY, "Second log entry");
    //!--

    //LOG_CONTENT_FILTER
    // Set filter using regular expression so message component must match "First"
    Log::SetErrorStringFilter(std::regex("First"));
    // Would be consumed
    logError(CATEGORY, "First log entry");
    // Would NOT be consumed
    logError(CATEGORY, "Second log entry");
    //!--

    //LOG_REGISTER_CONSUMER
    // Create a FileConsumer consumer that logs entries in "archive.log"
    std::unique_ptr<FileConsumer> file_consumer(new FileConsumer("archive.log"));
    // Register the consumer. Log entries will be logged to STDOUT and "archive.log"
    Log::RegisterConsumer(std::move(file_consumer));
    //!--

    //LOG_CLEAR_CONSUMERS
    // Clear all the consumers. Log entries are discarded upon consumption.
    Log::ClearConsumers();
    //!--

    //LOG_STDOUT_CONSUMER
    // Create a StdoutConsumer consumer that logs entries to stdout stream.
    std::unique_ptr<StdoutConsumer> stdout_consumer(new StdoutConsumer());

    // Register the consumers.
    Log::RegisterConsumer(std::move(stdout_consumer));
    //!--

    //LOG_FILE_CONSUMER
    // Create a FileConsumer consumer that logs entries in "archive_1.log", opening the file in "write" mode.
    std::unique_ptr<FileConsumer> write_file_consumer(new FileConsumer("archive_1.log", false));

    // Create a FileConsumer consumer that logs entries in "archive_2.log", opening the file in "append" mode.
    std::unique_ptr<FileConsumer> append_file_consumer(new FileConsumer("archive_2.log", true));

    // Register the consumers.
    Log::RegisterConsumer(std::move(write_file_consumer));
    Log::RegisterConsumer(std::move(append_file_consumer));
    //!--

    //LOG_FLUSH_AND_KILL
    // Block current thread until the log queue is empty.
    Log::Flush();

    // Stop the loggin thread and free its resources.
    Log::KillThread();
    //!--
}