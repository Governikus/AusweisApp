/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick

Colors {
	background: "#232323"
	border: "#ffffff"
	fail: "#ff9b29"
	focus_indicator: "#0098eb"
	image: "#0098eb"
	progressbar_text: textNormal.basic
	success: "#5fcb01"
	warning: "#ff9b29"

	control: DefaultControlColors {
		background.basic: "#0098eb"
		background.checked: "#232323"
		border.basic: "#0098eb"
		border.checked: "#ffffff"
		content.basic: "#232323"
		content.checked: "#ffffff"
	}
	controlCheckbox: DefaultControlColors {
		background.checked: transparent
		background.pressed: "#80cdec"
		border.pressed: "#80cdec"
		content.basic: "#ffffff"
		content.pressed: "#ffffff"
	}
	controlRadiobutton: DefaultControlColors {
		background.pressed: "#0098eb"
		content.basic: "#ffffff"
	}
	controlScrollbar: DefaultControlColors {
		background.basic: "#0098eb"
		background.checked: "#232323"
		background.pressed: "#80cdec"
		border.basic: "#0098eb"
		border.checked: "#ffffff"
		border.pressed: "#80cdec"
		content.basic: "#232323"
		content.checked: "#ffffff"
		content.pressed: "#ffffff"
	}
	controlSwitch: DefaultControlColors {
		background.pressed: "#80cdec"
		border.pressed: "#80cdec"
		content.basic: "#ffffff"
		content.pressed: "#ffffff"
	}
	linkBasic: DefaultLinkColors {
	}
	linkNavigation: DefaultLinkColors {
	}
	linkTitle: DefaultLinkColors {
		basic: "#ffffff"
		hovered: "#80cdec"
		pressed: "#0098eb"
	}
	pane: DefaultPaneColors {
	}
	paneSublevel: DefaultPaneColors {
		background.basic: pane.background.hovered
		border.basic: "#576164"
	}
	textHeadline: DefaultTextColors {
	}
	textNormal: DefaultTextColors {
		checked: "#000000"
	}
	textSubline: DefaultTextColors {
		basic: "#80cdec"
		disabled: "#798183"
	}
	textTitle: DefaultTextColors {
		basic: "#80cdec"
		checked: "#ffffff"
		disabled: "#798183"
	}

	component DefaultControlColors: ControlComponents {
		background: ComponentColors {
			basic: "#232323"
			checked: "#0098eb"
			disabled: "#303030"
			hovered: "#80cdec"
			pressed: transparent
		}
		border: ComponentColors {
			basic: "#ffffff"
			checked: "#0098eb"
			disabled: "#798183"
			hovered: "#80cdec"
			pressed: "#0098eb"
		}
		content: ComponentColors {
			basic: "#ffffff"
			checked: "#232323"
			disabled: "#798183"
			hovered: "#ffffff"
			pressed: "#0098eb"
		}
	}
	component DefaultLinkColors: ComponentColors {
		basic: "#80cdec"
		checked: basic
		disabled: "#bcc0c1"
		hovered: "#0098eb"
		pressed: "#0b7ab7"
	}
	component DefaultPaneColors: CoreComponents {
		background: ComponentColors {
			basic: "#303030"
			checked: "#0098eb"
			disabled: "#303030"
			hovered: lighter(basic, 0.05)
			pressed: lighter(basic, 0.10)
		}
		border: ComponentColors {
			basic: software_renderer ? "#ffffff" : transparent
			checked: basic
			disabled: basic
			hovered: basic
			pressed: basic
		}
	}
	component DefaultTextColors: ComponentColors {
		basic: "#ffffff"
		checked: basic
		disabled: "#bcc0c1"
		hovered: basic
		pressed: basic
	}
}
