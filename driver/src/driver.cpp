#include <openvr/openvr_driver.h>
#include <cstring>
#include <memory>
#include <string>
#include <thread>

#include "driverlog.h"
#include "hid.h"

#if defined(_MSC_VER)
    #define HMD_DLL_EXPORT extern "C" __declspec(dllexport)
#elif defined (__GNUC__)
    #define HMD_DLL_EXPORT extern "C" __attribute__((visibility("default"))) 
#endif

inline vr::HmdQuaternion_t HmdQuaternion_Init(double w, double x, double y, double z) {
    vr::HmdQuaternion_t q;
    q.w = w;
    q.x = x;
    q.y = y;
    q.z = z;

    return q;
}

static const char * const k_pch_c2vr_Section = "driver_c2vr";
static const char * const k_pch_c2vr_SerialNumber_String = "serialNumber";
static const char * const k_pch_c2vr_ModelNumber_String = "modelNumber";
static const char * const k_pch_c2vr_WindowX_Int32 = "windowX";
static const char * const k_pch_c2vr_WindowY_Int32 = "windowY";
static const char * const k_pch_c2vr_WindowWidth_Int32 = "windowWidth";
static const char * const k_pch_c2vr_WindowHeight_Int32 = "windowHeight";
static const char * const k_pch_c2vr_RenderWidth_Int32 = "renderWidth";
static const char * const k_pch_c2vr_RenderHeight_Int32 = "renderHeight";
static const char * const k_pch_c2vr_SecondsFromVsyncToPhotons_Float = "secondsFromVsyncToPhotons";
static const char * const k_pch_c2vr_DisplayFrequency_Float = "displayFrequency";

CHIDHandler g_hidHandler;

class CWatchdogDriver_Sample : public vr::IVRWatchdogProvider
{
public:
	CWatchdogDriver_Sample()
	{
		m_pWatchdogThread = nullptr;
	}

	virtual vr::EVRInitError Init( vr::IVRDriverContext *pDriverContext ) ;
	virtual void Cleanup() ;

private:
	std::thread *m_pWatchdogThread;
};

CWatchdogDriver_Sample g_watchdogDriverNull;
bool g_bExiting = false;

void WatchdogThreadFunction(  )
{
	while ( !g_bExiting )
	{
                std::this_thread::sleep_for( std::chrono::seconds( 5 ) );
                vr::VRWatchdogHost()->WatchdogWakeUp( vr::TrackedDeviceClass_HMD );
	}
}

vr::EVRInitError CWatchdogDriver_Sample::Init( vr::IVRDriverContext *pDriverContext )
{
	VR_INIT_WATCHDOG_DRIVER_CONTEXT( pDriverContext );
	InitDriverLog( vr::VRDriverLog() );

	g_bExiting = false;
	m_pWatchdogThread = new std::thread( WatchdogThreadFunction );
	if ( !m_pWatchdogThread )
	{
		DriverLog( "Unable to create watchdog thread\n");
		return vr::VRInitError_Driver_Failed;
	}

	return vr::VRInitError_None;
}


void CWatchdogDriver_Sample::Cleanup()
{
	g_bExiting = true;
	if ( m_pWatchdogThread )
	{
		m_pWatchdogThread->join();
		delete m_pWatchdogThread;
		m_pWatchdogThread = nullptr;
	}

	CleanupDriverLog();
}



