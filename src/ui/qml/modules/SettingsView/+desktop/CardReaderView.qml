/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Animations
import Governikus.Global
import Governikus.Style
import Governikus.Type

GPane {
	id: root

	property bool enablePaneOptic: true
	readonly property bool hasConnectedReader: readerRepeater.count > 0
	readonly property string hintTextBase: {
		//: LABEL DESKTOP
		qsTr("After connecting a new card reader it may take a few seconds to recognize the driver.") + "<br>" +
		//: LABEL DESKTOP
		qsTr("It may be necessary to restart your system after installing the driver.");
	}
	property alias showHint: hintAndDateText.visible

	color: root.enablePaneOptic ? Style.color.pane.background.basic : Style.color.transparent
	contentPadding: root.enablePaneOptic ? Style.dimens.pane_padding : 0
	layer.enabled: root.enablePaneOptic
	//: LABEL DESKTOP
	title: qsTr("Connected USB card readers")
	titleMargins: root.enablePaneOptic ? Style.dimens.pane_padding : 0

	ReaderScanEnabler {
		id: readerScanEnabler

		pluginType: ReaderManagerPluginType.PCSC
	}
	ColumnLayout {
		//: LABEL DESKTOP
		Accessible.name: qsTr("List of connected card readers.")
		Accessible.role: Accessible.List
		spacing: Style.dimens.pane_spacing
		visible: root.hasConnectedReader

		Repeater {
			id: readerRepeater

			model: ReaderModel.sortedModel

			delegate: CardReaderDelegate {
				Layout.fillWidth: true
			}
		}
	}
	RowLayout {
		spacing: Style.dimens.groupbox_spacing
		visible: !readerScanEnabler.scanRunning

		StatusAnimation {
			sourceSize.height: Style.dimens.large_icon_size
			symbol.type: Symbol.Type.WARNING
		}
		GText {
			//: LABEL DESKTOP
			text: qsTr("The connection to your system's smartcard service could not be established. You can try to resolve this issue and restart the scan.")
		}
		GButton {
			Layout.minimumWidth: implicitWidth
			//: LABEL DESKTOP
			text: qsTr("Restart smartcard scan")

			onClicked: readerScanEnabler.restartScan()
		}
	}
	CardReaderDelegate {
		Layout.fillWidth: true
		readerHTMLDescription: ""
		readerImagePath: "qrc:///images/desktop/default_reader.png"
		readerInstalled: false
		//: LABEL DESKTOP
		readerName: qsTr("No card reader connected")
		readerSupported: true
		showInstalledSupportedIcon: false
		visible: !root.hasConnectedReader
	}
	GSeparator {
		Layout.fillWidth: true
		visible: !root.hasConnectedReader && hintAndDateText.visible
	}
	RowLayout {
		id: hintAndDateText

		Layout.topMargin: Style.dimens.pane_spacing
		spacing: Style.dimens.groupbox_spacing

		TintableIcon {
			source: "qrc:/images/info.svg"
			sourceSize.height: Style.dimens.large_icon_size
			tintColor: hintText.color
		}
		GText {
			id: hintText

			Layout.alignment: Qt.AlignVCenter
			color: Style.color.textSubline.basic
			text: {
				root.hintTextBase + " " +
				//: LABEL DESKTOP
				qsTr("Only connected card readers are shown here.") + " " + ReaderModel.lastUpdatedInformation;
			}
			verticalAlignment: Text.AlignBottom
		}
	}
}
