#!/usr/bin/env python3
#** *****************************************************************************
# *
# * If not stated otherwise in this file or this component's LICENSE file the
# * following copyright and licenses apply:
# *
# * Copyright 2023 RDK Management
# *
# * Licensed under the Apache License, Version 2.0 (the "License");
# * you may not use this file except in compliance with the License.
# * You may obtain a copy of the License at
# *
# *
# http://www.apache.org/licenses/LICENSE-2.0
# *
# * Unless required by applicable law or agreed to in writing, software
# * distributed under the License is distributed on an "AS IS" BASIS,
# * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# * See the License for the specific language governing permissions and
# * limitations under the License.
# *
#* ******************************************************************************
import os
import sys

dir_path = os.path.dirname(os.path.realpath(__file__))
sys.path.append(dir_path+"/../")

from raft.framework.plugins.ut_raft.utHelper import utHelperClass

class testHelperClass(utHelperClass):

    """
    Test Helper Code

    This module provides common extensions for teams unit testing.
    Rather than being generic common modules, maybe more team centric on the way that the team will require to create their testing suites
    """
    def __init__(self, testName, qcId, log=None ):
        super().__init__(testName, qcId, log=log )

    # Engineering team adds any functions they require for the testing classes suite for this specific module.