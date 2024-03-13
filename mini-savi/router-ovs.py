"""
import time
import socketserver


class Myserver(socketserver.BaseRequestHandler):
    def handle(self):
        conn = self.request
        while True:
            try:
                content = conn.recv(1024).decode('utf-8')
                print(content)
                conn.send(content.upper().encode('utf-8'))
                time.sleep(0.5)
            except ConnectionResetError:
                break


server = socketserver.ThreadingTCPServer(('127.0.0.1', 12345), Myserver)
server.serve_forever()
"""


#!/usr/bin/python

from mininet.net import Mininet
from mininet.node import Node
from mininet.link import Link
from mininet.log import setLogLevel, info
from mininet.cli import CLI
from mininet.topo import Topo
from mininet.node import RemoteController, OVSSwitch
from mininet.link import TCLink
import time
import socketserver
import os
import math

sat_lines = {}
sat_coor_lines = {}
sat_line_str = ''
sat_coor_line_str = ''
sats_str = {}  # 创建字典
sats_coor_str = {}  # 创建字典
net = None
ISLs = {}
ISLs_origin = {}
hosts = {}
sats = {}


class Myserver(socketserver.BaseRequestHandler):
    sat_line_str = ''
    sat_coor_line_str = ''
    num = 0
    ISL_delays = {} #星间链路延时，为0表示不可见


    def calc_sat_num(self):
        global net

        print("calc sat num")
        print(self.sat_line_str)
        k = 0
        i = 0
        j = 0
        sat_lines = self.sat_line_str.split('\r\n')
        print("sat_lines:", sat_lines)

        for line in sat_lines:  # 按行遍历
            #print("line:",line)
            if(line == 'constellation import end' or line == ''):
                print("found constellation import end")
                continue

            arrs = line.split(' ')
            #print(arrs)

            if(arrs[30] == 'sunlight'): #exclude sunlight
                print("exclude sunlight")
                continue
            
            sats_str[i] = {}
            j = 0
            for arr in arrs:
                if arr != '':
                    #print(arr)
                    sats_str[i][j] = arr
                    j = j + 1
            i = i + 1
        print("---------------------sats_str-----------------")
        print(sats_str)
        self.num = i  # 总卫星数目
        print(i)
        
        myNet(self.num)
        CLI(net)

    def modify_ISL_para(self):
        global ISLs
        global ISLs_origin

        #print("enter modify_ISL_para()")
        #print("self.num")
        #print(self.num)
        #print("---------------print self.ISL_delays---------------------")
        #print(self.ISL_delays)
        #print("---------------print ISLs---------------------")
        #print(ISLs)
        self.num = 66
        for i in range(0, self.num):
             for j in range(0, self.num):
                #print("i=%d, j=%d" % (i, j))
                islx = ISLs[i]
                islx_origin = ISLs_origin[i]
                if ((j in islx_origin.keys()) == False) :
                    continue

                if ((j in islx.keys())) :
                    isl = ISLs[i][j]
                else :
                    if (self.ISL_delays[i][j] != 0) :  #add seen ISL again
                        isl = net.addLink(sats[i], sats[j])
                        ISLs[i][j] = isl
                        d = str(int(self.ISL_delays[i][j] * 1000))
                        print("2. new added ISL delay for ISL[%d][%d] is %s 'ms' : " % (i, j, d))
                        isl.intf1.config(delay = d + 'ms')
                        isl.intf2.config(delay = d + 'ms')
                    continue

                #print(isl)
                if (self.ISL_delays[i][j] != 0) :   #modify ISL delay
                    d = str(int(self.ISL_delays[i][j] * 1000))
                    print("1. new delay for existing ISL[%d][%d] is %s 'ms' : " % (i, j, d))
                    isl.intf1.config(delay = d + 'ms')
                    isl.intf2.config(delay = d + 'ms')
                else :  #remove unseen ISL
                    print("3.delLink unseen ISL[%d][%d]" % (i, j))
                    net.delLink(isl)
                    ISLs[i][j] = None

    def calc_ISL_delay(self):
        print("enter calc_ISL_delay()")
        i = 0
        j = 0
        c = 300000;
        r = 6371.0;
        self.ISL_delays = {}
        for coor_a in sats_coor_str :
            id_a = int(sats_coor_str[coor_a][0])
            x_a = float(sats_coor_str[coor_a][1])
            y_a = float(sats_coor_str[coor_a][2])
            z_a = float(sats_coor_str[coor_a][3])

            self.ISL_delays[id_a-1] = {}  #-1 exclude the sunlight
            for coor_b in sats_coor_str :
                id_b = int(sats_coor_str[coor_b][0])
                x_b = float(sats_coor_str[coor_b][1])
                y_b = float(sats_coor_str[coor_b][2])
                z_b = float(sats_coor_str[coor_b][3])

                if id_a == id_b :
                    self.ISL_delays[id_a-1][id_b-1] = 0;
                    continue

                
                ##https://www.doc88.com/p-0037157447701.html?r=1

                t = (x_a*(x_a - x_b) + y_a*(y_a - y_b) + z_a*(z_a - z_b)) / ((x_a - x_b)**2 + (y_a - y_b)**2 + (z_a - z_b)**2)
                dis_ab = math.sqrt((x_a - x_b)**2 + (y_a - y_b)**2 + (z_a - z_b)**2)
                if t<0 or t>1:
                    self.ISL_delays[id_a-1][id_b-1] = dis_ab / c;

                elif t >= 0 and t <= 1:
                    x_min = x_a + t * (x_b - x_a)
                    y_min = y_a + t * (y_b - y_a)
                    z_min = z_a + t * (z_b - z_a)

                    dis_min_abo = math.sqrt(x_min**2 + y_min**2 + z_min**2)
                    if (r < dis_min_abo) :
                        self.ISL_delays[id_a-1][id_b-1] = dis_ab / c;
                    else :
                        self.ISL_delays[id_a-1][id_b-1] = 0

        #print("---------------print self.ISL_delays---------------------")
        #print(self.ISL_delays)

    def parse_ISL_info(self):
        print("enter parse_ISL_info()")
        #print(self.sat_coor_line_str)
        i = 0
        j = 0
        k = 0
        sat_coor_lines = self.sat_coor_line_str.split('\r\n')
        self.sat_coor_line_str = ''
        #print("sat_coor_lines:", sat_coor_lines)

        for line in sat_coor_lines:  # 按行遍历
            #print("line:",line)
            if(line == 'ISL info start' or line == 'ISL info end' or line == ''):
                #print("found - ", line)
                continue

            arrs = line.split(', ')
            #print("arrs: ", arrs)

            if(arrs[4] == 'sunlight'): #exclude sunlight
                #print("exclude sunlight")
                continue
            
            sats_coor_str[i] = {}
            j = 0
            for arr in arrs:
                if arr != '':
                    #print(arr)
                    sats_coor_str[i][j] = arr
                    j = j + 1
            i = i + 1
        #print("--------------------------sats_coor_str---------------------")
        #print(sats_coor_str)

        self.calc_ISL_delay()
        self.modify_ISL_para()

    def handle(self):
        global net

        conn = self.request
        k = 0
        start_import_flag = 0
        start_calc_links_flag = 0
        while True:
            try:
                content = conn.recv(102400).decode('utf-8')
                #print(content)
                #time.sleep(0.5)
                if (content == 'constellation import start\r\n'):
                    start_import_flag = 1
                    print("start import routers")
                    self.sat_line_str = ''
                elif (content == 'constellation import end\r\n'):
                    print("calc sat num")
                    start_import_flag = 0
                    self.calc_sat_num()
                elif (content == 'forwards\r\n'):
                    print('net start\n')
                    #inet.start()
                elif (content == 'stop\r\n'):
                    print('net stop\n')
                    #net.stop()
                elif (content == 'ISL info start\r\n'):
                    print('recv ISL info start\n')
                    start_calc_links_flag = 1
                    self.sat_coor_line_str = ''
                elif (content == 'ISL info end\r\n'):
                    print('recv ISL info end\n')
                    start_calc_links_flag = 0
                    self.parse_ISL_info()

                else :  
                    if (start_import_flag == 1):
                        print("add sat info")
                        self.sat_line_str = self.sat_line_str + content
                        #print("self.sat_line_str:",self.sat_line_str)
                        if (self.sat_line_str.find("constellation import end") != -1):
                            start_import_flag = 0
                            self.calc_sat_num()
                    if (start_calc_links_flag == 1):
                        print("recv ISL info start2\n")
                        self.sat_coor_line_str = self.sat_coor_line_str + content
                        #print("self.sat_coor_line_str:",self.sat_coor_line_str)
                        if (self.sat_coor_line_str.find("ISL info end") != -1):
                            start_calc_links_flag = 0
                            self.parse_ISL_info()
            except ConnectionResetError:
                break

