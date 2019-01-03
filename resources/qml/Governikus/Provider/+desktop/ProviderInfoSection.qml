import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Type.ApplicationModel 1.0


Row {
	property alias image: icon.source
	property alias title: text.label
	property string name: ""

	height: text.height
	spacing: Constants.groupbox_spacing

	Image {
		id: icon
		sourceSize.height: ApplicationModel.scaleFactor * 40
		anchors.verticalCenter: text.verticalCenter
	}

	LabeledText {
		id: text
		text: name.length > 0 ? name : qsTr("See details under \"more...\"") + settingsModel.translationTrigger
	}
}
