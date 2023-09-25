//
// Created by tanja on 25.09.23.
//

#ifndef SURVIVING_SARNTAL_KINECT_HANDLER_H
#define SURVIVING_SARNTAL_KINECT_HANDLER_H

// General headers
#include <stdio.h>
// OpenNI headers
#ifdef kinect
#undef PI
#include <XnOpenNI.h>
// NITE headers
#include "XnVMultiProcessFlowClient.h"
#include <XnVFlowRouter.h>
#include <XnVSelectableSlider2D.h>
#include <XnVSessionManager.h>
#define PI 3.14159265358979323846f

#endif

int initKinect();

extern double horizontal_axis;
extern bool do_kinect_jump;

#endif // SURVIVING_SARNTAL_KINECT_HANDLER_H
