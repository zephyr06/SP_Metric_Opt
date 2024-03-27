import matplotlib.pyplot as plt
import os
import yaml



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
        for i in range(len(yaml_data)):
            data_entry = yaml_data['tasks'][i]
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
        self.publisher_index2_data = {}
        self.publisher_offset=0
        self.subscriber_index2_data = {}
        self.subscriber_offset=0
        
    def load_publish_data(self, data_folder_path):
        file_path = get_publisher_file_path(data_folder_path, self.name)
        self.publisher_offset, self.publisher_index2_data = get_index_to_data_map(file_path)

    def load_subscribe_data(self, data_folder_path):
        file_path = get_subscription_file_path(data_folder_path, self.name)
        self.subscriber_offset, self.subscriber_index2_data = get_index_to_data_map(file_path)


if __name__ == "__main__":
    PROJECT_PATH = "/home/zephyr/Programming/SP_Metric_Opt"
    data_folder_path = os.path.join(PROJECT_PATH, "Visualize_SP_Metric","data")
    task_set_config = os.path.join(PROJECT_PATH, "TaskData/test_robotics_v4.yaml") 
    verify_task_set_config(task_set_config)
    app_name2period = get_app2period(task_set_config)
    tasks_name_list =['TSP','RRT','SLAM','MPC']

    horizon_granularity = 10000 # 10 seconds
    horizon = 1e5 # 100 seconds
    discard_early_time = 2e4 # 20 seconds

    # offset, index_to_data = get_index_to_data_map(get_publisher_file_path(data_folder_path, 'MPC'))
    offset, index_to_data = get_index_to_data_map(get_subscription_file_path(data_folder_path, 'SLAM'))
    tasks_name_to_info={}
    for task in tasks_name_list:
        tasks_name_to_info[task] = TaskInfo(task, app_name2period[task])
        tasks_name_to_info[task].load_publish_data(data_folder_path)
        tasks_name_to_info[task].load_subscribe_data(data_folder_path)
    
    print(offset)
    print(index_to_data)








