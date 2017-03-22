from tkinter import Tk, Frame, BOTH, Button, ttk, filedialog


class FileManger():
    __filePath = './'

    @staticmethod
    def openFile():
        filename = filedialog.askopenfilename(initialdir=FileManger.__filePath, title="Select file",
                                              filetypes=(
                                              ("cnf files", "*.cnf"), ("txt files", "*.txt"), ("all files", "*.*")))

        if (filename == ''):
            return ''
        file = open(filename, 'r')

        klauzula = file.readline()

        file.close()
        return klauzula

    @staticmethod
    def saveFile(text):
        filename = filedialog.asksaveasfilename(initialdir=FileManger.__filePath, title="Select file", filetypes=(
        ("cnf files", "*.cnf"), ("txt files", "*.txt"), ("all files", "*.*")))

        if filename != '':
            file = open(filename, 'w')
            file.close()
