/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):	Balazs Vagvolgyi, Simon DiMaio, Anton Deguet
  Created on:	2008-05-23

  (C) Copyright 2008-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskManager.h>

//#include <cisstDaVinciAPI/cdvReadOnly.h>
#include <cisstDaVinci/cdvReadWrite.h>

#include <cisstCommon.h>
#include <cisstStereoVision.h>

#include "BehaviorLUS.h"

#include <MeasurementBehavior.h>
#include "MapBehavior.h"
#include <ImageViewer.h>

#define HAS_ULTRASOUDS 0
int main()
{
    // log configuration
    cmnLogger::SetLoD(CMN_LOG_LOD_VERY_VERBOSE);
	cmnLogger::GetMultiplexer()->AddChannel(std::cout, CMN_LOG_LOD_VERY_VERBOSE);
    // add a log per thread
    osaThreadedLogFile threadedLog("example1-");
    cmnLogger::GetMultiplexer()->AddChannel(threadedLog, CMN_LOG_LOD_VERY_VERBOSE);
    // specify a higher, more verbose log level for these classes
    cmnClassRegister::SetLoD("ui3BehaviorBase", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("ui3Manager", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("mtsTaskInterface", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("mtsComponentManager", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("BehaviorLUS", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("dvapi_stream", CMN_LOG_LOD_INIT_VERBOSE);

    mtsComponentManager * componentManager = mtsComponentManager::GetInstance();
#if 0
    cdvReadOnly * daVinci = new cdvReadOnly("daVinci", 0.0 /* period to be removed */,
                                                   "10.0.0.5", 5002, 0x1111, 50);
#else
    cdvReadWrite * daVinci = new cdvReadWrite("daVinci", 60 /* Hz */);
#endif
    componentManager->AddComponent(daVinci);

    ui3Manager guiManager;

#if HAS_ULTRASOUDS
    BehaviorLUS behavior3("BehaviorLUS");
    guiManager.AddBehavior(&behavior3,       // behavior reference
                           1,             // position in the menu bar: default
                           "LUS.png");            // icon file: no texture
#else
    MeasurementBehavior measurementBehavior("Measure");
    guiManager.AddBehavior(&measurementBehavior,
                           1,
                           "measure.png");

    MapBehavior mapBehavior("Map");
    guiManager.AddBehavior(&mapBehavior,
                           2,
                           "map.png");

    ImageViewer imageViewer("image");
    guiManager.AddBehavior(&imageViewer,
                           3,
                           "move.png");

    
#endif

#if HAS_ULTRASOUDS
    svlStreamManager vidUltrasoundStream(1);  // running on multiple threads

    svlFilterSourceVideoCapture vidUltrasoundSource(false); // mono source
    if (vidUltrasoundSource.LoadSettings("usvideo.dat") != SVL_OK) {
        cout << "Setup Ultrasound video input:" << endl;
        vidUltrasoundSource.DialogSetup();
        vidUltrasoundSource.SaveSettings("usvideo.dat");
    }

    vidUltrasoundStream.Trunk().Append(&vidUltrasoundSource);  //this crashes
    
    // add image cropper
    svlFilterImageCropper vidUltrasoundCropper;
    vidUltrasoundCropper.SetRectangle(186, 27, 186 + 360 - 1, 27 + 332 - 1);
    vidUltrasoundStream.Trunk().Append(&vidUltrasoundCropper);

    // add guiManager as a filter to the pipeline, so it will receive video frames
    // "StereoVideo" is defined in the UI Manager as a possible video interface    
    vidUltrasoundStream.Trunk().Append(behavior3.GetStreamSamplerFilter("USVideo"));


    // add debug window
    svlFilterImageWindow vidUltrasoundWindow;
    vidUltrasoundStream.Trunk().Append(&vidUltrasoundWindow);

    // save one frame
    svlFilterImageFileWriter vidUltrasoundWriter;
    vidUltrasoundWriter.SetFilePath("usimage", "bmp");
    vidUltrasoundWriter.Record(1);
    vidUltrasoundStream.Trunk().Append(&vidUltrasoundWriter);

    vidUltrasoundStream.Initialize();
#endif

    ////////////////////////////////////////////////////////////////
    // setup renderers

    svlCameraGeometry camera_geometry;
    // Load Camera calibration results
    camera_geometry.LoadCalibration("E:/Users/davinci_mock_or/calib_results.txt");
    // Center world in between the two cameras (da Vinci specific)
    camera_geometry.SetWorldToCenter();
    // Rotate world by 180 degrees (VTK specific)
    camera_geometry.RotateWorldAboutY(180.0);
    // Display camera configuration
    std::cerr << camera_geometry;

    // *** Left view ***
    guiManager.AddRenderer(svlRenderTargets::Get(1)->GetWidth(),  // render width
                           svlRenderTargets::Get(1)->GetHeight(), // render height
                           1.0,                                   // virtual camera zoom
                           false,                                 // borderless?
                           0, 0,                                  // window position
                           camera_geometry, SVL_LEFT,             // camera parameters
                           "LeftEyeView");                        // name of renderer

    // *** Right view ***

    guiManager.AddRenderer(svlRenderTargets::Get(0)->GetWidth(),  // render width
                           svlRenderTargets::Get(0)->GetHeight(), // render height
                           1.0,                                   // virtual camera zoom
                           false,                                 // borderless?
                           0, 0,                                  // window position
                           camera_geometry, SVL_RIGHT,            // camera parameters
                           "RightEyeView");                       // name of renderer

    // Sending renderer output to external render targets
    guiManager.SetRenderTargetToRenderer("LeftEyeView",  svlRenderTargets::Get(1));
    guiManager.SetRenderTargetToRenderer("RightEyeView", svlRenderTargets::Get(0));

#if 0
    // Add third camera: simple perspective camera placed in the world center
    camera_geometry.SetPerspective(400.0, 2);

    guiManager.AddRenderer(384,                // render width
                           216,                // render height
                           1.0,                // virtual camera zoom
                           false,              // borderless?
                           0, 0,               // window position
                           camera_geometry, 2, // camera parameters
                           "ThirdEyeView");    // name of renderer
#endif

    ///////////////////////////////////////////////////////////////
    // start streaming
#if HAS_ULTRASOUDS
    vidUltrasoundStream.Start();
#endif

    vctFrm3 transform;
    transform.Translation().Assign(0.0, 0.0, 0.0);
    transform.Rotation().From(vctAxAnRot3(vctDouble3(0.0, 1.0, 0.0), cmnPI));

    // setup first arm
    ui3MasterArm * rightMaster = new ui3MasterArm("MTMR");
    guiManager.AddMasterArm(rightMaster);
    rightMaster->SetInput(daVinci, "MTMR",
                          daVinci, "MTMRSelect",
                          daVinci, "MTMRClutch",
                          ui3MasterArm::PRIMARY);
    rightMaster->SetTransformation(transform, 0.8 /* scale factor */);
    ui3CursorBase * rightCursor = new ui3CursorSphere();
    rightCursor->SetAnchor(ui3CursorBase::CENTER_RIGHT);
    rightMaster->SetCursor(rightCursor);

    // setup second arm
    ui3MasterArm * leftMaster = new ui3MasterArm("MTML");
    guiManager.AddMasterArm(leftMaster);
    leftMaster->SetInput(daVinci, "MTML",
                         daVinci, "MTMLSelect",
                         daVinci, "MTMLClutch",
                         ui3MasterArm::SECONDARY);
    leftMaster->SetTransformation(transform, 0.8 /* scale factor */);
    ui3CursorBase * leftCursor = new ui3CursorSphere();
    leftCursor->SetAnchor(ui3CursorBase::CENTER_LEFT);
    leftMaster->SetCursor(leftCursor);

    // first slave arm, i.e. PSM1
    ui3SlaveArm * slave1 = new ui3SlaveArm("Slave1");
    guiManager.AddSlaveArm(slave1);
    slave1->SetInput(daVinci, "PSM1");
    slave1->SetTransformation(transform, 1.0 /* scale factor */);
    
    //set up ECM as slave arm
    ui3SlaveArm * ecm1 = new ui3SlaveArm("ECM1");
    guiManager.AddSlaveArm(ecm1);
    ecm1 -> SetInput(daVinci, "ECM1");
    ecm1 -> SetTransformation(transform, 1.0);

    // setup event for MaM transitions
    guiManager.SetupMaM(daVinci, "MastersAsMice");


    guiManager.ConnectAll();

    // following should be replaced by a utility function or method of ui3Manager 
    componentManager->CreateAll();
    osaSleep(10.0 * cmn_s);
    
    componentManager->StartAll();
    osaSleep(1.0 * cmn_s);

    int ch;
    
    cerr << endl << "Keyboard commands:" << endl << endl;
    cerr << "  In command window:" << endl;
    cerr << "    'q'   - Quit" << endl << endl;
    do {
        ch = cmnGetChar();
        osaSleep(100.0 * cmn_ms);
    } while (ch != 'q');

    componentManager->KillAll();
    componentManager->Cleanup();

#if HAS_ULTRASOUDS
    vidUltrasoundStream.RemoveAll();
#endif

    return 0;
}

