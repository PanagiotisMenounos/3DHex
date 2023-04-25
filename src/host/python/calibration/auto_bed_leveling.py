from PyQt5.QtCore import Qt, QThread, pyqtSignal
import os
import numpy as np
from scipy.interpolate import Rbf
from numpy import loadtxt
class ABL_Interpolation(QThread):
    message = pyqtSignal(str)

    def __init__(self, main_window):
        super().__init__()
        self.main_window = main_window

    def run(self):
        method = str(self.main_window.comboBox1.currentText())
        view_percentage = int(self.main_window.comboBox2.currentText().replace("%", ""))
        width = float(self.main_window.b53.toPlainText().strip())
        length = float(self.main_window.b54.toPlainText().strip())
        grid = float(self.main_window.b66.toPlainText().strip())

        x = loadtxt(os.getenv('LOCALAPPDATA') + '\\3DHex2\\settings\\Printer' + str(self.main_window.printer) + '\\abl_x.txt')
        y = loadtxt(os.getenv('LOCALAPPDATA') + '\\3DHex2\\settings\\Printer' + str(self.main_window.printer) + '\\abl_y.txt')
        z = loadtxt(os.getenv('LOCALAPPDATA') + '\\3DHex2\\settings\\Printer' + str(self.main_window.printer) + '\\abl_z.txt')

        xx = np.arange(0, width + 1, grid)
        yy = np.arange(0, length + 1, grid)
        xx, yy = np.meshgrid(xx, yy)

        if self.main_window.ABL_INTERPOLATION_TYPE == 1:  # https://www.geeksforgeeks.org/program-to-find-equation-of-a-plane-passing-through-3-points/
            a1 = x[2] - x[1]
            b1 = y[2] - y[1]
            c1 = z[2] - z[1]
            a2 = x[3] - x[1]
            b2 = y[3] - y[1]
            c2 = z[3] - z[1]
            a = b1 * c2 - b2 * c1
            b = a2 * c1 - a1 * c2
            c = a1 * b2 - b1 * a2
            d = (- a * x[1] - b * y[1] - c * z[1])
            # print ("equation of plane is "+str(a)+ "x +"+str(b)+ "y +"+str(c)+ "z +"+str(d)+ "= 0.")
        elif self.main_window.ABL_INTERPOLATION_TYPE == 2:
            rbfi = Rbf(x, y, z, function=method)  # radial basis function interpolator instance
            znew = rbfi(xx, yy)  # interpolated values              

        file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\settings\\XYZ.txt', 'w')
        size = xx.ravel().size
        view_step = int((100 / view_percentage) * grid)

        if self.main_window.ABL_INTERPOLATION_TYPE == 1:
            i = 1  # remove center point
            while i < size:
                z = (-d - (a * xx.ravel()[i]) - (b * yy.ravel()[i])) / c
                file.write(str(xx.ravel()[i]) + ' ' + str(yy.ravel()[i]) + ' ' + str(
                    round(z * int(self.main_window.b3.toPlainText().strip()))) + '\n')
                i = i + 1
            file.close()
        elif self.main_window.ABL_INTERPOLATION_TYPE == 2:
            i = 0
            while i < size:
                file.write(str(xx.ravel()[i]) + ' ' + str(yy.ravel()[i]) + ' ' + str(
                    round(znew.ravel()[i] * int(self.main_window.b3.toPlainText().strip()))) + '\n')
                i = i + 1
            file.close()

        view_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\settings\\XYZ_view.txt', 'w')

        xx = np.arange(0, width, view_step)
        yy = np.arange(0, length, view_step)
        xx, yy = np.meshgrid(xx, yy)
        size = xx.ravel().size

        if self.main_window.ABL_INTERPOLATION_TYPE == 1:
            i = 1
            while i < size:
                z = (-d - (a * xx.ravel()[i]) - (b * yy.ravel()[i])) / c
                view_file.write(str(xx.ravel()[i]) + ' ' + str(yy.ravel()[i]) + ' ' + str(z) + '\n')
                if (i % view_step == 0):
                    view_file.write(str(xx.ravel()[i]) + ' ' + str(yy.ravel()[i]) + ' ' + str(z) + '\n')
                i = i + 1
            view_file.close()
            self.message.emit("Plot BED plane")
        elif self.main_window.ABL_INTERPOLATION_TYPE == 2:
            i = 0
            rbfi = Rbf(x, y, z, function=method)  # radial basis function interpolator instance
            znew = rbfi(xx, yy)
            while i < size:
                view_file.write(str(xx.ravel()[i]) + ' ' + str(yy.ravel()[i]) + ' ' + str(znew.ravel()[i]) + '\n')
                if (i % view_step == 0):
                    view_file.write(str(xx.ravel()[i]) + ' ' + str(yy.ravel()[i]) + ' ' + str(znew.ravel()[i]) + '\n')
                i = i + 1
            view_file.close()
            self.message.emit("Plot BED mesh")   