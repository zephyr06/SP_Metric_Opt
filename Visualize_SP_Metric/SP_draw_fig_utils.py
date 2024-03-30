import matplotlib.pyplot as plt
import os
import yaml
import subprocess
from datetime import datetime

PROJECT_PATH = "/home/zephyr/Programming/SP_Metric_Opt"
# All time in seconds

def verify_task_set_config(path):
    if os.path.exists(path):
        # print("task_set_config exists.")
        return
    else:
        # print(path, " does not exist.")
        raise Exception(path + " does not exist.")


def get_app2period(task_set_config):
    app_name2period = {}
    with open(task_set_config, 'r') as file:
        yaml_data = yaml.safe_load(file)
        if 'tasks' in yaml_data:
            key ='tasks'
        elif 'Tasks' in yaml_data:
            key = 'Tasks'
        else:
            raise Exception("The key for tasks is not found in the yaml file.")
        for i in range(len(yaml_data[key])):
            data_entry = yaml_data[key][i]
            app_name2period[data_entry['name']] = data_entry['period']/float(1e3)
    return app_name2period


def get_publisher_file_path(folder_path, task_name):
    return os.path.join(folder_path, "publisher_" + task_name.lower() + ".txt")


def get_subscription_file_path(folder_path, task_name):
    return os.path.join(folder_path, task_name.lower() + "_subscriber.txt")


def get_index_to_data_map(file_path):
    verify_task_set_config(file_path)
    index_to_data = {}
    with open(file_path, 'r') as file:
        lines = file.readlines()
        offset = float(lines[0].split("::")[1][:-1])
        for i in range(1, len(lines)):
            line = lines[i]
            if len(line)==0:
                continue
            words = line.split(" ")
            last_word = words[-1]
            if last_word[-1]=='\n':
                last_word = last_word[:-1]
            index = int(last_word.split("::")[0])
            time = float(last_word.split("::")[1])
            index_to_data[index] = time
    return offset, index_to_data


class TaskInfo:
    def __init__(self, name, period):
        self.name = name
        self.period = period
        self.publisher_index2data = {}
        self.publisher_offset = 0
        self.subscriber_index2data = {}
        self.subscriber_offset = 0
        self.skipped_instance_response_time = 1e9
        self.response_time_index2data={}

    def load_publish_data(self, data_folder_path):
        file_path = get_publisher_file_path(data_folder_path, self.name)
        self.publisher_offset, self.publisher_index2data = get_index_to_data_map(
            file_path)

    def load_subscribe_data(self, data_folder_path):
        file_path = get_subscription_file_path(data_folder_path, self.name)
        self.subscriber_offset, self.subscriber_index2data = get_index_to_data_map(
            file_path)

    def get_response_time_index2data(self):
        if len(self.response_time_index2data) == 0:
            for index in self.publisher_index2data:
                if self.subscriber_index2data.get(index) is None:
                    self.response_time_index2data[index] = self.skipped_instance_response_time
                else:
                    self.response_time_index2data[index] = self.subscriber_index2data[index] + \
                        self.subscriber_offset - \
                        self.publisher_index2data[index] - self.publisher_offset
        return self.response_time_index2data
    
    def publisher_index2actual_time(self, index):
        return index * self.period + self.publisher_offset
    
    def get_response_time_within_range(self, start_time, end_time):
        response_time_index2data = self.get_response_time_index2data()
        response_time_within_range = []
        for index in self.publisher_index2data:
            release_time = self.publisher_index2actual_time(index)
            if release_time >= start_time and release_time < end_time:
                response_time_within_range.append(response_time_index2data[index]) 
        return response_time_within_range


def normalize_offsets(tasks_name_to_info):
    start_time = []
    for name, task_info in tasks_name_to_info.items():
        start_time.append(task_info.publisher_offset)
    min_start_time = min(start_time)

    for task in tasks_name_to_info:
        tasks_name_to_info[task].publisher_offset -= min_start_time
        tasks_name_to_info[task].subscriber_offset -= min_start_time
    return tasks_name_to_info

def get_task_set_info(tasks_name_list, app_name2period, data_folder_path):
    tasks_name_to_info = {}
    for task in tasks_name_list:
        task_info = TaskInfo(task, app_name2period[task])
        tasks_name_to_info[task] = task_info
        tasks_name_to_info[task].load_publish_data(data_folder_path)
        tasks_name_to_info[task].load_subscribe_data(data_folder_path)
    return normalize_offsets(tasks_name_to_info)

def get_response_time_file_name(task_name, start_time, end_time):
    file_name = task_name + "_response_time_" + str(start_time) + "_" + str(end_time) + ".txt"
    return os.path.join(PROJECT_PATH, "Visualize_SP_Metric", "data", "temp", file_name)

def get_SP_analyze_executable_file_path():
    path = os.path.join(PROJECT_PATH, "release", "tests", "AnalyzeSP_Metric")
    verify_task_set_config(path)
    return path

def get_sp_value(output_str):
    """output str format: SP-Metric: -2.40811"""
    return float(output_str.split(" ")[1])

def get_sp_value_file_name():
    current_time = datetime.now()
    time_string = current_time.strftime("%m-%d %H:%M:%S")  # Example format, adjust as needed
    return os.path.join(PROJECT_PATH, "Visualize_SP_Metric", "data", "temp", "sp_value"+time_string+ ".txt")

def write_sp_value_to_file(sp_value_list, file_name):
    with open(file_name, 'w') as file:
        for sp_value in sp_value_list:
            file.write(str(sp_value))
    file.close()

def draw_sp_value_plot(sp_value_list, horizon_granularity):
    x_axis = [i for i in range(0, len(sp_value_list)*horizon_granularity, horizon_granularity)]
    plt.plot(x_axis, sp_value_list)
    plt.xlabel("Time (s)")
    plt.ylabel("SP-Metric")
    plt.show()

def get_sp_value_list(tasks_name_list, tasks_name_to_info, horizon, horizon_granularity, discard_early_time):
    run_out_of_data=False
    sp_value_list=[]
    for start_time in range(discard_early_time, horizon, horizon_granularity):
        end_time = start_time + horizon_granularity
        command_in_terminal_to_analyze_taskset_sp = get_SP_analyze_executable_file_path()
        for task_name in tasks_name_list:
            response_time_within_range = tasks_name_to_info[task_name].get_response_time_within_range(
                start_time, end_time)
            if len(response_time_within_range) == 0:
                run_out_of_data=True
                break
            file_name = get_response_time_file_name(task_name, start_time, end_time)
            with open(file_name, 'w') as file:
                for response_time in response_time_within_range:
                    file.write(str(response_time*1000) + "\n")
            command_in_terminal_to_analyze_taskset_sp += " --" + task_name.lower() + "_path " + file_name
        if run_out_of_data:
            break
        # print(command_in_terminal_to_analyze_taskset_sp)
        result = subprocess.run(command_in_terminal_to_analyze_taskset_sp, shell=True, capture_output=True, text=True)
        # print(result.stdout)
        sp_value = get_sp_value(result.stdout)
        if sp_value > -4.5:
            a=1
        sp_value_list.append(sp_value)
    return sp_value_list
