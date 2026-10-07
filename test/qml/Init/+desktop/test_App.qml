/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

import QtQuick
import QtTest

import Governikus.Type

TestCase {
	id: testCase

	function cleanup() {
		spy.clear();
	}
	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Init; App {}", testCase);
	}
	function test_activeModule(data) {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
		ApplicationModel.currentWorkflow = data.workflow;
		if (data.effectiveModule === undefined) {
			// CHECK_ID_CARD and REMOTE_SERVICE have no suitable match on desktop and get ignored by ContentArea
			if ((data.requestedModule === UiModule.CHECK_ID_CARD || data.requestedModule === UiModule.REMOTE_SERVICE) && data.workflow === ApplicationModel.Workflow.NONE) {
				ignoreWarning("No suitable Component for UiModule request " + data.requestedModule);
			} else {
				ignoreWarning("Suppressing activation of UiModule " + data.requestedModule + " since a workflow is active");
			}
		}
		UiPluginModel.emitFireShowRequest(data.requestedModule);
		verify(spy.count === 1);
		compare(testObject.activeModule, data.effectiveModule || UiModule.DEFAULT);
	}
	function test_activeModule_data() {
		const workflows = [ApplicationModel.Workflow.CHANGE_PIN, ApplicationModel.Workflow.SELF_AUTHENTICATION, ApplicationModel.Workflow.AUTHENTICATION, ApplicationModel.Workflow.REMOTE_SERVICE, ApplicationModel.Workflow.NONE];
		const uiModules = [UiModule.CURRENT, UiModule.DEFAULT, UiModule.IDENTIFY, UiModule.SETTINGS, UiModule.PINMANAGEMENT, UiModule.HELP, UiModule.SELF_AUTHENTICATION, UiModule.ONBOARDING, UiModule.UPDATEINFORMATION, UiModule.REMOTE_SERVICE, UiModule.CHECK_ID_CARD];

		let testList = [];
		for (const workflow of workflows) {
			for (const module of uiModules) {
				let effectiveModule = undefined;

				switch (workflow) {
				case ApplicationModel.Workflow.NONE:
					{
						switch (module) {
						case UiModule.IDENTIFY:
							effectiveModule = UiModule.SELF_AUTHENTICATION;
							break;
						case UiModule.CHECK_ID_CARD:
						case UiModule.REMOTE_SERVICE:
							break;
						default:
							effectiveModule = module;
							break;
						}
						break;
					}
				case ApplicationModel.Workflow.AUTHENTICATION:
				case ApplicationModel.Workflow.SELF_AUTHENTICATION:
					{
						switch (module) {
						case UiModule.IDENTIFY:
							effectiveModule = UiModule.IDENTIFY;
							break;
						default:
							break;
						}
						break;
					}
				default:
					break;
				}

				testList.push({
					tag: workflow + " + " + module,
					workflow: workflow,
					requestedModule: module,
					effectiveModule: effectiveModule
				});
			}
		}

		return testList;
	}

	name: "test_App"
	visible: true
	when: windowShown

	SignalSpy {
		id: spy

		signalName: "fireShowRequest"
		target: UiPluginModel
	}
}
