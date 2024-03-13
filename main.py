from PyQt6.QtWidgets import QApplication, QPushButton, QWidget, QHBoxLayout

from views.task_view import TaskView

app = QApplication([])

#  创建一个窗口，窗口分为3列
#  第一列是用于选择文件、或者文件夹的按钮
#  第二列是一个列表用于展示第一列当前选中的文件或者文件夹里面的所有mp4文件
#  第三列是第二列选中文件的信息展示，包含了分辨率、大小、时长、码率等信息。还有一个按钮点击开始压缩，点击后显示压缩进度。
#  压缩完成，出现一个按钮，以及压缩后的文件的分辨率、大小信息。让用户选择是否覆盖到当前文件
window = QWidget()
window.setWindowTitle("Vap压缩")
window.setGeometry(0, 0, 600, 400)
task_view = TaskView()
task_view.initial_ui()
layout = QHBoxLayout()
layout.addWidget(task_view)
layout.addWidget(QPushButton("Center"))
layout.addWidget(QPushButton("Right"))
window.setLayout(layout)
window.show()
app.exec()
