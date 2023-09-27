//
// Created by tanja on 25.09.23.
//
#include "kinect_handler.h"
#include "../utils/kinect_variables.h"
#include "input.h"

#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include <algorithm>

// xml to initialize OpenNI
#define SAMPLE_XML_FILE "../../../Data/Sample-Tracking.xml"
#define SAMPLE_XML_FILE_LOCAL "Sample-Tracking.xml"

XnVSelectableSlider2D *g_pMainSlider2D;
XnVFlowRouter *g_pMainFlowRouter;
bool just_jumped = false;

xn::DepthGenerator g_DepthGenerator;
xn::UserGenerator g_UserGenerator;

XnUserID g_nPlayer = 0;
XnBool g_bCalibrated = FALSE;

XnFloat initial_y = 0.0;
XnFloat initial_z = 0.0;

XnBool g_bPause = false;

bool use_hand = false;

//-----------------------------------------------------------------------------
// Callbacks for kinect body
//-----------------------------------------------------------------------------


XnBool AssignPlayer(XnUserID user)
{
    if (g_nPlayer != 0)
        return FALSE;

    XnPoint3D com;
    g_UserGenerator.GetCoM(user, com);
    if (com.Z == 0)
        return FALSE;

    printf("Matching for existing calibration\n");
    g_UserGenerator.GetSkeletonCap().LoadCalibrationData(user, 0);
    g_UserGenerator.GetSkeletonCap().StartTracking(user);
    g_nPlayer = user;
    return TRUE;

}

void XN_CALLBACK_TYPE NewUser(xn::UserGenerator& generator, XnUserID user, void* pCookie)
{
    if (!g_bCalibrated) // check on player0 is enough
    {
        generator.GetSkeletonCap().StartTracking(user);
        return;
    }

    AssignPlayer(user);
}

void FindPlayer()
{
    if (g_nPlayer != 0)
    {
        return;
    }
    XnUserID aUsers[20];
    XnUInt16 nUsers = 20;
    g_UserGenerator.GetUsers(aUsers, nUsers);

    for (int i = 0; i < nUsers; ++i)
    {
        if (AssignPlayer(aUsers[i]))
            return;
    }
}

void LostPlayer()
{
    g_nPlayer = 0;
    FindPlayer();

}

void XN_CALLBACK_TYPE LostUser(xn::UserGenerator& generator, XnUserID user, void* pCookie)
{
    printf("Lost user %d\n", user);
    if (g_nPlayer == user)
    {
        LostPlayer();
    }
}

void XN_CALLBACK_TYPE CalibrationStarted(xn::SkeletonCapability& skeleton, XnUserID user, void* cxt)
{
}

void XN_CALLBACK_TYPE CalibrationCompleted(xn::SkeletonCapability& skeleton, XnUserID user, XnCalibrationStatus eStatus, void* cxt)
{
    printf("User found! You can start playing :) \n");
    if (eStatus == XN_CALIBRATION_STATUS_OK)
    {
        if (!g_bCalibrated)
        {
            g_UserGenerator.GetSkeletonCap().SaveCalibrationData(user, 0);
            g_nPlayer = user;
            g_UserGenerator.GetSkeletonCap().StartTracking(user);
            g_bCalibrated = TRUE;
        }

        XnUserID aUsers[10];
        XnUInt16 nUsers = 10;
        g_UserGenerator.GetUsers(aUsers, nUsers);
        for (int i = 0; i < nUsers; ++i)
            g_UserGenerator.GetPoseDetectionCap().StopPoseDetection(aUsers[i]);

    }
}

// this function is called each frame
void glutDisplay (void)
{
    xn::SceneMetaData sceneMD;
    xn::DepthMetaData depthMD;
    g_DepthGenerator.GetMetaData(depthMD);

    if (!g_bPause)
    {
        // Read next available data
        context.WaitOneUpdateAll(g_DepthGenerator);
    }

    if (g_nPlayer != 0)
    {
        XnPoint3D com;
        g_UserGenerator.GetCoM(g_nPlayer, com);
        if (com.Z == 0)
        {
            g_nPlayer = 0;
            FindPlayer();
        }

        XnSkeletonJointTransformation jointData;
        g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(g_nPlayer, XN_SKEL_TORSO, jointData);
        if (initial_y == 0.0) {
            initial_y = jointData.position.position.Y;
            initial_z = jointData.position.position.Z;
        }

        XnFloat scaled_z = std::max(-1.0f, std::min(1.0f, -(jointData.position.position.Z - initial_z) / 700));
        printf("%6.1f    (%6.1f)\n", scaled_z, jointData.position.position.Z);
        if (jointData.position.position.Y > initial_y + 100 + 10 * scaled_z){
            printf("JUMP %6.1f (init %6.1f)\n", jointData.position.position.Y, initial_y);
        }
    }

}

#define CHECK_RC(rc, what)											\
	if (rc != XN_STATUS_OK)											\
	{																\
		printf("%s failed: %s\n", what, xnGetStatusString(rc));		\
		return rc;													\
	}

#define CHECK_ERRORS(rc, errors, what)		\
	if (rc == XN_STATUS_NO_NODE_PRESENT)	\
{										\
	XnChar strError[1024];				\
	errors.ToString(strError, 1024);	\
	printf("%s\n", strError);			\
	return (rc);						\
}