def myNet(num):

    "Create network from scratch using Open vSwitch."

    global ISLs
    global ISLs_origin
    global net

    if(net != None) :
        net.stop()
        #deleteIntfs??
        '''
        cs = net.controllers
        for c in cs  :
            net.delController(c)
        '''
        hs = net.hosts
        for h in hs  :
            net.delHost(h)
        sws = net.switches
        for s in  sws :
            net.delSwitch(s)
        '''
        ls = net.links
        for l in  ls :
            net.delLink(l)
        '''
    #os.system("mn -c")
    info('--cx----000---------------------------\n')
    c0 = net.addController('c0', controller=RemoteController, ip='192.168.184.131', port=6653)
    info("*** Creating nodes\n")
    print(num)
    if (len(sats_str[num-1]) < 9): #example satellite
        M = 1
        N = 1
        print('construct example satellite')
        return
    else :
        #print(sats_str[num-1][9][0:-1])
        M = int(sats_str[num-1][9][0:-1]) + 1
        N = int(num / M)

    print("num is %d, N is %d, M is %d" % (num, N, M))
    #num = 33 # just for test
    #N = 3
    #M = 11
    for n in range(0, num):
        h = net.addHost(str(n)+ '_host', ip='192.168.123.' + str(n+100) + '/24')
        dpid=hex(n+1)[2:]
        dpid="%16s" % dpid
        dpid = dpid.replace(" ", "0")
        print("---cx----dpid---", dpid)
        s = net.addSwitch(str(n)+'_sat', dpid=hex(n)[2:])

        if (s == None) :
            print("switch add error")
            continue

        net.addLink(h, s)
        hosts[n] = h
        sats[n] = s

    for n in range(0, N): 
        for m in range(0, M):
            ISLs [n * M + m] = {}
            ISLs_origin [n * M + m] = {}

    for n in range(0, N):
        for m in range(0, M):
            isl1 = net.addLink(sats[n * M + m], sats[n * M + (m + 1) % M], bw=10, delay='1ms') #intra-satellite links
            ISLs [n * M + m][n * M + (m + 1) % M] = isl1
            ISLs_origin [n * M + m][n * M + (m + 1) % M] = isl1
            if (n != N - 1) :  # N-1说明没有跨缝的ISL
                isl2 = net.addLink(sats[n * M + m], sats[(n + 1) * M + m], bw=10, delay='1ms')
                ISLs [n * M + m][(n + 1) * M + m] = isl2
                ISLs_origin [n * M + m][(n + 1) * M + m] = isl2

    for n in range(0, N): 
        for m in range(0, M):
            sats[n * M + m].start([c0])

    info('myNet ends\n')
    net.start()


if __name__ == '__main__':
    setLogLevel('info')
    info('*** Scratch network demo (kernel datapath)\n')
    net = Mininet(controller=RemoteController, switch=OVSSwitch, link=TCLink)  # no controller
    info('--cx----111---------------------------\n')
    server = socketserver.ThreadingTCPServer(('127.0.0.1', 12345), Myserver)
    server.serve_forever()

   # myNet()
   # CLI(net)
