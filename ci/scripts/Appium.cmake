find_package(Python REQUIRED)
step(uv pip install -r source/test/appium/requirements.txt)
step(${Python_EXECUTABLE} -m unittest desktop.main.TestAusweisApp CHDIR ${CMAKE_SOURCE_DIR}/test/appium)
