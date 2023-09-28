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
#define SAMPLE_XML_FILE_LOCAL "Sample-User.xml"

XnVSelectableSlider2D *g_p_main_slider2_d;
XnVFlowRouter *g_p_main_flow_router;
xn::Context context;
bool just_jumped = false;

xn::DepthGenerator g_depth_generator;
xn::UserGenerator g_user_generator;

XnUserID g_n_player = 0;
XnBool g_b_calibrated = FALSE;

xn::ScriptNode script_node;
XnVSessionGenerator *p_session_generator;

XnFloat initial_y = 0.0;
XnFloat initial_z = 0.0;
XnFloat initial_head_y = 0.0;

XnBool g_b_pause = false;

bool use_hand = false;

//-----------------------------------------------------------------------------
// Callbacks for kinect body
//-----------------------------------------------------------------------------

XnBool assignPlayer(XnUserID user) {
    if (g_n_player != 0)
        return FALSE;

    XnPoint3D com;
    g_user_generator.GetCoM(user, com);
    if (com.Z == 0)
        return FALSE;

    printf("Matching for existing calibration\n");
    g_user_generator.GetSkeletonCap().LoadCalibrationData(user, 0);
    g_user_generator.GetSkeletonCap().StartTracking(user);
    g_n_player = user;
    return TRUE;
}

void XN_CALLBACK_TYPE newUser(xn::UserGenerator &generator, XnUserID user,
                              void *pCookie) {
    if (!g_b_calibrated) // check on player0 is enough
    {
        generator.GetSkeletonCap().StartTracking(user);
        return;
    }

    assignPlayer(user);
}

void findPlayer() {
    if (g_n_player != 0) {
        return;
    }
    XnUserID a_users[20];
    XnUInt16 n_users = 20;
    g_user_generator.GetUsers(a_users, n_users);

    for (int i = 0; i < n_users; ++i) {
        if (assignPlayer(a_users[i]))
            return;
    }
}

void lostPlayer() {
    g_n_player = 0;
    findPlayer();
}

void XN_CALLBACK_TYPE lostUser(xn::UserGenerator &generator, XnUserID user,
                               void *pCookie) {
    printf("Lost user %d\n", user);
    if (g_n_player == user) {
        lostPlayer();
    }
}

void XN_CALLBACK_TYPE calibrationStarted(xn::SkeletonCapability &skeleton,
                                         XnUserID user, void *cxt) {}

void XN_CALLBACK_TYPE calibrationCompleted(xn::SkeletonCapability &skeleton,
                                           XnUserID user,
                                           XnCalibrationStatus eStatus,
                                           void *cxt) {
    printf("User found! You can start playing :) \n");
    kinect_init = true;
    if (eStatus == XN_CALIBRATION_STATUS_OK) {
        if (!g_b_calibrated) {
            g_user_generator.GetSkeletonCap().SaveCalibrationData(user, 0);
            g_n_player = user;
            g_user_generator.GetSkeletonCap().StartTracking(user);
            g_b_calibrated = TRUE;
        }

        XnUserID a_users[10];
        XnUInt16 n_users = 10;
        g_user_generator.GetUsers(a_users, n_users);
        for (int i = 0; i < n_users; ++i)
            g_user_generator.GetPoseDetectionCap().StopPoseDetection(a_users[i]);
    }
}

// this function is called each frame
void glutDisplay() {
    xn::SceneMetaData scene_md;
    xn::DepthMetaData depth_md;
    g_depth_generator.GetMetaData(depth_md);

    if (!g_b_pause) {
        // Read next available data
        context.WaitOneUpdateAll(g_depth_generator);
    }

    if (g_n_player != 0) {
        XnPoint3D com;
        g_user_generator.GetCoM(g_n_player, com);
        if (com.Z == 0) {
            g_n_player = 0;
            findPlayer();
        }

        XnSkeletonJointTransformation torso_data;
        XnSkeletonJointTransformation head_data;
        g_user_generator.GetSkeletonCap().GetSkeletonJoint(
            g_n_player, XN_SKEL_TORSO, torso_data);
        g_user_generator.GetSkeletonCap().GetSkeletonJoint(
            g_n_player, XN_SKEL_HEAD, head_data);
        if (initial_y == 0.0) {
            initial_y = torso_data.position.position.Y;
            initial_z = torso_data.position.position.Z;
            initial_head_y = head_data.position.position.Y;
        }

        XnFloat scaled_z = std::max(
            -1.0f,
            std::min(1.0f, -(torso_data.position.position.Z - initial_z) / 700));
        // printf("%6.1f    (%6.1f)\n", scaled_z,
        // jointData.position.position.Z);
        horizontal_axis = scaled_z;
        if (torso_data.position.position.Y > initial_y + 80 + 10 * scaled_z) {
            do_kinect_jump = true;
            // printf("JUMP %6.1f (init %6.1f)\n",
            // jointData.position.position.Y, initial_y);
        } else {
            do_kinect_jump = false;
        }
        if (head_data.position.position.Y < initial_head_y - 200 - 10 * scaled_z) {
            do_kinect_duck = true;
        } else {
            do_kinect_duck = false;
        }
    }
}

#define CHECK_RC(rc, what)                                                     \
    if (rc != XN_STATUS_OK) {                                                  \
        printf("%s failed: %s\n", what, xnGetStatusString(rc));                \
        return rc;                                                             \
    }

