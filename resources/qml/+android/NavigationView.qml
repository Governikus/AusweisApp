import QtQuick 2.5
import QtQuick.Controls 2.0

import "global"


Rectangle {
	id: content
	height: appWindow.height - Constants.titlebar_height
	width: Utils.dp(250)
	color: Constants.background_color

	property var navigationController: null

	ListModel {
		id: navModel

		ListElement {
			image: "qrc:///images/android/navigation/ausweisen.png"
			desc: qsTr("Identify")
			condition: "identify"
		}

		ListElement {
			image: "qrc:///images/android/navigation/anbieter.png"
			desc: qsTr("Provider")
			condition: "provider"
		}

		ListElement {
			image: "qrc:///images/android/navigation/verlauf.png"
			desc: qsTr("History")
			condition: "history"
		}

		ListElement {
			image: "qrc:///images/android/navigation/pin.png"
			desc: qsTr("Pin")
			condition: "pin"
		}

		ListElement {
			image: "qrc:///images/android/navigation/versionsinformation.png"
			desc: qsTr("Versioninformation")
			condition: "versionInformation"
		}

		ListElement {
			image: "qrc:///images/android/navigation/faq.png"
			desc: qsTr("FAQ")
			condition: "external"
			external: qsTr("https://www.ausweisapp.bund.de/en/service/haeufig-gestellte-fragen/")
		}

		ListElement {
			image: "qrc:///images/android/navigation/support.png"
			desc: qsTr("Support")
			condition: "external"
			external: qsTr("https://www.ausweisapp.bund.de/en/service/support/")
		}

		ListElement {
			image: "qrc:///images/android/navigation/bewerten.png"
			desc: qsTr("Rate app")
			condition: "external"
			external: "market://details?id=com.governikus.ausweisapp2"
		}

		ListElement {
			image: "qrc:///images/android/navigation/teilen.png"
			desc: qsTr("Share...")
			condition: "share"
			share: qsTr("I'm using AusweisApp2, download it here for Android: https://play.google.com/store/apps/details?id=com.governikus.ausweisapp2 or here for iOS: https://itunes.apple.com/de/app/wikipedia-mobile/id324715238?mt=8")
			shareTitle: qsTr("Share with")
		}

		ListElement {
			image: "qrc:///images/zahnraeder.svg"
			desc: qsTr("Developer options")
			condition: "developeroptions"
		}
	}


	ListView {
		id: listView
		anchors.fill: parent
		boundsBehavior: Flickable.StopAtBounds
		model: navModel
		currentIndex: navigationController.currentIndex

		highlight: Rectangle {
			color: "black"
			opacity: 0.1
			height: Utils.dp(45)
			width: content.width
			y: listView.currentItem.y
		}
		highlightFollowsCurrentItem: false

		delegate: NavigationItem {
			height: Utils.dp(45)
			width: content.width
			source: image
			text: desc
			onClicked: {
				if (condition === "share") {
					shareUtil.shareText(share, shareTitle)
				}
				else if (condition === "external") {
					Qt.openUrlExternally(external)
				}
				else {
					navigationController.currentIndex = index
					navigationController.state = condition
				}

				navigationController.close()
			}
			// Hide developer options if we are not using developer build (debug build)
			visible: condition !== "developeroptions" || plugin.developerBuild
		}
	}
}
