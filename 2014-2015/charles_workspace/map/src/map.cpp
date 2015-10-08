#include <iostream>
#include <iomanip> //for setw()
#include <sstream>
#include <cstdlib> //for abs()

//ROS
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "sensor_msgs/NavSatFix.h" //for GPS data callback function

//Lat & Long conversion to UTM
#include "conversions.h"

using namespace gps_common;

//for use in reading waypoints from file
struct coords
{
    double latitude;
    double longitude;
};

//this means boxes are 1/4 m^2
const int BOX_WIDTH = 4;

//max integer, used for marking obstacles
const int OBSTACLE = 32767;

//globals to hold the gpsstream values
double gps_lat, gps_long;


bool createMap(int row, int col, int** &map);
void deleteMap(int row, int** &map);
void getRowAndCol(int &row, int &col, int northing_start, int northing_goal,
                  int easting_start, int easting_goal);
double getXRange(double start, double end);
double getYRange(double start, double end);
void mapGpsCallback(const sensor_msgs::NavSatFix::ConstPtr& msg);
void markGoal(double rx, double ry, double gx, double gy, int** &map, int row,
              int col);
void markObstacle(double distance, double angle_from_front);
void markPath(int** &map, int x, int y);
void populateMap(int row, int col, int** &map, int pos[]);
void printMap(int row, int col, int** &map);
void readWaypointFile(std::ifstream &fin, std::queue<coords> &q);
void wavefrontFill(int row, int col, int** &map);
void wavefrontStep(int** &map, int pos_r, int pos_c, int &next_r, int &next_c);

