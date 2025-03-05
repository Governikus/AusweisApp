/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

QtObject {
	readonly property real absoluteFrom: baseFrom + (baseTo - baseFrom) * from
	readonly property real absoluteProgress: absoluteFrom + (absoluteTo - absoluteFrom) * relativeProgress
	readonly property real absoluteTo: baseFrom + (baseTo - baseFrom) * to
	readonly property real baseFrom: baseProgressTracker !== null ? baseProgressTracker.from : 0
	property ProgressTracker baseProgressTracker: null
	readonly property real baseTo: baseProgressTracker !== null ? baseProgressTracker.to : 1
	property int currentStage: baseProgressTracker !== null ? baseProgressTracker.currentStage : 1
	property int currentStep: 0
	property bool enabled: baseProgressTracker !== null ? baseProgressTracker.enabled : false
	property real from: 0
	property real relativeProgress: currentStep / steps
	property int stages: baseProgressTracker !== null ? baseProgressTracker.stages : 1
	property int steps: 1
	property real to: 1
}
