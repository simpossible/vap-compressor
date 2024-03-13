import os


class TaskVM(object):

    def __init__(self, path):
        self.path = path
        self.vap_file_list = []

    @staticmethod
    def task_from_path(path):
        return TaskVM(path)


    # def load_files(self):
    #     if self.pa