int main(int argc, char* argv[])
{
    double threshold = 0.00001;
    double r_start_lat = 0.0;
    double r_start_long = 0.0;
    double goal_lat = 0.0;
    double goal_long = 0.0;
    int row = 0;
    int col = 0;
    int** map = NULL;
    std::string zone; //UTM zone
    double northing_start, easting_start, northing_goal, easting_goal;
    std::stringstream ss;
    //pos stores current x & y, next step x & y respectively
    int pos[4] = {0};
    //flag to tell if ready for next step in the path
    bool flag = false;
    std_msgs::String message;
    std::queue<coords> waypoints;
    coords next_waypoint;
    std::ifstream infile;


    /**
    * From: ROSTutorialsWritingPublisherSubscriber(c++)
    * At: http://wiki.ros.org/ROS/Tutorials/
    *            WritingPublisherSubscriber%28c%2B%2B%29
    *
    * The ros::init() function needs to see argc and argv so that it can perform
    * any ROS arguments and name remapping that were provided at the command
    * line. For programmatic remappings you can use a different version of
    * init() which takes remappings directly, but for most command-line
    * programs, passing argc and argv is the easiest way to do it.  The third
    * argument to init() is the name of the node.
    *
    * You must call one of the versions of ros::init() before using any other
    * part of the ROS system.
    */
    ros::init(argc, argv, "talker");

    /**
    * From: ROSTutorialsWritingPublisherSubscriber(c++)
    * At: http://wiki.ros.org/ROS/Tutorials/
    *            WritingPublisherSubscriber%28c%2B%2B%29
    *
    * NodeHandle is the main access point to communications with the ROS system.
    * The first NodeHandle constructed will fully initialize this node, and the
    * last NodeHandle destructed will close down the node.
    */
    ros::NodeHandle n;

    /**
    * From: ROSTutorialsWritingPublisherSubscriber(c++)
    * At: http://wiki.ros.org/ROS/Tutorials/
    *            WritingPublisherSubscriber%28c%2B%2B%29
    *
    * The advertise() function is how you tell ROS that you want to
    * publish on a given topic name. This invokes a call to the ROS
    * master node, which keeps a registry of who is publishing and who
    * is subscribing. After this advertise() call is made, the master
    * node will notify anyone who is trying to subscribe to this topic name,
    * and they will in turn negotiate a peer-to-peer connection with this
    * node.  advertise() returns a Publisher object which allows you to
    * publish messages on that topic through a call to publish().  Once
    * all copies of the returned Publisher object are destroyed, the topic
    * will be automatically unadvertised.
    *
    * The second parameter to advertise() is the size of the message queue
    * used for publishing messages.  If messages are published more quickly
    * than we can send them, the number here specifies how many messages to
    * buffer up before throwing some away.
    */
    ros::Publisher path_step_pub = n.advertise<std_msgs::String>("path_step",
                    1000);

    //subscriber for GPS data
    ros::Subscriber sub = n.subscribe("GPSStream", 1000, mapGpsCallback);

    //read in waypoint file and store waypoints in queue
    readWaypointFile(infile, waypoints);

    if(waypoints.empty() == false)
    {
        //get first waypoint from list
        next_waypoint = waypoints.front();
        //remove waypoint from list
        waypoints.pop();
    }

    //lattitude = x, longitude = y
    r_start_lat = atof(argv[1]);//should pull this from UAV's GPS
    r_start_long = atof(argv[2]);//should pull this from UAV's GPS
    goal_lat = atof(argv[3]);//read this from the waypoint list
    goal_long = atof(argv[4]);//read this from the waypoint list

    //convert lattitude and longitude into UTM coordinates
    LLtoUTM(r_start_lat, r_start_long, northing_start, easting_start, zone);
    LLtoUTM(goal_lat, goal_long, northing_goal, easting_goal, zone);


    //get row and column size
    getRowAndCol(row, col, northing_start, northing_goal, easting_start,
                 easting_goal);

    //allocate map
    createMap(row, col, map);

    //mark the start position and fill rest of map with zeros
    populateMap(row, col, map, pos);

    //marks the goal location with respect to the start location
    markGoal(northing_start, easting_start, northing_goal, easting_goal, map,
             row, col);

    //perform the wavefront algorithm using the 4 neighbors method
    wavefrontFill(row, col, map);


    //find first step in the path
    wavefrontStep(map, pos[0], pos[1], pos[2], pos[3]);

    //continues to loop until ros::shudown is triggered
    while(ros::ok())
    {
        if(abs(r_start_lat - goal_lat) < threshold &&
           abs(r_start_long - goal_long) < threshold)
        {
            //delete current map
            deleteMap(row, map);

            //if we've reached our current goal read the next waypoint from
            //the waypoint file and rebuild map

            //make previous goal lat & long the new start lat & long
            r_start_lat = goal_lat;
            r_start_long = goal_long;

            if(waypoints.empty() == false)
            {
                //get next waypoint lat & long from list
                next_waypoint = waypoints.front();
                waypoints.pop();
                goal_lat = next_waypoint.latitude;
                goal_long = next_waypoint.longitude;
            }


            //convert lat & long to UTM coords
            LLtoUTM(r_start_lat, r_start_long, northing_start, easting_start,
                    zone);
            LLtoUTM(goal_lat, goal_long, northing_goal, easting_goal, zone);

            //get new rows and columns for new map
            getRowAndCol(row, col, northing_start, northing_goal, easting_start,
                         easting_goal);

            //allocate map
            createMap(row, col, map);

            //mark the start position and fill rest of map with zeros
            populateMap(row, col, map, pos);

            //marks the goal location with respect to the start location
            markGoal(northing_start, easting_start, northing_goal, easting_goal,
                     map, row, col);

            //perform the wavefront algorithm using the 4 neighbors method
            wavefrontFill(row, col, map);


            //find first step in the path
            wavefrontStep(map, pos[0], pos[1], pos[2], pos[3]);

        }
        if(flag)
        {
            //mark previous step in path
            markPath(map, pos[0], pos[1]);
            //move the next step values into the current position values
            pos[0] = pos[2];
            pos[1] = pos[3];
            //find next step in path
            wavefrontStep(map, pos[0], pos[1], pos[2], pos[3]);
        }

        //clear stringstream variable before setting new value
        ss.str(std::string());
        //store the formatted string to be published
        ss << "next_step: " << pos[2] << ", " << pos[3];

        //set the message data to the output string
        message.data = ss.str();
        //publish formatted string
        path_step_pub.publish(message);

        ros::spinOnce();
    }

    deleteMap(row, map);

    return 0;
}

