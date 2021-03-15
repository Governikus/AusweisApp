/*
 * \copyright Copyright (c) 2019-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.NotificationModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.View 1.0


Item {
	id: baseItem

	visible: SettingsModel.showInAppNotifications

	readonly property color iconColor: {
		if (d.unreadMsg) {
			if (NotificationModel.lastType === "developermode") return Constants.red
			if (NotificationModel.lastType === "feedback") return Constants.green
		}
		return Style.text.header_inverse.textColor
	}
	signal newNotification()
	function toggle() {
		if (!fadingAnimation.running) d.fadeIn = !d.fadeIn
		if (d.fadeIn) {
			logEntryList.positionViewAtEnd()
		}
		d.unreadMsg = false
		fadeOutTimer.stop()
	}

	QtObject {
		id: d

		property bool fadeIn: false
		property bool unreadMsg: false
	}

	Timer {
		id: fadeOutTimer
		interval: 1800 // The notification button blinks 3 times for 600ms
		onTriggered: d.fadeIn = false
	}

	Rectangle {
		id: logList

		height: ApplicationModel.scaleFactor * 200
		width: ApplicationModel.scaleFactor * 800

		anchors.right: parent.right
		anchors.rightMargin: -radius
		anchors.bottom: parent.bottom
		anchors.bottomMargin: d.fadeIn ? radius - height : 0

		radius: logEntryList.spacing
		border.color: Constants.blue
		border.width: Math.max(1, ApplicationModel.scaleFactor * 3)
		color: Style.color.background

		Behavior on anchors.bottomMargin {
			PropertyAnimation {
				id: fadingAnimation
				duration: Constants.animation_duration
				easing.type: Easing.InOutQuad
			}
		}

		MouseArea {
			anchors.fill: parent
		}

		GListView {
			id: logEntryList

			anchors.fill: parent
			anchors.topMargin: logList.radius
			anchors.rightMargin: spacing
			anchors.bottomMargin: logList.border.width

			clip: true
			spacing: Constants.text_spacing
			topMargin: spacing
			bottomMargin: spacing
			leftMargin: spacing
			scrollBarTopPadding: spacing
			scrollBarBottomPadding: spacing
			model: NotificationModel
			delegate: Item {
				width: row.width
				height: row.height

				activeFocusOnTab: d.fadeIn && !fadeOutTimer.running
				Accessible.name: notificationTime.text + " " + notificationBody.text

				Row {
					id: row

					spacing: logEntryList.spacing

					Component.onCompleted: {
						d.unreadMsg = true
						if (!d.fadeIn) {
							d.fadeIn = true
							fadeOutTimer.restart()
						}
						if (fadeOutTimer.running) {
							// Calling logEntryList.positionViewAtEnd() only works unreliably.
							// Delay it so that the ListView has *really* completed adding the Row:
							positionViewAtEndTimer.restart()
						}
						baseItem.newNotification()
					}

					GText {
						id: notificationTime

						text: model.time
					}

					GText {
						id: notificationBody

						width: logEntryList.width - notificationTime.width - 3 * logEntryList.spacing

						text: model.text
						textStyle: model.type === "developermode" ? Style.text.normal_warning : Style.text.normal_inverse

					}
				}
				FocusFrame {}
			}

			Timer {
				id: positionViewAtEndTimer
				interval: 1
				onTriggered: logEntryList.positionViewAtEnd()
			}
		}
	}
}
