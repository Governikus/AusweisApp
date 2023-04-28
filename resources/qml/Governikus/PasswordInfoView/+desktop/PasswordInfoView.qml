/**
 * Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.PinResetInformationModel 1.0

SectionPage {
	id: root

	readonly property url buttonLink: infoContent.buttonLink
	readonly property string buttonText: infoContent.buttonText
	readonly property var contentList: infoContent.contentList
	readonly property string hint: infoContent.hint
	readonly property int imageType: infoContent.imageType
	property var infoContent: PasswordInfoContent {
	}
	property alias rootEnabled: titleBarAction.rootEnabled
	readonly property string title: infoContent.title

	signal abortCurrentWorkflow
	signal close

	titleBarAction: TitleBarAction {
		id: titleBarAction
		rootEnabled: ApplicationModel.currentWorkflow === ApplicationModel.WORKFLOW_NONE
		showHelp: false
		showSettings: false
		text: root.title
	}

	Keys.onEnterPressed: close()
	Keys.onEscapePressed: close()
	Keys.onReturnPressed: close()

	GFlickableColumnLayout {
		anchors.fill: parent
		anchors.margins: Constants.pane_padding
		spacing: Constants.groupbox_spacing

		GridLayout {
			Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
			Layout.fillWidth: true
			columnSpacing: Constants.groupbox_spacing
			columns: 2
			rowSpacing: Constants.groupbox_spacing

			ColumnLayout {
				id: contentLayout
				Layout.alignment: Qt.AlignTop
				Layout.fillWidth: true
				Layout.maximumWidth: Style.dimens.max_text_width
				spacing: Constants.component_spacing

				Repeater {
					model: root.contentList

					delegate: ColumnLayout {
						Layout.alignment: Qt.AlignTop
						Layout.fillWidth: true
						spacing: Constants.component_spacing

						PasswordInfoImage {
							Layout.alignment: Qt.AlignHCenter
							imageType: blockHeaderImageType
							scaleFactorCan: 1 * ApplicationModel.scaleFactor
							scaleFactorGeneral: 1.35 * ApplicationModel.scaleFactor
							visible: blockHeaderImageType !== PasswordInfoImage.Type.NONE
						}
						ColumnLayout {
							Layout.alignment: Qt.AlignTop
							Layout.fillWidth: true
							spacing: Constants.groupbox_spacing

							GText {
								Layout.alignment: Qt.AlignTop
								Layout.fillWidth: true
								Layout.maximumWidth: Style.dimens.max_text_width
								activeFocusOnTab: true
								horizontalAlignment: Text.AlignLeft
								text: blockTitle
								textStyle: Style.text.header_highlight
								visible: text !== ""
								wrapMode: Text.WordWrap

								FocusFrame {
								}
							}
							Repeater {
								model: paragraphList

								delegate: GText {
									Layout.alignment: Qt.AlignTop
									Layout.fillWidth: true
									Layout.maximumWidth: Style.dimens.max_text_width
									activeFocusOnTab: true
									horizontalAlignment: Text.AlignJustify
									text: modelData
									textStyle: Style.text.header

									FocusFrame {
									}
								}
							}
						}
					}
				}
				GButton {
					Layout.alignment: Qt.AlignRight
					Layout.topMargin: Constants.component_spacing
					icon.source: "qrc:///images/material_open_in_new.svg"
					text: root.buttonText
					visible: text !== ""

					onClicked: Qt.openUrlExternally(root.buttonLink)
				}
			}
			PasswordInfoImage {
				imageType: root.imageType
				scaleFactorCan: 1.5 * ApplicationModel.scaleFactor
				scaleFactorGeneral: 2.7 * ApplicationModel.scaleFactor
				visible: root.imageType !== PasswordInfoImage.Type.NONE
			}
			GSpacer {
				Layout.columnSpan: 2
				Layout.fillHeight: true
				Layout.row: 1
			}
			Hint {
				Layout.alignment: Qt.AlignHCenter
				Layout.fillWidth: true
				Layout.maximumWidth: contentLayout.width
				Layout.row: 2
				buttonText: PinResetInformationModel.pinResetActionText
				text: root.hint
				visible: text !== ""

				onClicked: {
					abortCurrentWorkflow();
					Qt.openUrlExternally(PinResetInformationModel.pinResetUrl);
				}
			}
		}
		NavigationButton {
			Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
			buttonType: NavigationButton.Type.Back

			onClicked: root.close()
		}
	}
}
