import pytest
import numpy as np
from SP_draw_fig_utils import *

def test_read_period():
    task_set_config = os.path.join(
        PROJECT_PATH, "TaskData/test_robotics_v4.yaml")
    verify_task_set_config(task_set_config)
    app_name2period = get_app2period(task_set_config)
    # assert app_name2period['TSP'] == 1.0
    assert app_name2period['MPC'] == 0.5
    assert app_name2period['RRT'] == 1.0
    assert app_name2period['SLAM'] == 1.0

def test_load_publish_data():
    task = TaskInfo('MPC', 1.0)
    data_folder_path = os.path.join(
        PROJECT_PATH, "Visualize_SP_Metric", "data_for_test")
    task.load_publish_data(data_folder_path)
    assert task.publisher_offset == pytest.approx(1711401393.7642669678)
    assert task.publisher_index2data[0] == pytest.approx(0.61328400)

    task.load_subscribe_data(data_folder_path)
    assert task.subscriber_offset == pytest.approx(1711401393.764341116)
    assert task.subscriber_index2data[0] == pytest.approx(0.750322999)

def test_get_response_time():
    task = TaskInfo('MPC', 1.0)
    data_folder_path = os.path.join(
        PROJECT_PATH, "Visualize_SP_Metric", "data_for_test")
    task.load_publish_data(data_folder_path)
    task.load_subscribe_data(data_folder_path)
    response_time_index2data = task.get_response_time_index2data()
    assert response_time_index2data[0] == pytest.approx(0.13703999999999998, abs=1e-3)
    assert response_time_index2data[100] == pytest.approx(50.7210350 - 50.614, abs=1e-3)

def test_get_invalid_response_time():
    task = TaskInfo('RRT', 1.0)
    data_folder_path = os.path.join(
        PROJECT_PATH, "Visualize_SP_Metric", "data_for_test")
    task.load_publish_data(data_folder_path)
    task.load_subscribe_data(data_folder_path)
    response_time_index2data = task.get_response_time_index2data()
    assert response_time_index2data[345] == pytest.approx(1e9, abs=1e-3)


def test_publisher_index2actual_time():
    task = TaskInfo('MPC', 1.0)
    data_folder_path = os.path.join(
        PROJECT_PATH, "Visualize_SP_Metric", "data_for_test")
    task.load_publish_data(data_folder_path)
    assert task.publisher_index2actual_time(0) == pytest.approx(1711401393.7642669678)
    assert task.publisher_index2actual_time(100) == pytest.approx(1711401393.7642669678 + 100 * 1.0)

def test_normalize_offsets():
    tasks_name_list = ['TSP', 'RRT', 'SLAM', 'MPC']

    task_set_config = os.path.join(
        PROJECT_PATH, "TaskData/test_robotics_v4.yaml")
    app_name2period = get_app2period(task_set_config)

    data_folder_path = os.path.join(
        PROJECT_PATH, "Visualize_SP_Metric", "data_for_test")
    
    tasks_name_to_info = get_task_set_info(tasks_name_list, app_name2period, data_folder_path)
    assert tasks_name_to_info['SLAM'].publisher_offset == pytest.approx(0.0, abs=1e-4)
    assert tasks_name_to_info['SLAM'].subscriber_offset == pytest.approx(12, abs=1e0)
    assert tasks_name_to_info['MPC'].publisher_offset == pytest.approx(0.0, abs=1e-2)
    assert tasks_name_to_info['MPC'].subscriber_offset == pytest.approx(0.0, abs=1e-2)

def test_get_sp_value_list():
    tasks_name_list = ['TSP', 'RRT', 'SLAM', 'MPC']

    task_set_config = os.path.join(
        PROJECT_PATH, "Visualize_SP_Metric/data_for_test2/task_characteristics.yaml")
    app_name2period = get_app2period(task_set_config)

    data_folder_path = os.path.join(
        PROJECT_PATH, "Visualize_SP_Metric", "data_for_test2")
    
    tasks_name_to_info = get_task_set_info(tasks_name_list, app_name2period, data_folder_path)
    sp_value_list = get_sp_value_list(tasks_name_list, tasks_name_to_info, 1000, 10, 0)
    # only one value; chance of four tasks to miss DDL:
    # TSP: 0.0, RRT: 1.0, SLAM: 1.0, MPC: 0.2
    exp_sp = 2*-0.01*np.exp(10*0.5) + np.log(1.5)*2
    assert sp_value_list == pytest.approx([exp_sp], abs=1e-2)
    