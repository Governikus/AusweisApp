/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick

Colors {
	background: "#ffffff"
	border: "#bbbbbb"
	fail: "#db6a00"
	focus_indicator: "#0077b6"
	image: "#0077b6"
	progressbar_text: "#232323"
	success: "#3e8401"
	warning: "#db6a00"

	control: DefaultControlColors {
		background.basic: "#0077b6"
		background.checked: "#ffffff"
		border.basic: "#0077b6"
		border.checked: "#576164"
		content.basic: "#ffffff"
		content.checked: "#576164"
	}
	controlCheckbox: DefaultControlColors {
		background.basic: transparent
		background.pressed: "#80cdec"
		border.pressed: "#80cdec"
		content.pressed: "#ffffff"
	}
	controlRadiobutton: DefaultControlColors {
		background.pressed: "#0077b6"
	}
	controlScrollbar: DefaultControlColors {
		background.basic: "#0077b6"
		background.checked: "#ffffff"
		background.pressed: "#80cdec"
		border.basic: "#0077b6"
		border.checked: "#576164"
		border.pressed: "#80cdec"
		content.basic: "#ffffff"
		content.checked: "#576164"
		content.pressed: "#ffffff"
	}
	controlSwitch: DefaultControlColors {
		background.pressed: "#80cdec"
		border.pressed: "#80cdec"
		content.pressed: "#ffffff"
	}
	linkBasic: DefaultLinkColors {
		hovered: "#379ec8"
	}
	linkNavigation: DefaultLinkColors {
		basic: "#004b76"
	}
	linkTitle: DefaultLinkColors {
		basic: "#576164"
	}
	pane: DefaultPaneColors {
	}
	paneSublevel: DefaultPaneColors {
		background.basic: "#f2f3f4"
		border.basic: "#f2f3f4"
	}
	textHeadline: DefaultTextColors {
	}
	textNormal: DefaultTextColors {
		checked: "#ffffff"
	}
	textSubline: DefaultTextColors {
		basic: "#0077b6"
		disabled: "#80cdec"
	}
	textTitle: DefaultTextColors {
		basic: "#004b76"
		checked: "#0077b6"
		disabled: "#798183"
	}

	component DefaultControlColors: ControlComponents {
		background: ComponentColors {
			basic: "#ffffff"
			checked: "#0077b6"
			disabled: "#bcc0c1"
			hovered: "#80cdec"
			pressed: transparent
		}
		border: ComponentColors {
			basic: "#576164"
			checked: "#0077b6"
			disabled: "#bcc0c1"
			hovered: "#80cdec"
			pressed: "#0077b6"
		}
		content: ComponentColors {
			basic: "#576164"
			checked: "#ffffff"
			disabled: "#ffffff"
			hovered: "#ffffff"
			pressed: "#0077b6"
		}
	}
	component DefaultLinkColors: ComponentColors {
		basic: "#0077b6"
		checked: basic
		disabled: "#bcc0c1"
		hovered: "#0077b6"
		pressed: "#80cdec"
	}
	component DefaultPaneColors: CoreComponents {
		background: ComponentColors {
			basic: "#ffffff"
			checked: "#0077b6"
			disabled: "#f2f3f4"
			hovered: darker(basic, 0.05)
			pressed: darker(basic, 0.10)
		}
		border: ComponentColors {
			basic: software_renderer ? "#bbbbbb" : transparent
			checked: basic
			disabled: basic
			hovered: basic
			pressed: basic
		}
	}
	component DefaultTextColors: ComponentColors {
		basic: "#576164"
		checked: basic
		disabled: "#bcc0c1"
		hovered: basic
		pressed: basic
	}
}
