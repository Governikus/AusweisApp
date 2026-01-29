/**
 * Copyright (c) 2019-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.Global
import Governikus.Style
import Governikus.Type
import Governikus.View

Item {
	id: root

	readonly property color iconColor: d.unreadMsg ? Style.color.textSubline.basic_unchecked : Style.color.textNormal.basic_unchecked
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
		border.color: Style.color.control.border.basic_unchecked
		border.width: Style.dimens.border_width
		color: Style.color.control.background.basic_unchecked
		height: UiPluginModel.scaleFactor * 120
		radius: Style.dimens.control_radius
		visible: anchors.leftMargin < 0
		width: UiPluginModel.scaleFactor * 480

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
			scrollBarColor: Style.color.control.content.basic_unchecked
			scrollBarTopPadding: spacing
			spacing: Style.dimens.text_spacing
			topMargin: Style.dimens.pane_padding

			delegate: Item {
				id: logEntry

				required property int index
				required property string text
				required property string time

				Accessible.name: notificationTime.text + " " + notificationBody.text
				Accessible.role: Accessible.StaticText
				implicitHeight: row.height
				implicitWidth: row.width + 2 * Style.dimens.pane_padding

				onActiveFocusChanged: if (activeFocus) {
					logEntryList.handleItemFocused(index);
				}

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
						//: DESKTOP %1 will be replaced with a notification text
						logEntry.Accessible.announce(qsTr("Notification: %1").arg(notificationBody.text));
					}

					GText {
						id: notificationTime

						Accessible.ignored: true
						color: Style.color.control.content.basic_unchecked
						text: logEntry.time
					}
					GText {
						id: notificationBody

						Accessible.ignored: true
						color: Style.color.control.content.basic_unchecked
						text: logEntry.text
						width: logEntryList.width - 2 * Style.dimens.pane_padding - notificationTime.width - 3 * logEntryList.spacing
					}
				}
				FocusFrame {
					borderColor: Style.color.control.content.basic_unchecked
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