//-----------------------------------------------------------------------------
// Callbacks for kinect hand
//-----------------------------------------------------------------------------

void XN_CALLBACK_TYPE MainSlider_OnValueChange(XnFloat xValue, XnFloat yValue,
                                               void *cxt) {
    printf("fValue %6.2f %6.2f\n", xValue * 2.0f - 1.0f, yValue * 2.0f - 1.0f);
    float scaled_x_value = xValue * 2.0f - 1.0f;
    float scaled_y_value = yValue * 2.0f - 1.0f;
    horizontal_axis = scaled_x_value;
    do_kinect_jump = false;
    if (scaled_y_value > 0.3) {
        if (!just_jumped) {
            do_kinect_jump = true;
            just_jumped = true;
        }
    } else {
        just_jumped = false;
    }
}

// Callback for when the focus is in progress
void XN_CALLBACK_TYPE SessionProgress(const XnChar *strFocus,
                                      const XnPoint3D &ptFocusPoint,
                                      XnFloat fProgress, void *UserCxt) {}

// callback for session start
void XN_CALLBACK_TYPE SessionStart(const XnPoint3D &ptFocusPoint,
                                   void *UserCxt) {
    g_pMainFlowRouter->SetActive(g_pMainSlider2D);
    printf("Player was found! The game starts soon...");
    kinect_init = true;
}
// Callback for session end
void XN_CALLBACK_TYPE SessionEnd(void *UserCxt) {
    printf("Session ended. Please perform focus gesture to start session\n");
    g_pMainFlowRouter->SetActive(NULL);
}

XnBool fileExists(const char *fn) {
    XnBool exists;
    xnOSDoesFileExist(fn, &exists);
    return exists;
}

int initKinect() {
    xn::Context context;
    xn::ScriptNode scriptNode;
    XnVSessionGenerator *pSessionGenerator;

    // Create context
    const char *fn = NULL;
    if (fileExists(SAMPLE_XML_FILE))
        fn = SAMPLE_XML_FILE;
    else if (fileExists(SAMPLE_XML_FILE_LOCAL))
        fn = SAMPLE_XML_FILE_LOCAL;
    else {
        printf("Could not find '%s' nor '%s'. Aborting.\n", SAMPLE_XML_FILE,
               SAMPLE_XML_FILE_LOCAL);
        return XN_STATUS_ERROR;
    }
    XnStatus rc = context.InitFromXmlFile(fn, scriptNode);
    if (rc != XN_STATUS_OK) {
        printf("Couldn't initialize: %s\n", xnGetStatusString(rc));
        return 1;
    }
    rc = context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_DepthGenerator);
    CHECK_RC(rc, "Find depth generator");
    rc = context.FindExistingNode(XN_NODE_TYPE_USER, g_UserGenerator);
    CHECK_RC(rc, "Find user generator");
    if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_SKELETON) ||
        !g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION))
    {
        printf("User generator doesn't support either skeleton or pose detection.\n");
        return XN_STATUS_ERROR;
    }

    if (use_hand) {
        // Create the Session Manager
        pSessionGenerator = new XnVSessionManager();
        rc = ((XnVSessionManager *)pSessionGenerator)
                 ->Initialize(&context, "Click", "RaiseHand");
        if (rc != XN_STATUS_OK) {
            printf("Session Manager couldn't initialize: %s\n",
                   xnGetStatusString(rc));
            delete pSessionGenerator;
            return 1;
        }

        // Initialization done. Start generating
        context.StartGeneratingAll();

        // Register session callbacks
        pSessionGenerator->RegisterSession(NULL, &SessionStart, &SessionEnd,
                                           &SessionProgress);

        g_pMainSlider2D = new XnVSelectableSlider2D(3, 3);
        g_pMainSlider2D->RegisterValueChange(NULL, &MainSlider_OnValueChange);
        g_pMainSlider2D->SetValueChangeOnOffAxis(true);

        // Create the flow manager and connect to point tracker
        g_pMainFlowRouter = new XnVFlowRouter;
        pSessionGenerator->AddListener(g_pMainFlowRouter);

        // Main loop
        while (!xnOSWasKeyboardHit()) {
            context.WaitAnyUpdateAll();
            ((XnVSessionManager *)pSessionGenerator)->Update(&context);
        }
    } else {
        g_UserGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_UPPER);

        rc = context.StartGeneratingAll();
        CHECK_RC(rc, "StartGenerating");

        XnCallbackHandle hUserCBs, hCalibrationStartCB, hCalibrationCompleteCB, hPoseCBs;
        g_UserGenerator.RegisterUserCallbacks(NewUser, LostUser, NULL, hUserCBs);
        rc = g_UserGenerator.GetSkeletonCap().RegisterToCalibrationStart(CalibrationStarted, NULL, hCalibrationStartCB);
        CHECK_RC(rc, "Register to calibration start");
        rc = g_UserGenerator.GetSkeletonCap().RegisterToCalibrationComplete(CalibrationCompleted, NULL, hCalibrationCompleteCB);
        CHECK_RC(rc, "Register to calibration complete");

        while (TRUE) {
            glutDisplay();
        }
    }

    delete pSessionGenerator;
    delete g_pMainSlider2D;
    delete g_pMainFlowRouter;

    return 0;
}
