from PyQt6.QtWidgets import QApplication, QLabel, QWidget, QFormLayout, QPushButton, QGridLayout


class TaskView(QWidget):
    def __init__(self):
        super().__init__()
        self.choose_button = QPushButton("选择文件")
        self.choose_button.setStyleSheet("background-color:white")
        self.task_list = []
        self.layout = QGridLayout()

    def initial_ui(self):
        self.layout.addWidget(self.choose_button)
        self.layout.addWidget(QLabel("Center"))
        self.setLayout(self.layout)

        #  设置背景颜色为白色
        self.setStyleSheet("background-color: white")  
        None
