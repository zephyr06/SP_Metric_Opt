
from SP_draw_fig_utils import *
import numpy as np
from collections import Counter
import seaborn as sns

# tasks_name_list = ['TSP', 'RRT', 'SLAM', 'MPC']

# task_set_config = os.path.join(
#     PROJECT_PATH, "Visualize_SP_Metric/data_for_test2/task_characteristics.yaml")
# app_name2period = get_app2period(task_set_config)

# data_folder_path = os.path.join(
#     PROJECT_PATH, "Visualize_SP_Metric", "data_for_test2")

# tasks_name_to_info = get_task_set_info(tasks_name_list, app_name2period, data_folder_path)
# sp_value_list = get_sp_value_list(tasks_name_list, tasks_name_to_info, 1000, 10, 0)
# print(sp_value_list)
def datafromtxtfile(file_path):
    with open(file_path, 'r') as file:
        data = file.readlines()
        data = [[float(x)] for x in data]
        data = np.array(data)
        data = [x for x in data if x < 1e8]
        data.sort()
        return data

def calculate_pmf(data):
    data_length = len(data)
    frequency_counts = Counter(data)
    pmf = {value: count / data_length for value, count in frequency_counts.items()}
    return pmf


# def visualize_txt_data_in_file(file_path):
#     with open(file_path, 'r') as file:
#         data = file.readlines()
#         data = [[float(x)] for x in data]
#         data = np.array(data)
#         data = [x for x in data if x<1e8]
#         # sns.displot(data, kde=True)
#         # plt.title("Distribution Plot")
#         # plt.xlabel("Values")
#         # plt.ylabel("Density")
#         # plt.scatter(range(len(data)), data)
#         # plt.hist(data, bins=10)
#         data.sort()
#         print(data)
#         plt.show()

path = os.path.join(PROJECT_PATH, 'Visualize_SP_Metric/data/temp/SLAM_response_time_120_1120.txt')
# visualize_txt_data_in_file(path)
data = datafromtxtfile(path)

pmf = calculate_pmf(data)
print("Probability Mass Function (PMF):", pmf)
