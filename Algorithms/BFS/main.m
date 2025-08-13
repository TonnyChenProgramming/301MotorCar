% MAIN BFS on a map
clc; clear; close all;

% Load map
m = map_convert('map_1.txt');
plotmap(m);

% Start & goal positions in [row, col] format
startPos = [14, 2]; % Example start (row 14, col 2)
goalPos  = [2, 17]; % Example goal (row 2, col 17)

% Run BFS
path = bfs_grid(m, startPos, goalPos);

% Display result
if ~isempty(path)
    disp('Path found by BFS:');
    disp(path);

    % Plot path on the map
    figure;
    plotmap(m, path);
else
    disp('No path found.');
end

%% BFS function for a grid map
function path = bfs_grid(map, startPos, goalPos)
    rows = size(map, 1);
    cols = size(map, 2);

    % Moves: up, down, left, right
    moves = [-1 0; 1 0; 0 -1; 0 1];

    visited = false(rows, cols);
    parent = zeros(rows, cols, 2); % store parent [row, col]
    queue = startPos;
    visited(startPos(1), startPos(2)) = true;

    found = false;

    while ~isempty(queue)
        current = queue(1, :);
        queue(1, :) = [];

        if isequal(current, goalPos)
            found = true;
            break;
        end

        for i = 1:4
            nr = current(1) + moves(i, 1);
            nc = current(2) + moves(i, 2);

            if nr >= 1 && nr <= rows && nc >= 1 && nc <= cols ...
                    && map(nr, nc) == 0 && ~visited(nr, nc)
                visited(nr, nc) = true;
                parent(nr, nc, :) = current;
                queue(end+1, :) = [nr, nc];
            end
        end
    end

    % Reconstruct path if found
    if found
        path = goalPos;
        p = goalPos;
        while ~isequal(p, startPos)
            p = squeeze(parent(p(1), p(2), :))';
            path = [p; path];
        end
    else
        path = [];
    end
end