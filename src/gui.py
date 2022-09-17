import matplotlib.pyplot as plt
import matplotlib.patches as patches
from PIL import Image
import sys
data_1 = open(sys.argv[1])
net_1 = open(sys.argv[2])
#emib_1 = open(sys.argv[3])
data = data_1.read()
net = net_1.read()
#emib = emib_1.read()
data = data.split("\n")
net = net.split("\n")
#emib = emib.split("\n")
for i in range(len(data)):
	data[i] = data[i].split(" ")
for i in range(len(net)):
	net[i] = net[i].split(" ")
#for i in range(len(emib)):
	#emib[i] = emib[i].split(" ")
fig,ax = plt.subplots()
plt.axis([0,8000,0,8000])
del data[-1]
del net[-1]
#del emib[-1]
color = ['r', 'b', 'g']
counter = 0
for i in data:
	col = color[counter%3]
	obstacle = patches.Rectangle((int(i[1]),int(i[2])), int(i[3]),int(i[4]),fill = False, color=col)
	if sys.argv[4] == "0":
		ax.add_patch(obstacle)
	counter+=1
counter = 0
for i in net:
	col = color[counter%3]
	arrow = patches.ConnectionPatch((int(i[0]),int(i[1])), (int(i[2]),int(i[3])), "data", "data", arrowstyle='-')
	ax.add_patch(arrow)
	counter+=1
	min_x = 0
	max_x = 0
	min_y = 0
	max_y = 0
	if(int(i[0]) < int(i[2])):
		min_x = int(i[0])
		max_x = int(i[2])
	else:
		min_x = int(i[2])
		max_x = int(i[0])
	if(int(i[1]) < int(i[3])):
		min_y = int(i[1])
		max_y = int(i[3])
	else:
		min_y = int(i[3])
		max_y = int(i[1])			
	obstacle = patches.Rectangle((min_x, min_y), max_x - min_x, max_y - min_y,fill = False, color=col, ls='--')
#for i in emib:
#	col = color[counter%3]
#	obstacle = patches.Rectangle((int(i[0]),int(i[1])), int(i[2]),int(i[3]),fill = False, color=col)
#	if sys.argv[4] == "1":
#		ax.add_patch(obstacle)
#	counter+=1
plt.savefig("result.png")



