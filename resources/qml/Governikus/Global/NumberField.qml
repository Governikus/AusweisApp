/*
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0

Item {
	id: root

	readonly property bool validInput: echoField.acceptableInput && confirmedInput
	readonly property bool confirmedInput: inputConfirmation.length != text.length || inputConfirmation === text
	readonly property real eyeWidth: eye.width + eye.anchors.leftMargin
	property alias text: echoField.text
	property alias passwordLength: echoField.maximumLength
	property string inputConfirmation

	function append(number) { echoField.insert(echoField.length, number) }
	function removeLast() { echoField.remove(echoField.length - 1, echoField.length) }

	height: Math.max(layout.height, eye.height)
	width: layout.width + eyeWidth

	activeFocusOnTab: true
	Accessible.role: Accessible.EditableText
	Accessible.name: (eye.activated
					  //: LABEL DESKTOP_QML Screenreader text for the password field
					  ? qsTr("The password is visible.")
					  //: LABEL DESKTOP_QML Screenreader text for the password field
					  : qsTr("The password is hidden.")
					  //: LABEL DESKTOP_QML Screenreader text for the password field
					  ) + " " + qsTr("You entered %1 of %2 digits.").arg(text.length).arg(passwordLength)
					  + SettingsModel.translationTrigger
	Keys.onPressed: event.accepted = root.handleKeyEvent(event.key, event.modifiers)

	function handleKeyEvent(eventKey, eventModifiers = Qt.NoModifier) {
		if (eventKey >= Qt.Key_0 && eventKey <= Qt.Key_9) {
			root.append(eventKey - Qt.Key_0)
		}
		else if (eventKey === Qt.Key_Backspace) {
			root.removeLast()
		}
		else if (eventKey === Qt.Key_Delete) {
			echoField.clear()
		}
		else if (eventKey === Qt.Key_Paste || (eventKey === Qt.Key_V) && (eventModifiers & Qt.ControlModifier)) {
			echoField.paste()
		}
		else {
			return false
		}

		// Otherwise focus is lost if last clicked button gets invisible
		// like 'C' in NumberPad.
		root.forceActiveFocus()
		return true
	}

	FontMetrics {
		id: fontMetrics

		font.bold: true
		font.pixelSize: Constants.is_desktop ? ApplicationModel.scaleFactor * 65 : 24
	}

	TextInput {
		id: echoField

		visible: false

		maximumLength: 6
		validator: RegExpValidator {
			regExp: new RegExp("[0-9]{" + echoField.maximumLength + "}")
		}
	}

	FocusFrame {
		framee: layout

		MouseArea {
			anchors.fill: parent

			cursorShape: Qt.PointingHandCursor
			acceptedButtons: Qt.AllButtons

			onPressAndHold: root.handleKeyEvent(Qt.Key_Paste)
			onClicked: {
				root.forceActiveFocus()
				if (mouse.button == Qt.RightButton || mouse.button == Qt.MiddleButton) {
					echoField.paste()
				}
			}
		}
	}

	RowLayout {
		id: layout

		spacing: Constants.is_desktop ? Constants.component_spacing : Constants.groupbox_spacing

		Repeater {
			model: root.passwordLength

			Text {
				readonly property real markerWidth: fontMetrics.averageCharacterWidth * 1.4

				Layout.preferredWidth: markerWidth
				Layout.preferredHeight: fontMetrics.height + Constants.text_spacing

				verticalAlignment: Text.AlignTop
				horizontalAlignment: Text.AlignHCenter
				text: eye.activated ? root.text.substr(index, 1) : ""
				color: Constants.is_desktop ? Style.color.primary_text_inverse : Style.color.primary_text
				font: fontMetrics.font

				Rectangle {
					width: parent.markerWidth
					height: Constants.is_desktop ? Math.max(ApplicationModel.scaleFactor * 4, 1) : 1
					anchors {
						bottom: parent.bottom
						horizontalCenter: parent.horizontalCenter
					}

					color: parent.color
				}

				Rectangle {
					visible: !eye.activated && root.text.charAt(index) !== ""
					width: fontMetrics.averageCharacterWidth
					height: width
					anchors {
						centerIn: parent
						verticalCenterOffset: -Constants.text_spacing / 2
					}

					radius: height / 2
					color: parent.color
				}
			}
		}
	}

	Button {
		id: eye

		property bool activated: false

		height: implicitHeight + (Constants.is_desktop ? 0 : Constants.text_spacing)
		width: implicitWidth + (Constants.is_desktop ? 0 : Constants.text_spacing)

		anchors {
			verticalCenter: parent.verticalCenter
			leftMargin: Constants.groupbox_spacing
			right: parent.right
		}

		Accessible.onPressAction: if (Qt.platform.os === "ios") clicked(null)

		text: (activated
			  //: LABEL DESKTOP_QML Screenreader text for the eye icon to change the password visibility
			  ? qsTr("Press to hide the password")
			  //: LABEL DESKTOP_QML Screenreader text for the eye icon to change the password visibility
			  : qsTr("Press to show the password")
			  ) + SettingsModel.translationTrigger

		contentItem: Item {}
		background: TintableIcon {
			tintColor: Constants.is_desktop ? Style.color.secondary_text_inverse : Style.color.secondary_text
			source: eye.activated ? "qrc:///images/visibility.svg" : "qrc:///images/visibility_off.svg"
			sourceSize.height: Constants.is_desktop ? Style.dimens.large_icon_size : Style.dimens.small_icon_size
			fillMode: Image.Pad
		}

		onVisibleChanged: if (visible) activated = false
		onClicked: eye.activated = !eye.activated

		MouseArea {
			anchors.fill: parent

			cursorShape: Qt.PointingHandCursor
			acceptedButtons: Qt.NoButton
		}

		FocusFrame {}
	}
}
