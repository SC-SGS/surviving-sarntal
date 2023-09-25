//
// Created by tanja on 25.09.23.
//
#include "kinect_handler.h"
#include "input.h"

// xml to initialize OpenNI
#define SAMPLE_XML_FILE "../../../Data/Sample-Tracking.xml"
#define SAMPLE_XML_FILE_LOCAL "Sample-Tracking.xml"

XnVSelectableSlider2D *g_pMainSlider2D;
XnVFlowRouter *g_pMainFlowRouter;
bool just_jumped = false;
//-----------------------------------------------------------------------------
// Callbacks
//-----------------------------------------------------------------------------

void XN_CALLBACK_TYPE MainSlider_OnValueChange(XnFloat xValue, XnFloat yValue,
                                               void *cxt) {
    printf("fValue %6.2f %6.2f\n", xValue * 2.0f - 1.0f, yValue * 2.0f - 1.0f);
    float scaled_x_value = xValue * 2.0f - 1.0f;
    float scaled_y_value = yValue * 2.0f - 1.0f;
    horizontal_axis = scaled_x_value;
    do_kinect_jump = false;
    if (scaled_y_value > 0.3){
        if (!just_jumped){
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
    printf("Session started.\n");
    g_pMainFlowRouter->SetActive(g_pMainSlider2D);
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
    XnBool bRemoting = FALSE;

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

    printf("Please perform focus gesture to start session\n");
    printf("Hit any key to exit\n");

    // Main loop
    while (!xnOSWasKeyboardHit()) {
        context.WaitAnyUpdateAll();
        ((XnVSessionManager *)pSessionGenerator)->Update(&context);
    }

    delete pSessionGenerator;
    delete g_pMainSlider2D;
    delete g_pMainFlowRouter;

    return 0;
}
