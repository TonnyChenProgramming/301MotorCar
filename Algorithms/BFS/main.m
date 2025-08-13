% BFS Algorithm

%viewmap('map_1.txt',0); % shows walls
%viewmap('map_1.txt',1); % shows track

[m]=map_convert('map_1.txt');
plotmap(m);

% BFS Function
    function path = bfs(startNode, goalNode, graph)
    % Initialize the queue and visited set
    queue = {startNode};
    visited = containers.Map('KeyType', 'int32', 'ValueType', 'logical');
    visited(startNode) = true;
    
    % Initialize parent map to reconstruct the path
    parent = containers.Map('KeyType', 'int32', 'ValueType', 'int32');
    
    while ~isempty(queue)
        % Dequeue the first node
        currentNode = queue{1};
        queue(1) = []; % Remove the first element
        
        % Check if the goal is reached
        if currentNode == goalNode
            path = reconstruct_path(parent, startNode, goalNode);
            return;
        end
        
        % Explore neighbors
        neighbors = graph(currentNode);
        for i = 1:length(neighbors)
            neighbor = neighbors(i);
            if ~isKey(visited, neighbor) || ~visited(neighbor)
                visited(neighbor) = true;
                parent(neighbor) = currentNode;
                queue{end + 1} = neighbor; % Enqueue the neighbor
            end
        end
    end
    
% Run the BFS algorithm on the map and display the path
startNode = 1; % Start Node
goalNode = 10; %  Goal Node 

% Call the BFS function
% Store the path in a variable for workspace visibility
assignin('base', 'bfsPath', path);
path = bfs(startNode, goalNode, m);

% Display the path if it exists
if ~isempty(path)
    disp('Path found by BFS:');
    disp(path);
    
    % Plot the path on the map
    hold on;
    plot(path, 'ro-', 'LineWidth', 2); % Plot the path in red
    hold off;
else
    disp('No path found.');
end
    % If the goal is not reachable
    path = [];
end

function path = reconstruct_path(parent, startNode, goalNode)
    path = goalNode;
    while goalNode ~= startNode
        goalNode = parent(goalNode);
        path = [goalNode, path]; % Prepend to the path
    end
end
% Convert the BFS path to a format suitable for plotmap
if ~isempty(path)
    % Create a binary map for the path
    pathMap = zeros(size(m)); % Initialize a map of zeros
    for i = 1:length(path)
        pathMap(path(i)) = 1; % Mark the path on the map
    end
    
    % Display the path on the map using plotmap
    figure;
    plotmap(pathMap); % Use the plotmap function to visualize the path
    % Visualize the path on the original map
    hold on;
    plot(path, 'ro-', 'LineWidth', 2); % Plot the path in red
    hold off;
else
    disp('No path found to display on the map.');
end