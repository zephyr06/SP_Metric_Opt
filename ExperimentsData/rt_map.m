% Assume 'time_big' and 'data_big' are your larger set of timestamped data
% Assume 'time_small' and 'data_small' are your smaller set of timestamped data
% Both 'time_big' and 'time_small' should be sorted in ascending order.
clc;
clear all;
close all;
num = xlsread('SLAM_time.xlsx'); 

timestamp1 =num(:,6);
data1=num(:,4);
timestamp2=num(:,9);
data2x=num(:,10);
data2y=num(:,11);

% % Initialize variables to hold the downsampled data
% time_downsampled = zeros(size(time_small));
% data_downsampled = zeros(size(time_small));

% % Loop over each timestamp in the smaller set
% for i = 1:length(time_small)
%     
%     % Find the index of the closest timestamp in the larger set
%     [~, closest_idx] = min(abs(time_big - time_small(i)));
%     
%     % Add this timestamp and corresponding data value to the downsampled sets
%     time_downsampled(i) = time_big(closest_idx);
%     data_downsampled(i) = data_bigx(closest_idx);
%     data_downsampled(i) = data_bigy(closest_idx);
%     
% end
% 
% % Now, 'time_downsampled' and 'data_downsampled' should be aligned with 'time_small' and 'data_small'.

% Initialize variables to store matched data
matched_data1 = [];
matched_data2x = [];
matched_data2y = [];

% Specify aggregation window (this could be mean, median, sum, etc.)
agg_window = 200;  % considering 200 as the window for this example

% Loop through the smaller dataset to find matching points in the larger dataset
for i = 1:length(timestamp1)
    lower_bound = timestamp1(i) - agg_window / 2;
    upper_bound = timestamp1(i) + agg_window / 2;
    
    % Indices of data points in the larger dataset within the aggregation window
    in_window_indices = find(timestamp2 >= lower_bound & timestamp2 <= upper_bound);
    
    % Aggregate the data (mean in this example)
    if ~isempty(in_window_indices)
        aggregated_datax = mean(data2x(in_window_indices));
        aggregated_datay = mean(data2y(in_window_indices));
        
        % Store the matched data
        matched_data1 = [matched_data1, data1(i)];
        matched_data2x = [matched_data2x, aggregated_datax];
        matched_data2y = [matched_data2y, aggregated_datay];
    end
end

x_grid = linspace(min(matched_data2x), max(matched_data2x), 30);
y_grid = linspace(min(matched_data2y), max(matched_data2y), 30);
[X_Grid, Y_Grid] = meshgrid(x_grid, y_grid);

matched_data2x=transpose(matched_data2x);
matched_data2y=transpose(matched_data2y);
matched_data1=transpose(matched_data1);
% Prepare data for GPR
X = [matched_data2x, matched_data2y]; % Input data
y = matched_data1; % Output data

% Train a Gaussian Process Regression model
gprMdl = fitrgp(X, y, 'KernelFunction', 'squaredexponential');

% Predict over the entire grid
[ypred, ystd] = predict(gprMdl, [X_Grid(:), Y_Grid(:)]);
ypred_grid = reshape(ypred, size(X_Grid));
ystd_grid = reshape(ystd, size(X_Grid));

% Visualize the mean predictions
figure;
surf(X_Grid, Y_Grid, ypred_grid);
title('GPR Mean Predictions');
xlabel('X'); ylabel('Y'); zlabel('Z');
colorbar;

% Visualize the variances (standard deviations)
figure;
surf(X_Grid, Y_Grid, ystd_grid);
title('GPR Variances (Standard Deviations)');
xlabel('X'); ylabel('Y'); zlabel('STD');
colorbar;

Data_Grid = griddata(matched_data2x, matched_data2y, matched_data1, X_Grid, Y_Grid, 'cubic');  % 'linear' is the interpolation method, you can also use 'nearest', 'cubic', etc.

figure;
colormap('hot');
imagesc(x_grid, y_grid, Data_Grid);
colorbar;
xlabel('X-axis');
ylabel('Y-axis');
title('Interpolated Heatmap');

figure;
plot3(matched_data2x,matched_data2y,matched_data1)
