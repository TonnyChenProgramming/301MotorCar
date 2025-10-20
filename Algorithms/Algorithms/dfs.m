%% This is a shell that you will have to follow strictly. 
% You will use the plotmap() and viewmap() to display the outcome of your algorithm.

% Load sample_data_map_8, three variables will be created in your workspace. These were created as a 
% result of [m,v,s]=dfs('map_8.txt',[14,1],[1,18]);
% The solution can be viewed using 
% plotmap(m,s) 

% write your own function for the DFS algorithm.
function [retmap, retvisited, retsteps] = dfs(mapfile, startlocation, targetlocation)

    % Load the map from file
    retmap = map_convert(mapfile);
    [ROWS, COLS] = size(retmap);

    % Initialise visited (start with all 1 = not visited)
    visited = ones(ROWS, COLS);

    % Stack for DFS
    stack = {startlocation};
    parent = cell(ROWS, COLS); % store parent for each cell

    found = false;

    while ~isempty(stack)
        % Take the last element (DFS)
        current = stack{end};
        stack(end) = [];

        r = current(1);
        c = current(2);

        % Skip if already visited
        if visited(r,c) == 0
            continue;
        end

        % Mark visited (0 means visited)
        visited(r,c) = 0;

        % Check if target found
        if isequal(current, targetlocation)
            found = true;
            break;
        end

        % Neighbours: up, right, down, left
        neighbours = [r-1, c; r, c+1; r+1, c; r, c-1];

        for k = 1:4
            nr = neighbours(k,1);
            nc = neighbours(k,2);

            % Check in bounds
            if nr < 1 || nr > ROWS || nc < 1 || nc > COLS
                continue;
            end

            % Skip walls and visited
            if retmap(nr,nc) == 1 || visited(nr,nc) == 0
                continue;
            end

            % Store parent
            if isempty(parent{nr,nc})
                parent{nr,nc} = [r c];
            end

            % Push onto stack
            stack{end+1} = [nr nc];
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

    % Output visited in required format
    retvisited = visited;
end



function placestep(position,i)
% This function will plot a insert yellow rectangle and also print a number in this rectangle. Use with plotmap/viewmap. 
position = [16-position(1) position(2)];
position=[position(2)+0.1 position(1)+0.1];
rectangle('Position',[position,0.8,0.8],'FaceColor','y');
c=sprintf('%d',i);
text(position(1)+0.2,position(2)+0.2,c,'FontSize',10);
end
