/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */


PlatformColors {
	readonly property color transparent: "transparent"

	readonly property color shadow: "#808080"
	readonly property color high_contrast_item_border: transparent

	readonly property color primary_text: "#333333"
	readonly property color primary_text_inverse: "#ffffff"
	readonly property color secondary_text: "#666666"
	readonly property color secondary_text_inverse: "#dadada"
	readonly property color accent_text: accent
	readonly property color accent_text_inverse: "#cde1f3"
	readonly property color warning_text: "#cc0000"

	readonly property color accent: "#4d7aa4"
	readonly property color accent_disabled: "#d0d0d0"

	readonly property color button: accent
	readonly property color button_disabled: accent_disabled
	readonly property color button_text: primary_text_inverse
	readonly property color button_text_disabled: secondary_text

	readonly property color focus_indicator: primary_text
	readonly property color focus_indicator_inverse: primary_text_inverse
}
