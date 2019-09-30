/*
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0

Item {
	property alias text: t.text
	height: 50
	width: height

	BusyIndicator {
		id: busy
		anchors.centerIn: parent
		height: rec.height * 1.1
		width: height
		running: false
		contentItem: NpaBusyIndicatorStyle {}
	}

	Rectangle {
		id: rec
		border.width: 1
		border.color: Constants.blue
		color: parent.state === "active" ? Style.color.accent : Constants.white
		height: parent.state === "active" ? parent.height : parent.height / 2
		width: height
		radius: width * 0.5
		anchors.centerIn: parent
	}

	GText {
		id: t
		anchors.centerIn: rec
		font.bold: parent.state === "active"
		textStyle: Style.text.normal
		color: parent.state === "active" ? Constants.white : Style.color.accent
	}

	state:"inactive"
	states: [
		State { name: "active" },
		State { name: "inactive" }
	]

	transitions: [
		Transition {
			from: "inactive"
			to: "active"
			SequentialAnimation {
				ParallelAnimation {
					NumberAnimation {target: t; property: "font.pixelSize"; duration: 1000; easing.type: Easing.OutElastic}
					ColorAnimation {duration: 100}
				}
				PropertyAnimation {target: busy; property: "running"; to: true}
			}
		},
		Transition {
			from: "active"
			to: "inactive"
			SequentialAnimation {
				PropertyAction {target: busy; property: "running"; value: false}
				NumberAnimation {target: t; property: "font.pixelSize"; duration: 200}
			}
		}
	]
}
