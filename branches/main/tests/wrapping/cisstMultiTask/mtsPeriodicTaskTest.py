# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

#
# $Id: $
#

# Author: Anton Deguet
# Date: 2010-01-20
#
# (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
# Reserved.

# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

import unittest
import numpy
import time

import cisstCommonPython as cisstCommon
import cisstVectorPython as cisstVector
import cisstMultiTaskPython as cisstMultiTask
import cisstMultiTaskPythonTestPython as cisstMultiTaskPythonTest # contains test classes

class PeriodicTaskTest(unittest.TestCase):
    def setUp(self):
        """Call before every test case."""

    def tearDown(self):
        """Call after every test case."""
        
    def TestType(self):
        """Test constructor and types of mtsPeriodicTask"""
        periodicTask = cisstMultiTaskPythonTest.mtsPeriodicTaskTest(0.05)
        # check type
        self.failUnless(isinstance(periodicTask, cisstMultiTaskPythonTest.mtsPeriodicTaskTest))
        self.failUnless(isinstance(periodicTask, cisstMultiTask.mtsTaskPeriodic))
        self.failUnless(isinstance(periodicTask, cisstMultiTask.mtsTaskContinuous))
        self.failUnless(isinstance(periodicTask, cisstMultiTask.mtsDevice))
        self.failUnless(isinstance(periodicTask, cisstCommon.cmnGenericObject))

    def TestUpdateFromC(self):
        """Test UpdateFromC for mtsPeriodicTask"""
        periodicTask = cisstMultiTaskPythonTest.mtsPeriodicTaskTest(0.05)
        periodicTask.UpdateFromC()
        # verify that both interfaces have been created
        self.failUnless(periodicTask.__dict__.has_key("MainInterface"))
        self.failUnless(periodicTask.__dict__.has_key("EmptyInterface")) # space should have been removed
        # test that MainInterface has been populated properly
        # command AddDouble(mtsDouble)
        self.failUnless(periodicTask.MainInterface.__dict__.has_key("AddDouble"))
        self.failUnless(isinstance(periodicTask.MainInterface.AddDouble, cisstMultiTask.mtsCommandWriteBase))
        proto = periodicTask.MainInterface.AddDouble.GetArgumentPrototype()
        self.failUnless(isinstance(proto, cisstMultiTask.mtsDouble))
        # command ZeroAll(void)
        self.failUnless(periodicTask.MainInterface.__dict__.has_key("ZeroAll"))
        self.failUnless(isinstance(periodicTask.MainInterface.ZeroAll, cisstMultiTask.mtsCommandVoidBase))
        # command GetDouble(mtsDouble)
        self.failUnless(periodicTask.MainInterface.__dict__.has_key("GetDouble"))
        self.failUnless(isinstance(periodicTask.MainInterface.GetDouble, cisstMultiTask.mtsCommandReadBase))
        proto = periodicTask.MainInterface.GetDouble.GetArgumentPrototype()
        self.failUnless(isinstance(proto, cisstMultiTask.mtsDouble))
        # command GetVector(mtsDoubleVec)
        self.failUnless(periodicTask.MainInterface.__dict__.has_key("GetVector"))
        self.failUnless(isinstance(periodicTask.MainInterface.GetVector, cisstMultiTask.mtsCommandReadBase))
        proto = periodicTask.MainInterface.GetVector.GetArgumentPrototype()
        self.failUnless(isinstance(proto, cisstMultiTask.mtsDoubleVec))
        
    def TestRun(self):
        """Test run mtsPeriodicTask"""
        # create the task
        period = 0.05
        periodicTask = cisstMultiTaskPythonTest.mtsPeriodicTaskTest(period)
        periodicTask.UpdateFromC()
        # get the task manager
        taskManager = cisstMultiTask.mtsManagerLocal_GetInstance()
        self.failUnless(isinstance(taskManager, cisstMultiTask.mtsManagerLocal))
        # add the task
        self.failUnless(taskManager.AddTask(periodicTask))
        # create and start the task(s)
        taskManager.CreateAll()
        taskManager.StartAll()
        # and now use the task ...
        # -1- set all to zero and check result
        periodicTask.MainInterface.ZeroAll()
        time.sleep(2 * period)
        result = periodicTask.MainInterface.GetDouble()
        timestamp1 = result.GetTimestamp()
        self.failUnless(result.Data == 0.0)
        result = periodicTask.MainInterface.GetVector()
        data = result.Data()
        size = data.size
        expected = numpy.zeros(size)
        self.failUnless((data == expected).all())
        # -2- use the AddDouble to modify all and test 
        periodicTask.MainInterface.AddDouble(1.0)
        time.sleep(2 * period)
        result = periodicTask.MainInterface.GetDouble()
        timestamp2 = result.GetTimestamp()
        self.failUnless(result.Data == 1.0)
        result = periodicTask.MainInterface.GetVector()
        data = result.Data()
        size = data.size
        expected = numpy.ones(size)
        self.failUnless((data == expected).all())
        # -3- set to zero again and test
        periodicTask.MainInterface.ZeroAll()
        time.sleep(2 * period)
        result = periodicTask.MainInterface.GetDouble()
        timestamp3 = result.GetTimestamp()
        self.failUnless(result.Data == 0.0)
        result = periodicTask.MainInterface.GetVector()
        data = result.Data()
        size = data.size
        expected = numpy.zeros(size)
        self.failUnless((data == expected).all())
        # ... and now check the timestamps
        self.failUnless(timestamp1 < timestamp2)
        self.failUnless(timestamp2 < timestamp3)
        # kill all
        taskManager.KillAll()