#define CHECK_ERRORS(rc, errors, what)                                         \
    if (rc == XN_STATUS_NO_NODE_PRESENT) {                                     \
        XnChar strError[1024];                                                 \
        errors.ToString(strError, 1024);                                       \
        printf("%s\n", strError);                                              \
        return (rc);                                                           \
    }

//-----------------------------------------------------------------------------
// Callbacks for kinect hand
//-----------------------------------------------------------------------------

void XN_CALLBACK_TYPE mainSliderOnValueChange(XnFloat xValue, XnFloat yValue,
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
    if (scaled_y_value < -0.5) {
        do_kinect_duck = true;
    } else {
        do_kinect_duck = false;
    }
}

// Callback for when the focus is in progress
void XN_CALLBACK_TYPE sessionProgress(const XnChar *strFocus,
                                      const XnPoint3D &ptFocusPoint,
                                      XnFloat fProgress, void *UserCxt) {}

// callback for session start
void XN_CALLBACK_TYPE sessionStart(const XnPoint3D &ptFocusPoint,
                                   void *UserCxt) {
    g_p_main_flow_router->SetActive(g_p_main_slider2_d);
    printf("Player was found! The game starts soon...");
    kinect_init = true;
}
// Callback for session end
void XN_CALLBACK_TYPE sessionEnd(void *UserCxt) {
    printf("Session ended. Please perform focus gesture to start session\n");
    g_p_main_flow_router->SetActive(nullptr);
}

XnBool fileExists(const char *fn) {
    XnBool exists;
    xnOSDoesFileExist(fn, &exists);
    return exists;
}

int initKinect() {

    // Create context
    const char *fn = nullptr;
    if (fileExists(SAMPLE_XML_FILE))
        fn = SAMPLE_XML_FILE;
    else if (fileExists(SAMPLE_XML_FILE_LOCAL))
        fn = SAMPLE_XML_FILE_LOCAL;
    else {
        printf("Could not find '%s' nor '%s'. Aborting.\n", SAMPLE_XML_FILE,
               SAMPLE_XML_FILE_LOCAL);
        return XN_STATUS_ERROR;
    }
    XnStatus rc = context.InitFromXmlFile(fn, script_node);
    if (rc != XN_STATUS_OK) {
        printf("Couldn't initialize: %s\n", xnGetStatusString(rc));
        return 1;
    }
    rc = context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_depth_generator);
    CHECK_RC(rc, "Find depth generator");
    rc = context.FindExistingNode(XN_NODE_TYPE_USER, g_user_generator);
    CHECK_RC(rc, "Find user generator");
    if (!g_user_generator.IsCapabilitySupported(XN_CAPABILITY_SKELETON) ||
        !g_user_generator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION)) {
        printf("User generator doesn't support either skeleton or pose "
               "detection.\n");
        return XN_STATUS_ERROR;
    }

    if (use_hand) {
        // Create the Session Manager
        p_session_generator = new XnVSessionManager();
        rc = ((XnVSessionManager *)p_session_generator)
                 ->Initialize(&context, "Click", "RaiseHand");
        if (rc != XN_STATUS_OK) {
            printf("Session Manager couldn't initialize: %s\n",
                   xnGetStatusString(rc));
            delete p_session_generator;
            return 1;
        }

        // Initialization done. Start generating
        context.StartGeneratingAll();

        // Register session callbacks
        p_session_generator->RegisterSession(nullptr, &sessionStart, &sessionEnd,
                                           &sessionProgress);

        g_p_main_slider2_d = new XnVSelectableSlider2D(3, 3);
        g_p_main_slider2_d->RegisterValueChange(nullptr, &mainSliderOnValueChange);
        g_p_main_slider2_d->SetValueChangeOnOffAxis(true);

        // Create the flow manager and connect to point tracker
        g_p_main_flow_router = new XnVFlowRouter;
        p_session_generator->AddListener(g_p_main_flow_router);

        // Main loop
        while (!xnOSWasKeyboardHit()) {
            context.WaitAnyUpdateAll();
            ((XnVSessionManager *)p_session_generator)->Update(&context);
        }
    } else {
        g_user_generator.GetSkeletonCap().SetSkeletonProfile(
            XN_SKEL_PROFILE_UPPER);

        rc = context.StartGeneratingAll();
        CHECK_RC(rc, "StartGenerating");

        XnCallbackHandle h_user_c_bs, h_calibration_start_cb, h_calibration_complete_cb,
            h_pose_c_bs;
        g_user_generator.RegisterUserCallbacks(newUser, lostUser, nullptr,
                                              h_user_c_bs);
        rc = g_user_generator.GetSkeletonCap().RegisterToCalibrationStart(
            calibrationStarted, nullptr, h_calibration_start_cb);
        CHECK_RC(rc, "Register to calibration start");
        rc = g_user_generator.GetSkeletonCap().RegisterToCalibrationComplete(
            calibrationCompleted, nullptr, h_calibration_complete_cb);
        CHECK_RC(rc, "Register to calibration complete");

        while (TRUE) {
            glutDisplay();
        }
    }

    delete p_session_generator;
    delete g_p_main_slider2_d;
    delete g_p_main_flow_router;

    return 0;
}
