def setup_style_sheet(self):
    self.style_sheet = """
        QMenu {
            color: rgb(255, 255, 255);
            background-color: rgb(47, 47, 47);
        }

        QMenuBar {
            color: rgb(255, 255, 255);
            background-color: rgb(47, 47, 47);
        }

        QMenu::item:selected {
            background-color: rgb(83, 83, 83);
        }

        QMenuBar::item:selected {
            background-color: rgb(83, 83, 83);
        }
    """
    self.setStyleSheet(self.style_sheet)