/******************************************************************************
getXRange: Determines the distance between to points, in the x range of
             the coordinate system.

Author: Charles Parsons
******************************************************************************/
double getXRange(double start, double end)
{
    return abs(end - start);
}

/******************************************************************************
getYRange: Determines the distance between to points, in the y range of
             the coordinate system.

Author: Charles Parsons
******************************************************************************/
double getYRange(double start, double end)
{
    return abs(end - start);
}

/******************************************************************************
createMap: Allocates a temporary map for the robot to use to the next
           waypoint. The robot's location is placed at the center of the map.

Author: Charles Parsons
******************************************************************************/
bool createMap(int row, int col, int** &map)
{
    int i = 0;
    map = new(std::nothrow) int*[row];
    if(map == NULL)
    {
        return false;
    }
    for(i = 0; i < row; ++i)
    {
        map[i] = new(std::nothrow) int[col];
        if(map[i] == NULL)
        {
            return false;
        }
    }
    return true;
}

/******************************************************************************
deleteMap: Deallocates the dynamic memory used by the temporary map.

Author: Charles Parsons
******************************************************************************/
void deleteMap(int row, int** &map)
{
    int i = 0;
    for(i = 0; i < row; ++i)
    {
        delete [] map[i];
    }
    delete [] map;
    map = NULL;
}

/******************************************************************************
populateMape: Marks the robot's location in the center. The rest of the map
              is marked with 0's.

Author: Charles Parsons
******************************************************************************/
void populateMap(int row, int col, int** &map, int pos[])
{
    int i = 0;
    int j = 0;

    for(i = 0; i < row; ++i)
    {
        for(j = 0; j < col; ++j)
        {
            //mark robot position in map
            if(i == (row / 2) && j == (col / 2))
            {
                map[i][j] = -2;
                pos[0] = i;
                pos[1] = j;
            }
            else
                map[i][j] = 0;
        }
    }
}

/******************************************************************************
printMap: Outputs the map to the console.

Author: Charles Parsons
******************************************************************************/
void printMap(int row, int col, int** &map)
{
    int i = 0;
    int j = 0;

    for(i = 0; i < row; ++i)
    {
        for(j = 0; j < col; ++j)
        {
            std::cout << std::setw(8) << map[i][j];
        }
        std::cout << std::endl;
    }
}

/******************************************************************************
markGoal: Marks the goal location on the map with respect to the robot.

Author: Charles Parsons
******************************************************************************/
void markGoal(double rx, double ry, double gx, double gy, int** &map, int row, int col)
{
    int goal_x_index = 0;
    int goal_y_index = 0;

    //to correct indexing problems
    gx -= rx;
    gy -= ry;
    rx = 0.0;
    ry = 0.0;

    //absolute value function is used to maintain values when trying to
    //calculate distance between a negative and positive numbers
    if(rx < gx)
        goal_x_index = (col / 2) + (abs(gx) * BOX_WIDTH) + (abs(rx) *
                        BOX_WIDTH);
    else
        goal_x_index = (col / 2) - (abs(gx) * BOX_WIDTH) - (abs(rx) *
                        BOX_WIDTH);
    if(ry < gy)
        goal_y_index = (row / 2) - (abs(gy) * BOX_WIDTH) - (abs(ry) *
                        BOX_WIDTH);
    else
        goal_y_index = (row / 2) + (abs(gy) * BOX_WIDTH) + (abs(rx) *
                        BOX_WIDTH);
    map[goal_y_index][goal_x_index] = -1;

}

