function [retmap, retvisited, retsteps, all] = dijkstra(mapfile, startlocation, targetlocation)
%dijkstra is not the best because it calculates one to all paths, innefficient.
    % Read map
    retmap = map_convert(mapfile);
    [ROWS, COLS] = size(retmap);

    % 1 means not visited, 0 means visited
    visited = ones(ROWS, COLS);

    % Distance from start
    dist = inf(ROWS, COLS);
    dist(startlocation(1), startlocation(2)) = 0;

    % For tracing the path
    parent = cell(ROWS, COLS);

    % List of cells to check next
    openList = [startlocation 0];

    % Setup all steps
    all = [];

    found = false;

    while ~isempty(openList)
    % Pick cell with smallest distance
        [~, idx] = min(openList(:,3));
        current = openList(idx, 1:2);
    openList(idx,:) = [];

        r = current(1);
        c = current(2);

    % Stop if at goal
        if isequal(current, targetlocation)
            found = true;
            break;
        end

    % Skip visited cells
        if visited(r,c) == 0
            continue;
        end

    % Mark as visited
        visited(r,c) = 0;

    %update all steps
    all = [all; r c];

    % Check up, right, down, left
        neighbours = [r-1, c; r, c+1; r+1, c; r, c-1];

        for k = 1:4
            nr = neighbours(k,1);
            nc = neighbours(k,2);

            % Out of bounds
            if nr < 1 || nr > ROWS || nc < 1 || nc > COLS
                continue;
            end

            % Wall
            if retmap(nr,nc) == 1
                continue;
            end

            % Distance to neighbour
            newDist = dist(r,c) + 1;

            if newDist < dist(nr,nc)
                dist(nr,nc) = newDist;
                parent{nr,nc} = [r c];
                openList = [openList; nr nc newDist];
            end
        end
    end

    % Build path
    retsteps = [];
    if found
        path = targetlocation;
        while ~isequal(path(1,:), startlocation)
            path = [parent{path(1,1), path(1,2)}; path];
        end
        retsteps = path;
    end

    % Return visited
    retvisited = visited;
end