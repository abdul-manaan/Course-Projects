###################################################################################################
#   Name : Abdul Manan                                                                            #
#   Roll#  2020-10-0198                                                                           #
#                                                                                                 #
#   Requirements:                                                                                 #
#      -> Python 2.7                                                                              #
#      -> Libraries: random, copy, statistics, os, time                                           #
#      -> Ubuntu or any Linux                                                                     #
#                                                                                                 #
#                                                                                                 #
###################################################################################################
import random
import copy
import statistics as s

TOTAL_CYCLES = 28
POP_SIZE = 20
move = { 1:'Left', 2:'Nothing', 3:'Forward', 4:'Right'}
pos = ['EAST','SOUTH', 'WEST','NORTH']
SIZE = 8

GRID = [[0,0,0,0,0,0,0,0],
        [0,1,2,2,2,2,2,0],
        [0,2,1,1,1,1,2,0],
        [0,2,1,1,1,2,0,0],
        [0,2,1,1,1,2,0,0],
        [0,2,1,1,1,1,2,0],
        [0,2,2,2,2,2,2,0],
        [0,0,0,0,0,0,0,0]]
CUR_POS = [1,1]
FACE = 1
totalGen  = 538
maxFitness = 20 
curMax = 0

#random.seed(1136) # 630
#random.seed(1001) # 466
#random.seed(1026) # 595
random.seed(6214) #19

def initialization():
    global TOTAL_CYCLES
    global POP_SIZE
    population = []
    for i in range(POP_SIZE):
        population.append([])
        for j in range(TOTAL_CYCLES):
            population[i].append(random.randint(1,4)) 
    return population

def isValidMove(x,y,face,grid):
    global SIZE
    newPos = [x,y]
    if x < SIZE and y < SIZE:
        if face == 1:
            newPos[1] += 1
        elif face == 2:
            newPos[0] += 1
        elif face == 3:
            newPos[1] -= 1
        else:
            newPos[0] -= 1
        if grid[newPos[0]][newPos[1]]:
            score = grid[newPos[0]][newPos[1]] -1
            grid[newPos[0]][newPos[1]] = 1
            return score,[newPos[0],newPos[1]]
        else:
            return 0,[x,y]

    return 0, [x,y]


def calcFitness(individual):
    global move
    global GRID
    global CUR_POS
    global FACE
    global maxFitness
    grid = copy.deepcopy(GRID)
    ifitness = 1 
    ipos = CUR_POS
    iface = FACE
    nothingBox = 0
    for i in individual:
        if move[i] == 'Nothing':
            nothingBox += 1
            continue
        elif move[i] == 'Forward':
            res, ipos = isValidMove(ipos[0],ipos[1],iface,grid)
            ifitness += res
        elif move[i] == 'Left':
            iface = (iface-1)
            if not iface:
                iface = 1
        elif move[i] == 'Right':
            iface = iface+1
            if iface == 5:
                iface = 1;
    ifitness = (ifitness - nothingBox*0.4)
    # if grid[2][6] == 2 or grid[5][6] == 2:
        # ifitness -= 5
    return int((ifitness *100)/(maxFitness))

def reproduction(a, b): 
    global TOTAL_CYCLES
    x = random.randint(0,TOTAL_CYCLES-1)
    r = random.uniform(0,1)
    if r > 0.95:
        return a,b
    for i in range(x,len(a)):
        a[i],b[i] = b[i],a[i]
    return list(a),list(b)


def mutation(a):
    global TOTAL_CYCLES
    global POP_SIZE
    j =0
    for i in range(TOTAL_CYCLES-1):
        rr = random.uniform(0,1)
        y = random.randint(1,4)
        if(rr <= 0.029):
            a[i] = y
    return list(a)



def printGen(genNum, pop,fitness):
    global curMax
    print "Generation# ",genNum, " max# ", ((max(fitness))*maxFitness/100)#, " min# ",min(fitness), " mean ", sum(fitness) / float(len(fitness)), " median ", s.median(fitness) 
    curMax = max(curMax,max(fitness))
    

def selection(parent,fitness):
    nextGen = []
    selection = []
    '''
    for i in range(len(fitness)):
        for j in range(fitness[i]):
            selection.append(i)
    while len(nextGen) < POP_SIZE:
        y = random.randint(0,len(selection)-1)
        nextGen.append(copy.deepcopy(parent[selection[y]]))
    '''
    for i in range(20):
        x =  random.randint(0,len(parent)-1)
        y =  random.randint(0,len(parent)-1)
        
        if fitness[x] > fitness[y]:
            nextGen.append(list(parent[x]))
        else:
            nextGen.append(list(parent[y]))
    return list(nextGen)

def getAllFitness(pop):
    fitness = []
    for i in pop:
        fitness.append(calcFitness(i))
    return fitness

def crossAndMutate(pop):
    nextGen = []
    j = len(pop)-1
    for i in range(len(pop)/2):
        n1,n2 = reproduction(pop[i],pop[j])
        n1 = mutation(n1)
        n2 = mutation(n2)
        nextGen.append(n1)
        nextGen.append(n2)
        j -= 1
    return copy.deepcopy(nextGen)

def printTraversal(individual,Gen,fitness):
    global move
    global GRID
    grid = copy.deepcopy(GRID)
    ifitness = 1 
    ipos = CUR_POS
    iface = 1
    nothingBox = 0
    for i in individual:
        if move[i] == 'Nothing':
            nothingBox += 1
            continue
        elif move[i] == 'Forward':
            res, ipos = isValidMove(ipos[0],ipos[1],iface,grid)
            ifitness += res
        elif move[i] == 'Left':
            iface = (iface-1)
            if not iface:
                iface = 1
        elif move[i] == 'Right':
            iface = iface+1
            if iface == 5:
                iface = 1;
        printGrid(grid,Gen,fitness)

def printGrid(grid,gen,fitness):
    import os
    #!/usr/bin/env python
    from time import sleep 
    os.system('clear')
    print "Generation: ", gen," Fitness: ", fitness
    print ""
    for i in grid:
        for j in i:
            if j == 0:
                print "##",
            elif j == 2:
                print "88",
            else:
                print "  ",

        print 
    sleep(0.1)

def main():
    global totalGen
    global curMax
    pop = initialization()
    curGen = 1
    while curGen <= totalGen: 
        curGen += 1
        fitness = getAllFitness(pop)  #Calculating Fitness
        curMax = max(curMax,max(fitness))


        printGen(curGen,pop,fitness)  #Dispaly the Generation Statistics
        if curGen%50 == 0 or curGen == 537:
            i = fitness.index(max(fitness))
            printTraversal(pop[i],curGen,fitness[i])
        #Check if MAX_FITNESS REACHED
        if curGen == 537:
            i = fitness.index(max(fitness))
            print pop[i]
        if max(fitness) >= 100:
            print "MAX FITNESS REACHED"
            break
        
        pop = selection(pop,fitness)  #Selecting next Generation

        pop = list(crossAndMutate(pop)) #Cross-Over and Muatate

    _max = copy.deepcopy(curMax)
    curMax = 0
    return _max


def go():
    for i in range(1000,10000):
        random.seed(i)
        x =  main()
        print i,x
        if (x > 70):
            if x > 95:
                break
main()



