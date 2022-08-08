# Thanks to: https://yapayzekalabs.blogspot.com/2018/11/pyqt5-gui-qt-designer-matplotlib.html
from PyQt5.QtWidgets import*
from matplotlib.backends.backend_qt5agg import FigureCanvas
from matplotlib.figure import Figure

    
class MplWidget(QWidget):
    def __init__(self, parent = None):
        QWidget.__init__(self, parent)
        self.canvas = FigureCanvas(Figure(facecolor='#2F2F2F',tight_layout=True))
        vertical_layout = QVBoxLayout()
        vertical_layout.addWidget(self.canvas)
        self.canvas.axes = self.canvas.figure.add_subplot(111, facecolor='#2F2F2F',frameon=False)
        self.canvas.axes.get_xaxis().set_visible(False)
        self.canvas.axes.set_ylim(bottom=0)
        self.canvas.axes.tick_params(axis='y', colors="White")
        self.setLayout(vertical_layout)