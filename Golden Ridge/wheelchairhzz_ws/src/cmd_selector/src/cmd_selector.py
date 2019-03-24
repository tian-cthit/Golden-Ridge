#!/usr/bin/env python

import rospy
import message_filters
from std_msgs.msg import Bool
from geometry_msgs.msg import Twist

Nav_vel = Twist()
LFollow_vel = Twist()
LSearch_vel = Twist()
IDLE_vel = Twist()

Nav_bool = Bool()
LFollow_bool = Bool()
LSearch_bool = Bool()
I_bool = Bool()

CMD_Dict = {}
cmd_vel_pub = rospy.Publisher('/cmd_vel', Twist, queue_size=10)


def bools_cb(navigation_bool, lanefollowing_bool, lanesearching_bool, IDLE_bool):
    global Nav_bool, LFollow_bool, LSearch_bool, I_bool
    Nav_bool = navigation_bool
    LFollow_bool = lanefollowing_bool
    LSearch_bool = lanesearching_bool
    I_bool = IDLE_bool


def cmd_cb(navigation_cmd, lanefollowing_cmd, lanesearching_cmd, IDLE_cmd):
    global Nav_vel, LFollow_vel, LSearch_vel, IDLE_vel
    Nav_vel = navigation_cmd
    LFollow_vel = lanefollowing_cmd
    LSearch_vel = lanesearching_cmd
    IDLE_vel = IDLE_cmd

    # add datas to a dictionry
    CMD_Dict['navigation_bool'] = [Nav_bool.data, Nav_vel]
    CMD_Dict['lanefollowing_bool'] = [LFollow_bool.data, LFollow_vel]
    CMD_Dict['lanesearching_bool'] = [LSearch_bool.data, LSearch_vel]
    CMD_Dict['IDLE_bool'] = [I_bool.data, IDLE_vel]

    # serach which bool value == 1 and publish its velocity
    # print(CMD_Dict)
    for cmd in CMD_Dict:
        if CMD_Dict[cmd][0] == 1:
            final_vel = CMD_Dict[cmd][1]
            cmd_vel_pub.publish(final_vel)
            print('publishing:', cmd)
            break


'''
def navigation_bool_cb(navigation_bool):
    global Nav_bool
    Nav_bool = navigation_bool

def lanefollowing_bool_cb(lanefollowing_bool):
    global LFollow_bool
    LFollow_bool = lanefollowing_bool


def lanesearching_bool_cb(lanesearching_bool):
    global LSearch_bool
    LSearch_bool = lanesearching_bool

def IDLE_bool_cb(IDLE_bool):
    global I_bool
    I_bool = IDLE_bool



def navigation_cmd_cb(navigation_cmd):
    global Nav_vel
    Nav_vel = navigation_cmd

def lanefollowing_cmd_cb(lanefollowing_cmd):
    global LFollow_vel
    LFollow_vel = lanefollowing_cmd

def lanesearching_cmd_cb(lanesearching_cmd):
    global LSearch_vel
    LSearch_vel = lanesearching_cmd

def IDLE_cmd_cb(IDLE_cmd):
    global IDLE_vel
    IDLE_vel = IDLE_cmd
    print('ttt')
'''




def main():
    rospy.init_node('cmd_selector')
    '''
    rospy.Subscriber("/FLAG_ObstacleAvoidanceMode", Bool, navigation_bool_cb)
    rospy.Subscriber("/FLAG_LaneFollowingMode", Bool, lanefollowing_bool_cb)
    rospy.Subscriber("/FLAG_LaneSearchingMode", Bool, lanesearching_bool_cb)
    rospy.Subscriber("/FLAG_IdleMode", Bool, IDLE_bool_cb)

    rospy.Subscriber("/navigation_cmd", Twist, navigation_cmd_cb)
    rospy.Subscriber("/lanefollowing_cmd", Twist, lanefollowing_cmd_cb)
    rospy.Subscriber("/lanesearching_cmd", Twist, lanesearching_cmd_cb)
    rospy.Subscriber("/Idle_cmd", Twist, IDLE_cmd_cb)
    '''

    navigation_bool_sub = message_filters.Subscriber('/FLAG_ObstacleAvoidanceMode', Bool)
    lanefollowing_bool_sub = message_filters.Subscriber('/FLAG_LaneFollowingMode', Bool)
    lanesearching_bool_sub = message_filters.Subscriber('/FLAG_LaneSearchingMode', Bool)
    IDLE_bool_sub = message_filters.Subscriber('/FLAG_IdleMode', Bool)

    navigation_cmd_sub = message_filters.Subscriber('/navigation_cmd', Twist)
    lanefollowing_cmd_sub = message_filters.Subscriber('/lanefollowing_cmd', Twist)
    lanesearching_cmd_sub = message_filters.Subscriber('/lanesearching_cmd', Twist)
    IDLE_cmd_sub = message_filters.Subscriber('/Idle_cmd', Twist)

    BOOL_CBS = message_filters.TimeSynchronizer(
        [navigation_bool_sub, lanefollowing_bool_sub, lanesearching_bool_sub, IDLE_bool_sub], 10)
    CMD_CBS = message_filters.TimeSynchronizer(
        [navigation_cmd_sub, lanefollowing_cmd_sub, lanesearching_cmd_sub, IDLE_cmd_sub], 10)

    BOOL_CBS.registerCallback(bools_cb)
    CMD_CBS.registerCallback(cmd_cb)

    rate = rospy.Rate(20)

    while not rospy.is_shutdown():
        #print(Nav_bool)
        '''
        # add datas to a dictionry
        CMD_Dict['navigation_bool'] = [Nav_bool.data, Nav_vel]
        CMD_Dict['lanefollowing_bool'] = [LFollow_bool.data, LFollow_vel]
        CMD_Dict['lanesearching_bool'] = [LSearch_bool.data, LSearch_vel]
        CMD_Dict['IDLE_bool'] = [I_bool.data, IDLE_vel]

        # serach which bool value == 1 and publish its velocity
        #print(CMD_Dict)
        for cmd in CMD_Dict:
            if CMD_Dict[cmd][0] == 1:
                final_vel = CMD_Dict[cmd][1]
                cmd_vel_pub.publish(final_vel)
                print('publishing:', cmd)
                break
        '''
        rospy.spin()
        rate.sleep()


if __name__ == '__main__':
    main()
