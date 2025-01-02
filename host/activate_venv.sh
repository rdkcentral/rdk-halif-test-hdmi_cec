#!/usr/bin/env bash
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
MY_PATH="$(realpath ${BASH_SOURCE[0]})"
MY_DIR="$(dirname ${MY_PATH})"
VENV_DIR="${MY_DIR}/python_venv"  # Default virtual environment directory name

# Check if the script is sourced
if [[ "${BASH_SOURCE[0]}" != "${0}" ]]; then
    source "$VENV_DIR"/bin/activate
    echo "Virtual environment '$VENV_DIR' activated."
else
    echo "The script must be sourced. 'source ./activate_venv.sh' or '. ./activate_venv.sh'"
    echo "Once activated you can deactivate with 'deactivate' command"
fi
