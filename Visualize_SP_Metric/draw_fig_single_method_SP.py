from SP_draw_fig_utils import *
import os
if __name__ == "__main__":
    # the path of the folder which generates the profiling data in ROS2 workspace
    # data_folder_path = os.path.join(
    #     PROJECT_PATH, "Visualize_SP_Metric", "data")
    data_folder_path = os.path.join(
        PROJECT_PATH, "Visualize_SP_Metric", "data_for_test")
    # used to provide period parameters
    task_set_config = os.path.join(
        PROJECT_PATH, "TaskData/test_robotics_v4.yaml")
    verify_task_set_config(task_set_config)
    app_name2period = get_app2period(task_set_config)
    tasks_name_list = ['TSP', 'RRT', 'SLAM', 'MPC']

    horizon_granularity = 10  # 10 seconds
    horizon = 2000  # 100 seconds
    discard_early_time = 20  # 20 seconds

    tasks_name_to_info = get_task_set_info(tasks_name_list, app_name2period, data_folder_path)

    sp_value_list = get_sp_value_list(tasks_name_list, tasks_name_to_info, horizon, horizon_granularity, discard_early_time)
    # write_sp_value_to_file(sp_value_list, get_sp_value_file_name())
    draw_sp_value_plot(sp_value_list, horizon_granularity)
    

            
