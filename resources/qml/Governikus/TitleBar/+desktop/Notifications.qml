/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.NotificationModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.View 1.0

Item {
	id: baseItem

	readonly property color iconColor: {
		if (d.unreadMsg) {
			if (NotificationModel.lastType === "developermode")
				return Constants.red;
			if (NotificationModel.lastType === "feedback")
				return Constants.green;
		}
		return Style.text.header_inverse.textColor;
	}

	signal newNotification

	function toggle() {
		if (!fadingAnimation.running)
			d.fadeIn = !d.fadeIn;
		if (d.fadeIn) {
			logEntryList.positionViewAtEnd();
		}
		d.unreadMsg = false;
		fadeOutTimer.stop();
	}

	visible: SettingsModel.showInAppNotifications

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
		anchors.bottom: parent.bottom
		anchors.bottomMargin: d.fadeIn ? radius - height : 0
		anchors.right: parent.right
		anchors.rightMargin: -radius
		border.color: Constants.blue
		border.width: Math.max(1, ApplicationModel.scaleFactor * 3)
		color: Style.color.background
		height: ApplicationModel.scaleFactor * 200
		radius: logEntryList.spacing
		width: ApplicationModel.scaleFactor * 800

		Behavior on anchors.bottomMargin  {
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
			anchors.bottomMargin: logList.border.width
			anchors.fill: parent
			anchors.rightMargin: spacing
			anchors.topMargin: logList.radius
			bottomMargin: spacing
			clip: true
			leftMargin: spacing
			model: NotificationModel
			scrollBarBottomPadding: spacing
			scrollBarTopPadding: spacing
			spacing: Constants.text_spacing
			topMargin: spacing

			delegate: Item {
				Accessible.name: notificationTime.text + " " + notificationBody.text
				activeFocusOnTab: d.fadeIn && !fadeOutTimer.running
				height: row.height
				width: row.width

				Row {
					id: row
					spacing: logEntryList.spacing

					Component.onCompleted: {
						d.unreadMsg = true;
						if (!d.fadeIn) {
							d.fadeIn = true;
							fadeOutTimer.restart();
						}
						if (fadeOutTimer.running) {
							// Calling logEntryList.positionViewAtEnd() only works unreliably.
							// Delay it so that the ListView has *really* completed adding the Row:
							positionViewAtEndTimer.restart();
						}
						baseItem.newNotification();
					}

					GText {
						id: notificationTime
						text: model.time
					}
					GText {
						id: notificationBody
						text: model.text
						textStyle: model.type === "developermode" ? Style.text.normal_warning : Style.text.normal
						width: logEntryList.width - notificationTime.width - 3 * logEntryList.spacing
					}
				}
				FocusFrame {
				}
			}

			Timer {
				id: positionViewAtEndTimer
				interval: 1

				onTriggered: logEntryList.positionViewAtEnd()
			}
		}
	}
}