/******************************************************************************
wavefront_fill: Performs the wavefront algorithm to fill in values that
represent steps from the goal. It starts out by marking the row and column that
the goal is in. It then iterates through the remaining rows using the value from
the goal's column as its seed value. On the row that the starting location is on
it stops evaluating those values once it reaches the start location.

Author: Charles Parsons
******************************************************************************/
void wavefrontFill(int row, int col, int** &map)
{
    int i, j;
    bool goal_found = false;
    int goal_index[2] = {0}; //index 0 = i, index 1 = j
    bool start_found = false;
    int start_location[2] = {0};

    //find the indexes of the goal
    for(i = 0; i < row && !goal_found; ++i)
    {
        for(j = 0; j < col && !goal_found; ++j)
        {
            if(map[i][j] == -1)
            {
                goal_found = true;
                goal_index[0] = i;
                goal_index[1] = j;
            }
        }
    }

    //mark the row and column of the goal with their values
    for(j = goal_index[1] - 1; j >= 0; --j)
    {
        if(map[goal_index[0]][j + 1] == -1)
        {
            map[goal_index[0]][j] = 1;
        }
        else if(map[goal_index[0]][j] == 0)
        {
            map[goal_index[0]][j] = map[goal_index[0]][j + 1] + 1;
        }
    }
    for(j = goal_index[1] + 1; j < col; ++j)
    {
        if(map[goal_index[0]][j - 1] == -1)
        {
            map[goal_index[0]][j] = 1;
        }
        else if(map[goal_index[0]][j] == 0)
        {
            map[goal_index[0]][j] = map[goal_index[0]][j - 1] + 1;
        }
    }
    for(i = goal_index[0] - 1; i >= 0; --i)
    {
        if(map[i + 1][goal_index[1]] == -1)
        {
            map[i][goal_index[1]] = 1;
        }
        else if(map[i][goal_index[1]] == 0)
        {
            map[i][goal_index[1]] = map[i + 1][goal_index[1]] + 1;
        }
    }
    for(i = goal_index[0] + 1; i < row; ++i)
    {
        if(map[i - 1][goal_index[1]] == -1)
        {
            map[i][goal_index[1]] = 1;
        }
        else if(map[i][goal_index[1]] == 0)
        {
            map[i][goal_index[1]] = map[i - 1][goal_index[1]] + 1;
        }
    }
    //mark the rest of the rows and columns using the already marked ones as
    //seed values

    for(i = 0; i < row; ++i)
    {
        start_found = false;
        //skip the row that the goal is on
        if(i != goal_index[0])
        {
            for(j = goal_index[1] - 1; j >= 0 && !start_found; --j)
            {
                if(map[i][j] == 0)
                {
                    map[i][j] = map[i][j + 1] + 1;
                }
                else if(map[i][j] == -2)
                {
                    start_found = true;
                    start_location[0] = i;
                    start_location[1] = j;
                }
            }
            start_found = false;
            for(j = goal_index[1] + 1; j < col && !start_found; ++j)
            {
                if(map[i][j] == 0)
                {
                    map[i][j] = map[i][j - 1] + 1;
                }
                else if(map[i][j] == -2)
                {
                    start_found = true;
                    start_location[0] = i;
                    start_location[1] = j;
                }
            }
        }
    }
    for(j = start_location[1] + 1; j < col; ++j)
    {
        if(map[start_location[0]][j] == 0)
        {
            if(map[start_location[0] - 1][j] < map[start_location[0] + 1][j])
            {
                map[start_location[0]][j] = map[start_location[0] - 1][j] + 1;
            }
            else
            {
                map[start_location[0]][j] = map[start_location[0] + 1][j] + 1;
            }
        }
    }
    for(j = start_location[1] - 1; j >= 0; --j)
    {
        if(map[start_location[0]][j] == 0)
        {
            if(map[start_location[0] - 1][j] < map[start_location[0] + 1][j])
            {
                map[start_location[0]][j] = map[start_location[0] - 1][j] + 1;
            }
            else
            {
                map[start_location[0]][j] = map[start_location[0] + 1][j] + 1;
            }
        }
    }
}

