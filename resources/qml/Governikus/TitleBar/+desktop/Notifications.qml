/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.NotificationModel 1.0
import Governikus.Type.SettingsModel 1.0


Item {
	id: baseItem

	visible: SettingsModel.showInAppNotifications

	readonly property string icon: {
		if (d.unreadMsg) {
			if (NotificationModel.lastType === "developermode") return "qrc:///images/desktop/bell_red.svg"
			if (NotificationModel.lastType === "feedback") return "qrc:///images/desktop/bell_green.svg"
		}
		return "qrc:///images/desktop/bell_white.svg"
	}
	signal newNotification()
	function toggle() {
		if (!fadingAnimation.running) d.fadeIn = !d.fadeIn
		if (d.fadeIn) {
			logEntryList.positionViewAtEnd()
			d.unreadMsg = false
		}
	}

	QtObject {
		id: d

		property bool fadeIn: false
		property bool unreadMsg: false
	}

	Connections {
		target: NotificationModel
		onRowsInserted: {
			if (!d.fadeIn) d.unreadMsg = true
			baseItem.newNotification()
		}
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
		color: Qt.lighter(Constants.blue, 1.15)

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

		ListView {
			id: logEntryList

			anchors.fill: parent
			anchors.margins: spacing
			anchors.rightMargin: 2 * spacing

			clip: true
			spacing: ApplicationModel.scaleFactor * 10
			model: NotificationModel
			delegate: Row {
				spacing: logEntryList.spacing

				GText {
					id: notificationTime

					text: model.time
				}

				GText {
					width: logEntryList.width - notificationTime.width - spacing

					text: model.text
					textStyle: model.type === "developermode" ? Style.text.normal_warning : Style.text.normal

					onLinkActivated: Qt.openUrlExternally(link)
				}
			}

			ScrollBar.vertical: ScrollBar {
				policy: ScrollBar.AlwaysOn
			}
		}
	}
}
