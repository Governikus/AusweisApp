/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.Global
import Governikus.Style
import Governikus.Type
import Governikus.View

Item {
	id: baseItem

	readonly property color iconColor: {
		if (d.unreadMsg) {
			if (NotificationModel.lastType === "developermode")
				return Style.color.warning;
			return Style.color.textSubline.basic;
		}
		return Style.color.textNormal.basic;
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
		border.color: Style.color.control.border.basic
		border.width: Style.dimens.border_width
		color: Style.color.control.background.basic
		height: UiPluginModel.scaleFactor * 200
		radius: Style.dimens.control_radius
		width: UiPluginModel.scaleFactor * 800

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
			scrollBarColor: Style.color.control.content.basic
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

						color: Style.color.control.content.basic
						text: model.time
					}
					GText {
						id: notificationBody

						color: model.type === "developermode" ? Style.color.warning : Style.color.control.content.basic
						text: model.text
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
