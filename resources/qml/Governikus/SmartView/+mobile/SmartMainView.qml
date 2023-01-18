/*
 * \copyright Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.SmartModel 1.0
import Governikus.Type.PersonalizationModel 1.0
import Governikus.Type.UiModule 1.0

GFlickableColumnLayout {
	id: root

	readonly property int smartState: SmartModel.smartState

	signal checkSmart
	signal deletePersonalization
	signal deleteSmart
	signal setupSmart
	signal updateSmart

	fillHeight: false
	spacing: 0

	SmartCardView {
		Layout.alignment: Qt.AlignHCenter
		Layout.maximumHeight: Style.dimens.header_icon_size
		Layout.maximumWidth: Style.dimens.max_text_width
	}
	GText {
		Layout.alignment: Qt.AlignCenter
		Layout.fillWidth: true
		Layout.maximumWidth: Style.dimens.max_text_width
		Layout.topMargin: Constants.component_spacing
		horizontalAlignment: Text.AlignHCenter
		text: {
			switch (root.smartState) {
			case SmartModel.SMART_UPDATING_STATUS:
				//: LABEL ANDROID IOS
				return qsTr("Updating Smart-eID status...");
			case SmartModel.SMART_UNAVAILABLE:
				//: LABEL ANDROID IOS
				return qsTr("Smart-eID not supported");
			case SmartModel.SMART_UNUSABLE:
				//: LABEL ANDROID IOS
				return qsTr("Smart-eID invalid");
			case SmartModel.SMART_READY:
				//: LABEL ANDROID IOS
				return qsTr("Smart-eID ready for use");
			default:
				//: LABEL ANDROID IOS
				return qsTr("Smart-eID supported");
			}
		}
		textStyle: Style.text.header_accent
		wrapMode: Text.WordWrap
	}
	GText {
		Layout.alignment: Qt.AlignCenter
		Layout.fillWidth: true
		Layout.maximumWidth: Style.dimens.max_text_width
		Layout.topMargin: Constants.text_spacing
		horizontalAlignment: Text.AlignHCenter
		text: {
			switch (root.smartState) {
			case SmartModel.SMART_UPDATING_STATUS:
				//: LABEL ANDROID IOS
				return qsTr("Please wait a moment.");
			case SmartModel.SMART_UNAVAILABLE:
				//: LABEL ANDROID IOS
				return qsTr("Unfortunately, this functionality is not supported by your device.");
			case SmartModel.SMART_UNUSABLE:
				//: LABEL ANDROID IOS
				return qsTr("Your Smart-eID is in an invalid state. You need to set it up again to perform online identifications without your ID card.");
			case SmartModel.SMART_READY:
				//: LABEL ANDROID IOS
				return qsTr("Your Smart-eID is set up and ready for use. You can now perform online identifications without your ID card if supported by the provider.");
			default:
				//: LABEL ANDROID IOS
				return qsTr("Set up a Smart-eID in order to perform online identifications without your ID card if supported by the provider.");
			}
		}
		textStyle: Style.text.normal_secondary
		wrapMode: Text.WordWrap
	}
	SmartSettingsView {
		Layout.fillWidth: true
		Layout.topMargin: Constants.component_spacing
		visible: root.smartState !== SmartModel.SMART_UPDATING_STATUS

		onCheckSmart: root.checkSmart()
		onDeletePersonalization: root.deletePersonalization()
		onDeleteSmart: root.deleteSmart()
		onSetupSmart: root.setupSmart()
		onUpdateSmart: root.updateSmart()
	}
}
