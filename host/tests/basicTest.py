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

from helpers.testHelper import testHelperClass

class basicTest(testHelperClass):
           
    def __init__(self):
        super().__init__('basic_test', '1')

    def testFunction(self):
        """This test will provide basic functionality and call the framework

        Returns:
            bool: true
        """
        print("unit_test2 called.")
        return True

if __name__ == '__main__':
    test = basicTest()
    test.run()

# Transmit receive