class CHMDDriver : public vr::ITrackedDeviceServerDriver, public vr::IVRDisplayComponent {
public:
    CHMDDriver() {
        m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
        m_propertyContainer = vr::k_ulInvalidPropertyContainer;

        m_IPD = vr::VRSettings()->GetFloat(vr::k_pch_SteamVR_Section, vr::k_pch_SteamVR_IPD_Float);

        char buf[2048];
        vr::VRSettings()->GetString(k_pch_c2vr_Section, k_pch_c2vr_SerialNumber_String, buf, sizeof(buf));
        m_serialNumber = buf;
        
        vr::VRSettings()->GetString(k_pch_c2vr_Section, k_pch_c2vr_ModelNumber_String, buf, sizeof(buf));
        m_modelNumber = buf;

        
        m_windowX = vr::VRSettings()->GetInt32( k_pch_c2vr_Section, k_pch_c2vr_WindowX_Int32 );
        m_windowY = vr::VRSettings()->GetInt32( k_pch_c2vr_Section, k_pch_c2vr_WindowY_Int32 );
        m_windowW = vr::VRSettings()->GetInt32( k_pch_c2vr_Section, k_pch_c2vr_WindowWidth_Int32 );
        m_windowH = vr::VRSettings()->GetInt32( k_pch_c2vr_Section, k_pch_c2vr_WindowHeight_Int32 );
        m_renderW = vr::VRSettings()->GetInt32( k_pch_c2vr_Section, k_pch_c2vr_RenderWidth_Int32 );
        m_renderH = vr::VRSettings()->GetInt32( k_pch_c2vr_Section, k_pch_c2vr_RenderHeight_Int32 );
        m_secondsFromVsyncToPhotons = vr::VRSettings()->GetFloat( k_pch_c2vr_Section, k_pch_c2vr_SecondsFromVsyncToPhotons_Float );
        m_displayFreq = vr::VRSettings()->GetFloat( k_pch_c2vr_Section, k_pch_c2vr_DisplayFrequency_Float );
    }

    vr::EVRInitError Activate(uint32_t unObjectId) {
        m_unObjectId = unObjectId;
        m_propertyContainer = vr::VRProperties()->TrackedDeviceToPropertyContainer(unObjectId);


        vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_ModelNumber_String, m_modelNumber.c_str() );
        vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_RenderModelName_String, m_modelNumber.c_str() );
        vr::VRProperties()->SetFloatProperty(m_propertyContainer, vr::Prop_UserIpdMeters_Float, m_IPD );
        vr::VRProperties()->SetFloatProperty(m_propertyContainer, vr::Prop_UserHeadToEyeDepthMeters_Float, 0.f );
        vr::VRProperties()->SetFloatProperty(m_propertyContainer, vr::Prop_DisplayFrequency_Float, m_displayFreq);
        vr::VRProperties()->SetFloatProperty(m_propertyContainer, vr::Prop_SecondsFromVsyncToPhotons_Float, m_secondsFromVsyncToPhotons);

        // return a constant that's not 0 (invalid) or 1 (reserved for Oculus)
        vr::VRProperties()->SetUint64Property(m_propertyContainer, vr::Prop_CurrentUniverseId_Uint64, 2);

        // avoid "not fullscreen" warnings from vrmonitor
        vr::VRProperties()->SetBoolProperty(m_propertyContainer, vr::Prop_IsOnDesktop_Bool, false);

        return vr::VRInitError_None;
    }

    void Deactivate() {
        m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
    }

    void EnterStandby() {}
    void PowerOff() {}
    
    void *GetComponent(const char *componentNameVer) {
        if(!strcasecmp(componentNameVer, vr::IVRDisplayComponent_Version))
            return (vr::IVRDisplayComponent*)this;

        return NULL;
    }

    void DebugRequest(const char *, char *respBuffer, uint32_t respBufferSz) {
        if(respBufferSz > 0)
            respBuffer[0] = 0;
    }
    
    vr::DriverPose_t GetPose() {
        //vr::DriverPose_t pose = { 0 };
        vr::DriverPose_t pose; 

        if(this->m_unObjectId != vr::k_unTrackedDeviceIndexInvalid) {
            pose.poseIsValid = true;
            pose.result = vr::TrackingResult_Running_OK;
            pose.deviceIsConnected = true;
        } else {
            pose.poseIsValid = false;
            pose.result = vr::TrackingResult_Uninitialized;
            pose.deviceIsConnected = false;
        }

        pose.qWorldFromDriverRotation = HmdQuaternion_Init( 1, 0, 0, 0 );
        pose.qDriverFromHeadRotation = HmdQuaternion_Init( 1, 0, 0, 0 );
        
        return pose;
    }

    void RunFrame() {
        if ( m_unObjectId != vr::k_unTrackedDeviceIndexInvalid )
        {
                vr::VRServerDriverHost()->TrackedDevicePoseUpdated( m_unObjectId, GetPose(), sizeof( vr::DriverPose_t ) );
        }
    }
    
    const char *GetSerialNumber() { return m_serialNumber.c_str(); };

    // Display methods
    
    virtual void GetWindowBounds( int32_t *pnX, int32_t *pnY, uint32_t *pnW, uint32_t *pnH) {
        *pnX = m_windowX;
        *pnY = m_windowY;
        *pnW = m_windowW;
        *pnH = m_windowH;
    }

    void GetRecommendedRenderTargetSize(uint32_t *pnWidth, uint32_t *pnHeight) {
        *pnWidth = m_renderW;
        *pnHeight= m_renderH;
    }

    void GetEyeOutputViewport(vr::EVREye eye, uint32_t *pnX, uint32_t *pnY, uint32_t *pnW, uint32_t *pnH) {
        *pnY = 0;
        *pnW = m_windowW / 2;
        *pnH = m_windowH;

        if(eye == vr::Eye_Left)
            *pnX = 0;
        else
            *pnX = m_windowW / 2;
    }
    
    void GetProjectionRaw(vr::EVREye eye, float *pfLeft, float *pfRight, float *pfTop, float *pfBottom) {
        // Reversed projection - TODO: Add reverse video to device settings
        *pfLeft = 1.0;
        *pfRight = -1.0;
        *pfTop = 1.0;
        *pfBottom = -1.0;	
    }

    vr::DistortionCoordinates_t ComputeDistortion(vr::EVREye eye, float fU, float fV) {
        vr::DistortionCoordinates_t coordinates;
        coordinates.rfBlue[0] = fU;
        coordinates.rfBlue[1] = fV;
        coordinates.rfGreen[0] = fU;
        coordinates.rfGreen[1] = fV;
        coordinates.rfRed[0] = fU;
        coordinates.rfRed[1] = fV;
        return coordinates;
    }

    bool IsDisplayOnDesktop() { return true; }
    bool IsDisplayRealDisplay() { return false; }


