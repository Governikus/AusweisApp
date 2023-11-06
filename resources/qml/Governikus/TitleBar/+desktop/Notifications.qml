/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.Global
import Governikus.Style
import Governikus.Type.NotificationModel
import Governikus.Type.SettingsModel
import Governikus.View

Item {
	id: baseItem

	readonly property color iconColor: {
		if (d.unreadMsg) {
			if (NotificationModel.lastType === "developermode")
				return Constants.red;
			return Style.color.text_subline;
		}
		return Style.color.text;
	}
	readonly property bool unreadMessages: d.unreadMsg

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

		anchors.left: parent.left
		anchors.leftMargin: d.fadeIn ? -(width + Constants.pane_spacing) : 0
		anchors.top: parent.top
		border.color: Style.color.control_border
		border.width: Style.dimens.border_width
		color: Style.color.control
		height: plugin.scaleFactor * 200
		radius: Style.dimens.control_radius
		width: plugin.scaleFactor * 800

		Behavior on anchors.leftMargin {
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
			anchors.leftMargin: Constants.pane_padding
			bottomMargin: Constants.pane_padding
			clip: true
			model: NotificationModel
			scrollBarBottomPadding: spacing
			scrollBarColor: Style.color.control_content
			scrollBarTopPadding: spacing
			spacing: Constants.text_spacing
			topMargin: Constants.pane_padding

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
						textStyle: Style.text.button
					}
					GText {
						id: notificationBody

						text: model.text
						textStyle: model.type === "developermode" ? Style.text.normal_warning : Style.text.button
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
