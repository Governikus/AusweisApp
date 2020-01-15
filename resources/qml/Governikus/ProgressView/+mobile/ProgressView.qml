/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0

SectionPage
{
	id: baseItem
	property alias text: text.text
	property alias subText: subText.text
	property alias subTextColor: subText.color
	property alias progressText: progressText.text
	property int progressValue
	property alias progressBarVisible: progressBar.visible

	BusyIndicator {
		id: busyIndicator
		anchors.fill: circle
		running: baseItem.visible
		contentItem: NpaBusyIndicatorStyle { factor: 1.2 }
	}
	Rectangle {
		id: circle
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.top
		anchors.verticalCenterOffset: Constants.is_tablet ? parent.height / 4 : parent.width / 2
		width: parent.height / 4
		height: width
		radius: width / 2
		color: Constants.blue
	}
	GText {
		id: text
		anchors.top: circle.bottom
		anchors.topMargin: 50
		anchors.horizontalCenter: parent.horizontalCenter
		horizontalAlignment: Text.AlignHCenter
		width: baseItem.width * 0.8
		textStyle: Style.text.header_accent
	}
	GText {
		id: subText
		horizontalAlignment: Text.AlignHCenter
		anchors.top: text.bottom
		anchors.topMargin: 10
		anchors.horizontalCenter: parent.horizontalCenter
		width: baseItem.width * 0.8
		textStyle: Style.text.normal_secondary
	}
	GText {
		id: progressText
		horizontalAlignment: Text.AlignHCenter
		anchors.top: subText.bottom
		anchors.topMargin: 20
		anchors.horizontalCenter: parent.horizontalCenter
		width: baseItem.width * 0.8
		textStyle: Style.text.normal_secondary
	}
	ProgressBar {
		id: progressBar
		anchors.top: progressText.bottom
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.topMargin: 10
		width: baseItem.width * 0.6
		height: 24
		from: 0
		to: 100
		visible: false
		value: progressValue

		background: Rectangle {
			radius: 2
			color: Constants.lightgrey
		}

		contentItem: Item {
			Rectangle {
				width: progressBar.visualPosition * parent.width
				height: parent.height
				radius: 2
				color: Constants.green
			}
		}
	}
}
