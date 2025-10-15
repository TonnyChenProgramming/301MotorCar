function [retmap, retvisited, retsteps] = dijkstra(mapfile, startlocation, targetlocation)
% Dijkstra's Algorithm Pathfinding
% mapfile       : filename of the map (e.g. 'map_1.txt')
% startlocation : [row col] start
% targetlocation: [row col] goal
%
% retmap     : map matrix (0 = free, 1 = wall)
% retvisited : 0 = visited, 1 = not visited
% retsteps   : ordered path [row col] from start to goal

    % Load map from file
    retmap = map_convert(mapfile);
    [ROWS, COLS] = size(retmap);

    % Initialise visited (1 = not visited, 0 = visited)
    visited = ones(ROWS, COLS);

    % Distance from start to each cell (infinite to start with)
    dist = inf(ROWS, COLS);
    dist(startlocation(1), startlocation(2)) = 0;

    % Parent for path reconstruction
    parent = cell(ROWS, COLS);

    % Open list: [row col distance]
    openList = [startlocation 0];

    found = false;

    while ~isempty(openList)
        % Pick node with smallest distance
        [~, idx] = min(openList(:,3));
        current = openList(idx, 1:2);
        openList(idx,:) = []; % remove from list

        r = current(1);
        c = current(2);

        % If target found, stop
        if isequal(current, targetlocation)
            found = true;
            break;
        end

        % Skip if already visited
        if visited(r,c) == 0
            continue;
        end

        % Mark visited
        visited(r,c) = 0;

        % Check 4 neighbours (up, right, down, left)
        neighbours = [r-1, c; r, c+1; r+1, c; r, c-1];

        for k = 1:4
            nr = neighbours(k,1);
            nc = neighbours(k,2);

            % Skip if out of bounds
            if nr < 1 || nr > ROWS || nc < 1 || nc > COLS
                continue;
            end

            % Skip if wall
            if retmap(nr,nc) == 1
                continue;
            end

            % New distance
            newDist = dist(r,c) + 1; % cost = 1 for each step

            if newDist < dist(nr,nc)
                dist(nr,nc) = newDist;
                parent{nr,nc} = [r c];
                openList = [openList; nr nc newDist];
            end
        end
    end

    % Reconstruct path
    retsteps = [];
    if found
        path = targetlocation;
        while ~isequal(path(1,:), startlocation)
            path = [parent{path(1,1), path(1,2)}; path];
        end
        retsteps = path;
    end

    % Output visited in correct format
    retvisited = visited;
end