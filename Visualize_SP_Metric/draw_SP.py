import matplotlib.pyplot as plt
import os
import yaml

PROJECT_PATH = "/home/zephyr/Programming/SP_Metric_Opt"
# All time in seconds

def verify_task_set_config(path):
    if os.path.exists(path):
        # print("task_set_config exists.")
        return
    else:
        print(path, " does not exist.")


def get_app2period(task_set_config):
    app_name2period = {}
    with open(task_set_config, 'r') as file:
        yaml_data = yaml.safe_load(file)
        for i in range(len(yaml_data['tasks'])):
            data_entry = yaml_data['tasks'][i]
            app_name2period[data_entry['name']] = data_entry['period']/1e3
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
            words = line.split(" ")
            last_word = words[-1][:-1]
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
        return self.publisher_index2data[index] * self.period + self.publisher_offset
    
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



if __name__ == "__main__":
    data_folder_path = os.path.join(
        PROJECT_PATH, "Visualize_SP_Metric", "data")
    task_set_config = os.path.join(
        PROJECT_PATH, "TaskData/test_robotics_v4.yaml")
    verify_task_set_config(task_set_config)
    app_name2period = get_app2period(task_set_config)
    tasks_name_list = ['TSP', 'RRT', 'SLAM', 'MPC']

    horizon_granularity = 10  # 10 seconds
    horizon = 100  # 100 seconds
    discard_early_time = 20  # 20 seconds

    # offset, index_to_data = get_index_to_data_map(get_publisher_file_path(data_folder_path, 'MPC'))
    offset, index_to_data = get_index_to_data_map(
        get_subscription_file_path(data_folder_path, 'SLAM'))
    tasks_name_to_info = get_task_set_info(tasks_name_list, app_name2period, data_folder_path)

    for task in tasks_name_list:
        print(task, tasks_name_to_info[task].publisher_offset,
              tasks_name_to_info[task].subscriber_offset)

    for start_time in range(discard_early_time, horizon, horizon_granularity):
        end_time = start_time + horizon_granularity
        # for task_name in tasks_name_list:
