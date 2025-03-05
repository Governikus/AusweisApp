/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */
import QtTest
import QtQuick

import Governikus.Global

TestCase {
	id: testCase

	function test_enabled() {
		base.enabled = false;
		compare(base.enabled, false);
		compare(sub.enabled, false);
		base.enabled = true;
		compare(base.enabled, true);
		compare(sub.enabled, true);
	}
	function test_stageprogress() {
		compare(base.absoluteProgress, 0.5);
		compare(sub.absoluteProgress, 0.6);
		compare(testController.progress.absoluteProgress, 0.3);
		compare(testController.actualStageProgress1, 0.6);
		compare(testController.actualStageProgress2, 0.9);
	}
	function test_stages(data) {
		base.currentStage = data.currentStage;
		base.stages = data.stages;
		compare(sub.currentStage, data.currentStage);
		compare(sub.stages, data.stages);
	}
	function test_stages_data() {
		return [
			{
				"currentStage": 1,
				"stages": 2
			},
			{
				"currentStage": 2,
				"stages": 3
			},
		];
	}

	name: "test_ProgressTracker"
	visible: true
	when: windowShown

	ProgressTracker {
		id: base

		currentStage: 2
		currentStep: 5
		enabled: true
		stages: 3
		steps: 10
	}
	ProgressTracker {
		id: sub

		baseProgressTracker: base
		relativeProgress: 0.6
	}
	SomeController {
		id: testController

		progress: ProgressTracker {
			from: 0.3
			to: 0.9
		}
	}

	component SomeController: Item {
		id: someController

		readonly property real actualStageProgress1: controllerSubProgress1.absoluteProgress
		readonly property real actualStageProgress2: controllerSubProgress2.absoluteProgress
		property ProgressTracker progress: null

		ProgressTracker {
			id: controllerSubProgress1

			baseProgressTracker: someController.progress
			relativeProgress: 0.5
		}
		ProgressTracker {
			id: controllerSubProgress2

			baseProgressTracker: someController.progress
			relativeProgress: 1
		}
	}
}