private:
    std::string m_serialNumber;
    std::string m_modelNumber;

    vr::TrackedDeviceIndex_t m_unObjectId;
    vr::PropertyContainerHandle_t m_propertyContainer;

    float m_IPD, m_displayFreq, m_secondsFromVsyncToPhotons;
    int32_t m_windowX, m_windowY, m_windowW, m_windowH;
    int32_t m_renderW, m_renderH;
};
    
class CServerDriver : public vr::IServerTrackedDeviceProvider {
public:
    vr::EVRInitError Init(vr::IVRDriverContext *driverContext) {
        VR_INIT_SERVER_DRIVER_CONTEXT(driverContext);
        InitDriverLog(vr::VRDriverLog());

        DriverLog("Initializing Device Provider...\n");

        m_hmd = new CHMDDriver();
        vr::VRServerDriverHost()->TrackedDeviceAdded(m_hmd->GetSerialNumber(), vr::TrackedDeviceClass_HMD, m_hmd);

        return vr::VRInitError_None;
    }
    
    void Cleanup() {
        CleanupDriverLog();
    }

    const char * const *GetInterfaceVersions() { 
        return vr::k_InterfaceVersions; 
    };

    void RunFrame() {
        if(m_hmd)
            m_hmd->RunFrame();
    };

    // Power management
    bool ShouldBlockStandbyMode() { return false; }
    void EnterStandby() {};
    void LeaveStandby() {};

private:
    CHMDDriver *m_hmd;
};


CServerDriver g_serverDriver;

HMD_DLL_EXPORT 
void *HmdDriverFactory( const char *pInterfaceName, int *pReturnCode )
{
    
    if( 0 == strcmp( vr::IServerTrackedDeviceProvider_Version, pInterfaceName ) )
    {
            return &g_serverDriver;
    }
    if( 0 == strcmp( vr::IVRWatchdogProvider_Version, pInterfaceName ) )
    {
            return &g_watchdogDriverNull; 
    }


    if( pReturnCode )
            *pReturnCode = vr::VRInitError_Init_InterfaceNotFound;

    return NULL;
}
