/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.Global
import Governikus.Style
import Governikus.Type
import Governikus.View

Item {
	id: root

	readonly property color iconColor: {
		if (d.unreadMsg) {
			if (NotificationModel.lastType === "developermode")
				return Style.color.warning;
			return Style.color.textSubline.basic;
		}
		return Style.color.textNormal.basic;
	}
	readonly property bool unreadMessages: d.unreadMsg
	readonly property bool visibleToUser: d.fadeIn && !fadeOutTimer.running

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
		anchors.leftMargin: d.fadeIn ? -(width + Style.dimens.pane_spacing) : 0
		anchors.top: parent.top
		border.color: Style.color.control.border.basic
		border.width: Style.dimens.border_width
		color: Style.color.control.background.basic
		height: UiPluginModel.scaleFactor * 200
		radius: Style.dimens.control_radius
		width: UiPluginModel.scaleFactor * 800

		Behavior on anchors.leftMargin {
			PropertyAnimation {
				id: fadingAnimation

				duration: Style.animation_duration
				easing.type: Easing.InOutQuad
			}
		}

		MouseArea {
			anchors.fill: parent
		}
		GListView {
			id: logEntryList

			activeFocusOnTab: root.visibleToUser
			anchors.fill: parent
			bottomMargin: Style.dimens.pane_padding
			clip: true
			model: NotificationModel
			scrollBarBottomPadding: spacing
			scrollBarColor: Style.color.control.content.basic
			scrollBarTopPadding: spacing
			spacing: Style.dimens.text_spacing
			topMargin: Style.dimens.pane_padding

			delegate: Item {
				id: logEntry

				required property string text
				required property string time
				required property string type

				Accessible.name: notificationTime.text + " " + notificationBody.text
				Accessible.role: Accessible.StaticText
				implicitHeight: row.height
				implicitWidth: row.width + 2 * Style.dimens.pane_padding

				Row {
					id: row

					anchors.left: parent.left
					anchors.leftMargin: Style.dimens.pane_padding
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
						root.newNotification();
					}

					GText {
						id: notificationTime

						activeFocusOnTab: false
						color: Style.color.control.content.basic
						text: logEntry.time
					}
					GText {
						id: notificationBody

						activeFocusOnTab: false
						color: Style.color.control.content.basic
						style: logEntry.type === "developermode" ? Text.Outline : Text.Normal
						styleColor: Style.color.warning
						text: logEntry.text
						width: logEntryList.width - 2 * Style.dimens.pane_padding - notificationTime.width - 3 * logEntryList.spacing
					}
				}
				FocusFrame {
					borderColor: Style.color.control.content.basic
					framee: row
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
