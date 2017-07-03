import QtQuick 2.5
import QtQuick.Layouts 1.2

import "../"
import "../global"


SectionPage {
	headerTitleBarAction: TitleBarAction { text: qsTr("More"); font.bold: true }


	Rectangle {
		anchors.fill: menu
		color: "white"
	}

	Column {
		id: menu
		width: parent.width
		anchors.top: parent.top
		anchors.topMargin:  Utils.dp(40)

		MoreViewMenuItem {
			text: qsTr("Version information")
			imageSource: "qrc:///images/npa.svg"
			onClicked: push(versionInformationPage)
		}

		MoreViewMenuItem {
			text: qsTr("FAQ")
			imageSource: "qrc:///images/iOS/more/icon_mehr_info.svg"
			onClicked: Qt.openUrlExternally("https://www.ausweisapp.bund.de/service/haeufig-gestellte-fragen/")
		}

		MoreViewMenuItem {
			text: qsTr("Support")
			imageSource: "qrc:///images/iOS/more/icon_mehr_fragen.svg"
			onClicked: push(supportPage)
		}

		MoreViewMenuItem {
			text: qsTr("Rate app");
			imageSource: "qrc:///images/iOS/more/icon_mehr_favorit.svg";
			// Use Qt.platform.os here instead of platformstyle because rating market URLs on iOS doesn't work and vice versa
			onClicked: {
				if (Qt.platform.os === "android") {
					Qt.openUrlExternally("market://details?id=com.governikus.ausweisapp2")
				}
				else if (Qt.platform.os === "ios") {
					Qt.openUrlExternally("itms://itunes.apple.com/de/app/wikipedia-mobile/id324715238?mt=8")
				}
			}
		}

		MoreViewMenuItem {
			text: qsTr("Share");
			imageSource: "qrc:///images/iOS/more/icon_mehr_upload.svg";
			onClicked: qmlExtension.shareText(qsTr("I'm using Ausweisapp2, download it here for Android: https://play.google.com/store/apps/details?id=com.governikus.ausweisapp2&hl=de or here for iOS: https://itunes.apple.com/de/app/wikipedia-mobile/id324715238?mt=8"), qsTr("Share with"))
		}

		MoreViewMenuItem {
			visible: plugin.developerBuild
			text: qsTr("Developer options")
			imageSource: "qrc:///images/zahnraeder.svg"
			onClicked: push(developerView)
		}
	}

	Rectangle {
		anchors.top: menu.top
		height: 1; width: parent.width
		color: Constants.grey
	}
	Rectangle {
		anchors.bottom: menu.bottom
		height: 1; width: parent.width
		color: Constants.grey
	}

	VersionInformation {
		id: versionInformationPage
		visible: false
	}

	SupportView {
		id: supportPage
		visible: false
	}

	DeveloperView {
		id: developerView
		visible: false
	}
}
