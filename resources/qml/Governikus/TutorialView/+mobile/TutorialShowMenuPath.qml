/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Global 1.0
import Governikus.Type.SettingsModel 1.0

Item {
	id: root

	property alias newSectionImage: newSection.source
	property double newSectionPointerX: 1
	property double newSectionPointerY: 0
	property alias backgroundIcon: icon.source

	height: width * 0.6

	Image {
		id: icon

		width: 0.54 * parent.width
		height: 0.85 * parent.height
		x: 0.2167 * parent.width
		y: 0
		z: -1
	}

	TutorialZoomTriangle {
		width: 0.083 * parent.width
		height: startMenu.height
		x: startMenu.x + startMenu.width
		y: startMenu.y
		opacity: 0.3
		z: 1
	}

	TutorialImage {
		id: startMenu
		width: root.width * 0.3

		source: "qrc:///images/tutorial/screenshot_start_%1_%2.png".arg(Constants.layout).arg(SettingsModel.language)
		centerX: 0.483 + 0.1525
		centerY: 0.15 + 0.4
	}

	TutorialZoomTriangle {
		width: 0.083 * parent.width
		height: newSection.height
		x: newSection.x + newSection.width
		y: newSection.y
		opacity: 0.3
		z: 1

		pointerXRatio: root.newSectionPointerX
		pointerYRatio: root.newSectionPointerY
	}

	TutorialImage {
		id: newSection

		width: root.width * 0.3
		centerX: 0.125 + 0.1525
		centerY: 0.075 + 0.4
	}

	TutorialImage {
		width: root.width / 7.5

		source: "qrc:///images/tutorial/phone.svg"
		centerX: 0.808 + 0.0667
		centerY: 0.4 + 0.155
	}
}
