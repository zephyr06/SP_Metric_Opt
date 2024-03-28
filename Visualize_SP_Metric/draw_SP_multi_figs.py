from draw_SP import *

if __name__=="__main__":
    # the path of the folder which generates the profiling data in ROS2 workspace
    data_folder_paths = {
        # "FIFO": os.path.join(
        # PROJECT_PATH, "Visualize_SP_Metric", "data"),
        "FIFO": os.path.join(PROJECT_PATH,"Visualize_SP_Metric/data/all_time_records_FIFO_Dyn/all_time_records"),
        "CFS": os.path.join(PROJECT_PATH,"Visualize_SP_Metric/data/all_time_records_CFS/all_time_records")}
    

    # used to provide period parameters
    task_set_config = os.path.join(
        PROJECT_PATH, "TaskData/test_robotics_v4.yaml")
    verify_task_set_config(task_set_config)
    app_name2period = get_app2period(task_set_config)
    tasks_name_list = ['TSP', 'RRT', 'SLAM', 'MPC']

    horizon_granularity = 10  # 10 seconds
    horizon = 2000  # 100 seconds
    discard_early_time = 20  # 20 seconds



    for method_name, data_folder_path in data_folder_paths.items():
        tasks_name_to_info = get_task_set_info(tasks_name_list, app_name2period, data_folder_path)
        sp_value_list = get_sp_value_list(tasks_name_list, tasks_name_to_info, horizon, horizon_granularity, discard_early_time)
        x_axis = [i for i in range(0, len(sp_value_list)*horizon_granularity, horizon_granularity)]
        plt.plot(x_axis, sp_value_list, label = method_name)

    plt.legend(data_folder_paths.keys())
    plt.xlabel("Time (s)")
    plt.ylabel("SP-Metric")
    plt.show()