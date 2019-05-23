import numpy as np
import sys

## Constants
Layer1 = 784
Layer2 = 30
Layer3 = 10
Epochs = 2
Seed = 2
np.random.seed(Seed)

def readData(dataFile,labelFile):
    with open(dataFile,'r') as f:
        data = map(lambda x: x.replace('[',' ').split(), f.read().split(']'))
        data = np.array(data[:-1]).astype(np.float)
        print(data.shape)
    with open(labelFile,'r') as f:
        labels = np.array(f.read().split()).astype(int)
    return (data,labels)


def sigmoid(output):
    return (1/(1+np.exp(-output)))

def sigmoidDerivative(output):
    return output*(1-output)

## This function will train the neural network
def train():
    data,labels = readData(sys.argv[2],sys.argv[3])
    learningRate = sys.argv[4]

    layer1Weights = []
    layer2Weights = []
    for i in range(Layer2):
        layer1Weights.append(np.random.uniform(-1.0, 1.0, Layer1))
    for i in range(Layer3):
        layer2Weights.append(np.random.uniform(-1,1,Layer2))
    

    layer1Weights = np.array(layer1Weights)
    layer2Weights = np.array(layer2Weights)
    for q in range(Epochs):
        accurate = 0
        for i in range(data.shape[0]):
            templ1 = np.array((data[i] - np.mean(data[i]))/np.std(data[i]))
            l1 = np.dot(layer1Weights,templ1.T) ## 30,784   (784,)
            l1 = np.array(sigmoid(l1)) # (30,)
            # print(l1.shape)
    
            l2 = np.dot(layer2Weights,l1)
            l2 = np.array(sigmoid(l2))
            # print(l2.shape)
            y = np.argmax(l2)
            if(y == labels[i]):
                accurate += 1
    
            t = np.zeros(Layer3)
            t[labels[i]] = 1
            # t = np.array([t]).T

            layer2Error = np.array(l2-t)
            layer2Delta = np.array([layer2Error * sigmoidDerivative(l2)]).T
            layer1Error = np.dot(layer2Error.T, layer2Weights)
            layer1Delta = np.array([layer1Error.T * sigmoidDerivative(l1)])

            # print(layer1Delta.shape, layer2Delta.shape, layer1Error.shape, layer2Error.shape)
            # print (templ1.shape, np.array(layer1Delta).shape)
            layer1Weights -= np.float(learningRate) * (np.dot(np.array(layer1Delta).T, np.array([templ1])))
            layer2Weights -= np.float(learningRate) * (np.dot(np.array(layer2Delta),   np.array([l1])))

        print( "Epoch #%d: Accuracy: %f , Error: %f" % (q+1, float(accurate)/data.shape[0]*100, 100-(float(accurate)/data.shape[0]*100)))

    return layer1Weights,layer2Weights


## Testing the neural Network
def test():
    data,labels = readData(sys.argv[2],sys.argv[3])
    weights = sys.argv[4]
    layer1Weights = []
    layer2Weights = []
    
    with open(weights,'r') as f:
        allWeights = np.load(f)
    layer1Weights = allWeights[0]
    layer2Weights = allWeights[1]
    accurate = 0

    for i in range(data.shape[0]):
        l1 = np.array(np.dot(layer1Weights,data[i].T))
        l1 = np.array(sigmoid(l1))

        l2 = np.dot(layer2Weights,l1)
        l2 = np.array(sigmoid(l2))
        
        y = np.argmax(l2)
        if(y == labels[i]):
            accurate += 1

    print( "Accuracy: %f , Error: %f" % (float(accurate)/data.shape[0]*100, 100-(float(accurate)/data.shape[0]*100)))


## Saving Weights
def saveWeights(l1Weights,l2Weights,filename):
    with open(filename, 'w') as f:
        x= []
        x.append(l1Weights)
        x.append(l2Weights)
        x = np.array(x)
        np.save(f,x)
    print("Successfully saved weights in file %s" % (filename))


## Main Function
def main():
    if len(sys.argv) < 5:
        print("Wrong Arguments")
    elif (sys.argv[1] == "train"):
        l1,l2 = train()
        saveWeights(l1,l2,"netWeights.txt")
    elif (sys.argv[1] == "test"):
        test()
    else:
        print("Wrong Arguments")

main()