/******************************************************************************
markObstacle: Marks an obstacle on the map.

Author: Charles Parsons
******************************************************************************/
void markObstacle(double distance, double angle_from_front)
{
    //need to find out how the obstacles are detected, how far away they are,
    //and where on the map they are in relation to the robot position.

}

/******************************************************************************
wavefrontStep: Returns the next step to be taken in the wavefront algorithm.

Author: Charles Parsons
******************************************************************************/
void wavefrontStep(int** &map, int pos_r, int pos_c, int &next_r, int &next_c)
{
    int i, cost, lowest_cost, lowest_c, lowest_r;

    //OBSTACLE is max int
    lowest_cost = OBSTACLE;

    for(i = 0; i < 8; ++i)
    {
        switch(i)
        {
            case 0:
                cost = map[pos_r - 1][pos_c];
                lowest_c = pos_c;
                lowest_r = pos_r - 1;
                break;
            case 1:
                cost = map[pos_r - 1][pos_c + 1];
                lowest_c = pos_c + 1;
                lowest_r = pos_r - 1;
                break;
            case 2:
                cost = map[pos_r][pos_c + 1];
                lowest_c = pos_c + 1;
                lowest_r = pos_r;
                break;
            case 3:
                cost = map[pos_r + 1][pos_c + 1];
                lowest_c = pos_c + 1;
                lowest_r = pos_r + 1;
                break;
            case 4:
                cost = map[pos_r + 1][pos_c];
                lowest_c = pos_c;
                lowest_r = pos_r + 1;
                break;
            case 5:
                cost = map[pos_r + 1][pos_c - 1];
                lowest_c = pos_c - 1;
                lowest_r = pos_r + 1;
                break;
            case 6:
                cost = map[pos_r][pos_c - 1];
                lowest_c = pos_c - 1;
                lowest_r = pos_r;
                break;
            case 7:
                cost = map[pos_r -1][pos_c -1];
                lowest_c = pos_c - 1;
                lowest_r = pos_r - 1;
                break;
            default:
                break;
        }
        //check to see if the current cost is lower than the lowest cost
        if(cost > 0 && cost < lowest_cost)
        {
            //set current cost to lowest cost
            lowest_cost = cost;
            //save the position of the lowest cost node
            next_c = lowest_c;
            next_r = lowest_r;
        }
    }
}

/******************************************************************************
markPath: this marks the previous step taken in the path.

Author: Charles Parsons
******************************************************************************/
void markPath(int** &map, int x, int y)
{
    map[x][y] = -2;
}

/******************************************************************************
getRowAndCol: determines the size of the map

Author: Charles Parsons
******************************************************************************/
void getRowAndCol(int &row, int &col, int northing_start, int northing_goal,
                  int easting_start, int easting_goal)
{
    col = (int(getXRange(northing_start, northing_goal)) + 1) *
           BOX_WIDTH * 2;
    row = (int(getYRange(easting_start, easting_goal)) + 1) *
           BOX_WIDTH * 2;

}

void mapGpsCallback(const sensor_msgs::NavSatFix::ConstPtr& msg)
{
    //parse message data to get long and lat and store in the globals
    gps_lat = msg.lattitude;
    gps_long = msg.longitude;
}


void obstacleCallback(const std_msgs::String::ConstPtr& msg)
{
    //obstacle data published as x: num meters  z: num meters (distance away)

}

/******************************************************************************
readWaypointFile: reads values from the waypoint file and pushes them into the
 queue of waypoints. values in waypoint file should be stored with lat and long
 on a single line for each point, separated by whitespace.

Author: Charles Parsons
******************************************************************************/
void readWaypointFile(std::ifstream &fin, std::queue<coords> &q)
{
    double latitude, longitude;
    coords c;

    fin.open("waypoints.points");
    while(fin >> latitude >> longitude)
    {
        c.latitude = latitude;
        c.longitude = longitude;
        q.push(c);
    }
    fin.close();
}