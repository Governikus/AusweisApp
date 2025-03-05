/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import Governikus.Style
import Governikus.View
import Governikus.Type

Rectangle {
	id: root

	property ProgressTracker progress: null

	Accessible.focusable: true
	Accessible.ignored: !visible
	//: LABEL ALL_PLATFORMS
	Accessible.name: qsTr("Step %1 of %2. This step is %3 percent complete.").arg(d.currentStage).arg(d.stages).arg(Math.floor(d.absoluteProgress * 100))
	Accessible.role: Accessible.ProgressBar
	color: Style.color.pane.background.basic
	implicitHeight: bars.implicitHeight + UiPluginModel.safeAreaMargins.bottom
	layer.enabled: GraphicsInfo.api !== GraphicsInfo.Software

	layer.effect: GDropShadow {
		shadowOpacity: UiPluginModel.qtVersion === "6.8.0" ? 0.5 : 0.25
		shadowVerticalOffset: -5
	}

	QtObject {
		id: d

		readonly property real absoluteProgress: root.progress !== null ? root.progress.absoluteProgress : 0
		readonly property ControlComponents colors: Style.color.controlStagedProgressBar
		readonly property int currentStage: root.progress !== null ? root.progress.currentStage : 0
		readonly property int currentStep: root.progress !== null ? root.progress.currentStep : 0
		readonly property int stages: root.progress !== null ? root.progress.stages : 0
		readonly property int steps: root.progress !== null ? root.progress.steps : 0
	}
	RowLayout {
		id: bars

		spacing: Style.dimens.stagedprogressbar_spacing

		anchors {
			left: parent.left
			right: parent.right
			top: parent.top
		}
		Repeater {
			model: d.stages

			delegate: Rectangle {
				id: bar

				required property int index

				Layout.fillWidth: true
				border.color: d.colors.border.basic
				border.width: Style.dimens.border_width
				color: d.colors.background.basic
				implicitHeight: Style.dimens.stagedprogressbar_height

				Rectangle {
					color: d.colors.content.basic
					width: {
						let thisStage = bar.index + 1;
						if (thisStage > d.currentStage) {
							return 0;
						}
						if (thisStage < d.currentStage) {
							return bar.width;
						}
						return bar.width * d.absoluteProgress;
					}

					Behavior on width {
						NumberAnimation {
							duration: Style.animation_duration
						}
					}

					anchors {
						bottom: parent.bottom
						left: parent.left
						top: parent.top
					}
				}
			}
		}
	}
	FocusFrame {
		borderColor: Style.color.warning
		marginFactor: 0
	}
}
